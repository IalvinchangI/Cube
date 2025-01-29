#include<stdlib.h>  // NULL, malloc, free, exit, system
#include<stdio.h>  // printf
#include"__object.h"

// #define DEBUG
#define offset 0.0000001

#ifdef DEBUG
    #define pixel_type 3
    char pixel[pixel_type] = {' ', '+', '#'};  // empty, line, surface
#else
    #define pixel_type 8
    static float color_change_interval = 1.f / (pixel_type - 1);
    // empty, very dark, dark, little dark, middle, little bright, bright, very bright
    static char pixel[pixel_type] = {' ', '.', '-', ';', '/', '%', 'X', '@'};
#endif
typedef struct {
    int *pos[2];  // 0 -> high, 1 -> low
    float delta_x;
} line;


#include"screen.h"

static void clear_screen(screen*);

static void world2screen(vector, camera*, vector2_int);  // convert vector from world coord to screen coord
static int set_line(vector2_int, vector2_int, line*);  // return successful or not
void fill(screen*, vector2_int, vector2_int, vector2_int, char);  // fill color


screen* create_screen(unsigned int width, unsigned int height) {  // create screen and initialize
    screen* __screen = malloc(sizeof(screen));
    if (__screen == NULL) exit(1);  // <error> cannot allocate memory
    __screen -> width = width;
    __screen -> height = height;
    // ground
    __screen -> ground = malloc(sizeof(char*) * height);
    if (__screen -> ground == NULL) exit(1);  // <error> cannot allocate memory
    for (int y = 0; y < __screen -> height; y++) {
        __screen -> ground[y] = malloc(sizeof(char) * (width + 1));  // +1 for '\0'
        if (__screen -> ground[y] == NULL) exit(1);  // <error> cannot allocate memory
    }
    clear_screen(__screen);
    return __screen;
}

static void clear_screen(screen *obj) {
    for (int y = 0; y < obj -> height; y++) {
        for (int x = 0; x < obj -> width; x++) {
            obj -> ground[y][x] = pixel[0];  // empty
        }
        obj -> ground[y][obj -> width] = '\0';  // end of the string
    }
}

static void world2screen(vector in, camera *obj, vector2_int out) {  // convert vector from world coord to screen coord (matrix multiplication)
    double x_dot = dot_product(in, obj -> unit_x);
    double y_dot = dot_product(in, obj -> unit_y);
    out[0] = (int)((obj -> scale / obj -> distance) * x_dot + (x_dot >= 0 ? 0.5 : -0.5));
    out[1] = (int)((obj -> scale / obj -> distance) * y_dot + (y_dot >= 0 ? 0.5 : -0.5));
}

static int set_line(vector2_int p0, vector2_int p1, line *out) {  // return successful or not
    if (p0[1] == p1[1]) {  // horizontal
        return 0;  // failed
    }
    if (p0[1] > p1[1]) {  // find the higher point
        out -> pos[0] = p0;
        out -> pos[1] = p1;
        out -> delta_x = (float)(p1[0] - p0[0]) / (p0[1] - p1[1]);  // // is equal to "(float)(p0[0] - p1[0]) / (p1[1] - p0[1])"
    } else {
        out -> pos[0] = p1;
        out -> pos[1] = p0;
        out -> delta_x = (float)(p0[0] - p1[0]) / (p1[1] - p0[1]);
    }
    return 1;  // successful
}

void fill(screen *load_screen, vector2_int p0, vector2_int p1, vector2_int p2, char color) {  // fill color
    if (p0[1] == p1[1] && p1[1] == p2[1]) {  // all flat => print line
        #ifdef DEBUG
            printf("    failed: all\n");
        #endif
        // set x, y start
        int y = p0[1];
        int x_start = (p0[0] >= p1[0] ? p0[0] : p1[0]);
        x_start = (x_start >= p2[0] ? x_start : p2[0]);
        int x_end = (p0[0] < p1[0] ? p0[0] : p1[0]);
        x_end = (x_end < p2[0] ? x_end : p2[0]);
        // draw
        for (int x = x_start; x <= x_end; x++) {
            if (x < 0) continue;
            if (x >= load_screen -> width) break;
            #ifdef DEBUG
                load_screen -> ground[y][x] = pixel[1];  // print line
            #else
                load_screen -> ground[y][x] = color;
            #endif
        }
        return;
    }
    // have area
    int *pos[3] = {p0, p1, p2};
    line lines[3];
    int failed_index = -1;
    for (int i = 0; i < 3; i++) {
        if (set_line(pos[i], pos[(i + 1) % 3], lines + i)) {
            continue;
        }
        failed_index = i;
    }
    #ifdef DEBUG
        printf("    failed: % d\n", failed_index);
        printf("             start     , end       , delta_x\n");
        for (int i = 0; i < 3; i++) {
            if (failed_index == i) continue;
            printf("    line[%d]: (% 3d, % 3d), (% 3d, % 3d), % 6.4f\n", i, lines[i].pos[0][0], lines[i].pos[0][1], lines[i].pos[1][0], lines[i].pos[1][1], lines[i].delta_x);
        }
    #endif
    // find the highest lines
    int scan_lines[2] = {0};  // 0 = left, 1 = right
    if (failed_index == -1) {  // all success
        if (lines[0].pos[0][1] == lines[1].pos[0][1]) {
            scan_lines[1] = 1;
        } else if (lines[0].pos[0][1] > lines[1].pos[0][1]) {
            scan_lines[1] = 2;
        } else {
            scan_lines[0] = 1;
            scan_lines[1] = 2;
        }
    } else {  // one flat
        scan_lines[0] = (failed_index + 1) % 3;
        scan_lines[1] = (failed_index + 2) % 3;
    }
    if (lines[scan_lines[0]].pos[0][0] > lines[scan_lines[1]].pos[0][0] ||  // start_x is different(left > right)
        (lines[scan_lines[0]].pos[0][0] == lines[scan_lines[1]].pos[0][0] && lines[scan_lines[0]].delta_x > lines[scan_lines[1]].delta_x)  // start_x is equal & delta_x is different(left > right)
        ) {  // exchange
            scan_lines[0] ^= scan_lines[1] ^= scan_lines[0] ^= scan_lines[1];
    }
    #ifdef DEBUG
        printf("    scan_lines: %d, %d\n", scan_lines[0], scan_lines[1]);
    #endif
    // set x, y start
    int x_start, x_end, y;  // y = y_start
    y = lines[scan_lines[0]].pos[0][1];
    x_start = lines[scan_lines[0]].pos[0][0];
    x_end = lines[scan_lines[1]].pos[0][0];
    #ifdef DEBUG
        printf("    start: % d, % d\n", x_start, y);
        printf("    end  : % d, % d\n", x_end, y);
    #endif
    // draw
    int change_line = (failed_index == -1 ? 0 : 1);  // 0 -> can change, 1 -> cannot change(if change, the for loop will break)
    for (int i[2] = {0}; ; i[0]++, i[1]++,
         y--,  // change scan line
         x_start = lines[scan_lines[0]].pos[0][0] + (int)(lines[scan_lines[0]].delta_x * i[0]),  // change the point start to fill
         x_end   = lines[scan_lines[1]].pos[0][0] + (int)(lines[scan_lines[1]].delta_x * i[1])   // change the point end   to fill
    ) {
        if (y < 0) break;  // out of range
        if (y < load_screen -> height) {
            // int sign = (x_end - x_start >= 0 ? 1 : -1);
            // for (int x = x_start; (x - x_end) * sign < 0; x += sign) {  // "if (x >= load_screen -> width)" need change
            for (int x = x_start; x <= x_end; x++) {  // scan line fill
                if (x < 0) continue;  // out of range
                if (x >= load_screen -> width) break;  // out of range
                #ifdef DEBUG
                    if (x == x_start || x == x_end || y == lines[scan_lines[0]].pos[0][1] || y == lines[scan_lines[0]].pos[1][1]) {
                        load_screen -> ground[y][x] = pixel[1];  // print line
                    } else {
                        load_screen -> ground[y][x] = pixel[2];  // print surface
                    }
                    printf("    print: (% d, % d) : [% d, % d] : %c\n", x, y, x_start, x_end, load_screen -> ground[y][x]);
                #else
                    load_screen -> ground[y][x] = color;
                #endif
            }
        }  // else: out of range
        // change line?
        for (int j = 0; j < 2; j++) {
            if (y == lines[scan_lines[j]].pos[1][1]) {
                i[j] = -1;
                scan_lines[j] ^= scan_lines[j ^ 1] ^ 3;  // j = 0 => j ^ 1 = 1; j = 1 => j ^ 1 = 0
                change_line++;
                break;
            }
        }
        // stop?
        if (change_line >= 2) break;  // all lines are scanned
    }

}

void load(linked_triangle* list, camera* obj, screen* load_screen) {  // 3D -> 2D
    clear_screen(load_screen);
    #ifdef DEBUG
        int a = 1;
    #endif
    for (triangle *current = list -> head; current != NULL; current = current -> next) {  // load triangle
        double dot_result = dot_product(current -> normal, obj -> direction);
        if (dot_result >= -offset) {  // do not show
            continue;
        }
        // show
        #ifdef DEBUG
            printf("triangle[%d]\n", a);
            a++;
        #endif
        // get vector which is parallel to the screen
        double ratio0 = dot_product(current -> pos[0], obj -> direction) / current -> p0_magnitude;
        double ratio1 = dot_product(current -> v1, obj -> direction) / current -> v1_magnitude;
        double ratio2 = dot_product(current -> v2, obj -> direction) / current -> v2_magnitude;
        vector p0, v1, v2;  // 3D position & vector parallel to screen
        for (int i = 0; i < 3; i++) {
            p0[i] = current -> pos[0][i] - obj -> direction[i] * ratio0;  // 3D position 0 parallel to screen
            v1[i] = current -> v1[i] - obj -> direction[i] * ratio1;  // 3D vector 1 parallel to screen
            v2[i] = current -> v2[i] - obj -> direction[i] * ratio2;  // 3D vector 2 parallel to screen
        }
        // get vector on the screen coordinate
        vector2_int screen_p0, screen_v1, screen_v2;  // 2D position & vector on screen
        world2screen(p0, obj, screen_p0);  // 2D position 0 on screen
        world2screen(v1, obj, screen_v1);  // 2D vector 1 on screen
        world2screen(v2, obj, screen_v2);  // 2D vector 2 on screen
        // move the origin point of the screen coordinate to the lower left corner
        screen_p0[0] += load_screen -> width / 2;
        screen_p0[1] += load_screen -> height / 2;
        #ifdef DEBUG
            printf("    screen_p0: (% d, % d)\n", screen_p0[0], screen_p0[1]);
            printf("    screen_v1: (% d, % d)\n", screen_v1[0], screen_v1[1]);
            printf("    screen_v2: (% d, % d)\n", screen_v2[0], screen_v2[1]);
        #endif
        // fill pixel
        char color = 0;
        #ifndef DEBUG
            int i = 1;
            // set color by the angle between direction of the camera and normal vector of the triangle
            for (float level = -color_change_interval; i < pixel_type - 1; i++) {
                if (dot_result >= level) break;  // if or else if
                level -= color_change_interval;  // next else if or else
            }
            color = pixel[i];
        #endif
        fill(load_screen, screen_p0,  // p0
             (vector2_int){screen_p0[0] + screen_v1[0], screen_p0[1] + screen_v1[1]},  // p1
             (vector2_int){screen_p0[0] + screen_v2[0], screen_p0[1] + screen_v2[1]}, color);  // p2
    }
}

void show(screen* load_screen) {  // put load onto screen
    #ifndef DEBUG
        system("CLS");
    #endif
    for (int y = load_screen -> height - 1; y >= 0 ; y--) {
        printf("%s\n", load_screen -> ground[y]);
    }
}