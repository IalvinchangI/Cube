#include<stdio.h>  // io
#include<conio.h>  // getch
#include<stdlib.h>  // malloc
#include<stdarg.h>  // va_list
#include<math.h>

#define PI (3.141592653589)
// #define DEBUG
#define EXECUTE_ON_TERMINAL

/*
z -> right
x -> forward
y -> up
*/


// ==================== data ==================== //
typedef double position[3];
typedef double vector[3];
typedef int vector2_int[2];

double magnitude(vector);
double magnitudeXZ(vector);
double magnitude2_int(vector2_int);
double dot_product(vector, vector);

typedef struct _triangle {
    position pos[3];
    double p0_magnitude;  // distance between pos[0] and O(0, 0, 0)
    vector v1;
    vector v2;
    double v1_magnitude;
    double v2_magnitude;
    vector normal;
    struct _triangle *next;
} triangle;
typedef struct {
    triangle *head;
    triangle *end;
} linked_triangle;

void set_triangle(triangle*);  // set p0, v1, v2, normal, next
void append(linked_triangle*, triangle*);  // insert new triangle at the end of the list
void print_linked_triangle(linked_triangle*);  // print triangle


// ==================== input data ==================== //
enum input_mode {input_mode_SCAN = 0, input_mode_FILE = 1};

linked_triangle* input(int, ...);  // input data by input_mode(int)
void clear_buffer(void);


// ==================== control ==================== //
typedef double polar[3];  // {left-right, up-down, distance}
enum keydown {
    close_window = 27,
    move_left = 'a',
    move_right = 'd',
    move_up = 'w',
    move_down = 's',
    move_forward = '=',
    move_backward = '-'
};

double control(polar);  // get player control and return success or not


// ==================== show ==================== //
#ifdef DEBUG
    #define pixel_type 3
    char pixel[pixel_type] = {' ', '+', '#'};  // empty, line, surface
#else
    #define pixel_type 8
    float color_change_interval = 1.f / (pixel_type - 1);
    // empty, very dark, dark, little dark, middle, little bright, bright, very bright
    char pixel[pixel_type] = {' ', '.', '-', ';', '/', '%', 'X', '@'};
#endif
typedef struct {
    int *pos[2];  // 0 -> high, 1 -> low
    float delta_x;
} line;
typedef struct {
    position pos;
    double distance;
    vector direction;
    vector unit_x;
    vector unit_y;
    double scale;
} camera;
typedef struct {
    char** ground;  // malloc & malloc
    unsigned int width;
    unsigned int height;
} screen;

void set_camera_coord(camera*);  // set unit_x, unit_y, direction
camera* create_camera(position, int);  // create camera and initialize
void clear_screen(screen*);
void move_camera(camera*, polar);  // +-radia, +-distance

void world2screen(vector, camera*, vector2_int);  // convert vector from world coord to screen coord
int set_line(vector2_int, vector2_int, line*);  // return successful or not
void fill(screen*, vector2_int, vector2_int, vector2_int, char);  // fill color
void load(linked_triangle*, camera*, screen*);  // 3D -> 2D

void show(screen*);  // put load onto screen


int main(int argc, char *argv[]) {
    // input
    #ifdef EXECUTE_ON_TERMINAL
        linked_triangle *list = input(input_mode_FILE, argv[1]);
    #else
        linked_triangle *list = input(input_mode_FILE, ".\\input_data.txt");
    #endif
    #ifdef DEBUG
        print_linked_triangle(list);
    #endif

    // show init
    #ifdef EXECUTE_ON_TERMINAL
        camera *player = create_camera((position){0, 0, 10}, atoi(argv[4]));
        unsigned int screen_width = atoi(argv[2]), screen_height = atoi(argv[3]);
    #else
        camera *player = create_camera((position){0, 0, 10}, 100);
        unsigned int screen_width = 237, screen_height = 123;
    #endif
    screen load_screen = {malloc(sizeof(char*) * screen_height), screen_width, screen_height};
    if (load_screen.ground == NULL) exit(1);
    for (int y = 0; y < load_screen.height; y++) {
        load_screen.ground[y] = malloc(sizeof(char) * (screen_width + 1));  // +1 for '\0'
        if (load_screen.ground[y] == NULL) exit(1);
    }
    clear_screen(&load_screen);

    // first load & show
    load(list, player, &load_screen);
    show(&load_screen);

    // loop
    while (1) {
        polar add = {0};
        if (control(add)) {
            move_camera(player, add);
            #ifdef DEBUG
                printf("pos: %15.11lf, %15.11lf, %15.11lf\n", player -> pos[0], player -> pos[1], player -> pos[2]);
                printf("ux : %15.11lf, %15.11lf, %15.11lf\n", player -> unit_x[0], player -> unit_x[1], player -> unit_x[2]);
                printf("uy : %15.11lf, %15.11lf, %15.11lf\n", player -> unit_y[0], player -> unit_y[1], player -> unit_y[2]);
                printf("dir: %15.11lf, %15.11lf, %15.11lf\n\n", player -> direction[0], player -> direction[1], player -> direction[2]);
            #endif
            load(list, player, &load_screen);
            show(&load_screen);
        }
    }
    return 0;
}



// ==================== data ==================== //
double magnitude(vector v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

double magnitudeXZ(vector v) {
    return sqrt(v[0] * v[0] + v[2] * v[2]);
}

double magnitude2_int(vector2_int v) {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

double dot_product(vector v1, vector v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void set_triangle(triangle* node) {  // set p0, v1, v2, normal, next
    for (int i = 0; i < 3; i++) {  // v1 = 0 -> 1; v2 = 0 -> 2
        node -> v1[i] = node -> pos[1][i] - node -> pos[0][i];
        node -> v2[i] = node -> pos[2][i] - node -> pos[0][i];
    }
    // magnitude
    node -> p0_magnitude = magnitude(node -> pos[0]);
    node -> v1_magnitude = magnitude(node -> v1);
    node -> v2_magnitude = magnitude(node -> v2);
    // normal(cross)
    node -> normal[0] = node -> v1[1] * node -> v2[2] - node -> v1[2] * node -> v2[1];
    node -> normal[1] = node -> v1[2] * node -> v2[0] - node -> v1[0] * node -> v2[2];
    node -> normal[2] = node -> v1[0] * node -> v2[1] - node -> v1[1] * node -> v2[0];
    // magnitude of normal vector = 1
    double normal_mag = magnitude(node -> normal);
    for (int i = 0; i < 3; i++) {  
        node -> normal[i] /= normal_mag;
    }
    // next
    node -> next = NULL;
}

void append(linked_triangle* list, triangle* node) {  // insert new triangle at the end of the list
    if (list -> end != NULL) {
        list -> end -> next = node;
    } else {
        list -> head = node;
    }
    list -> end = node;
}

void print_linked_triangle(linked_triangle* list) {  // print triangle
    triangle *current = list -> head;
    for (int i = 1; current != NULL; i++) {
        printf("triangle[%d]\n", i);
        printf("    pos[0]: (% f, % f, % f) %f\n", current -> pos[0][0], current -> pos[0][1], current -> pos[0][2], current -> p0_magnitude);
        printf("    pos[1]: (% f, % f, % f)\n", current -> pos[1][0], current -> pos[1][1], current -> pos[1][2]);
        printf("    pos[2]: (% f, % f, % f)\n", current -> pos[2][0], current -> pos[2][1], current -> pos[2][2]);
        printf("    v1    : (% f, % f, % f) %f\n", current -> v1[0], current -> v1[1], current -> v1[2], current -> v1_magnitude);
        printf("    v2    : (% f, % f, % f) %f\n", current -> v2[0], current -> v2[1], current -> v2[2], current -> v1_magnitude);
        printf("    normal: (% f, % f, % f)\n\n", current -> normal[0], current -> normal[1], current -> normal[2]);
        current = current -> next;
    }
}


// ==================== input data ==================== //
linked_triangle* input(int mode, ...) {  // input data by input_mode(int)
    linked_triangle *list = calloc(1, sizeof(linked_triangle));
    switch (mode) {
        triangle *node;
        int check;  // for scan
        // for FILE
        va_list v;
        FILE *file;
        case input_mode_SCAN:
            for (int count = 1; ; count++) {
                node = malloc(sizeof(triangle));
                if (node == NULL) exit(1);
                printf("triangle[%d]\n", count);
                for (int i = 0; i < 3; i++) {
                    printf("    pos[%d]: ", i);
                    clear_buffer();
                    check = scanf("%lf, %lf, %lf", (node -> pos[i]), (node -> pos[i]) + 1, (node -> pos[i]) + 2);
                    if (check != 3) {
                        free(node);
                        clear_buffer();
                        return list;
                    }
                }
                set_triangle(node);
                append(list, node);
                printf("\n");
            }
            break;
        case input_mode_FILE:
            va_start(v, mode);
            file = fopen(va_arg(v, char*), "r");
            if (file == NULL) exit(1);
            while (1) {
                node = malloc(sizeof(triangle));
                if (node == NULL) exit(1);
                for (int i = 0; i < 3; i++) {
                    check = fscanf(file, "%lf, %lf, %lf", (node -> pos[i]), (node -> pos[i]) + 1, (node -> pos[i]) + 2);
                    if (check != 3) {
                        free(node);
                        return list;
                    }
                }
                set_triangle(node);
                append(list, node);
            }
            fclose(file);
            break;
    }
    return list;
}

void clear_buffer(void) {
    char a;
    while ((a = getchar()) && a != '\n' && a != EOF);
}


// ==================== control ==================== //
double control(polar add) {  // get player control and return success or not
    char key = getch();  // wait for player
    if (key == close_window) exit(0);
    double angle = 0.0625 * PI;
    double distance = 1;
    if (key == move_left) {
        add[0] = -angle;
    } else if (key == move_right) {
        add[0] = angle;
    }
    if (key == move_up) {
        add[1] = angle;
    } else if (key == move_down) {
        add[1] = -angle;
    }
    if (key == move_forward) {
        add[2] = -distance;
    } else if (key == move_backward) {
        add[2] = distance;
    }
    return add[0] + add[1] + add[2];
}


// ==================== show ==================== //
void set_camera_coord(camera *obj) {  // set unit_x, unit_y, direction
    // direction
    for (int i = 0; i < 3; i++) {
        obj -> direction[i] = -(obj -> pos[i] / obj -> distance);
    }
    // unit_x(cross)
    double ratio_ux = magnitudeXZ(obj -> direction);
    obj -> unit_x[0] = -(obj -> direction[2] / ratio_ux);
    obj -> unit_x[1] = 0;
    obj -> unit_x[2] = obj -> direction[0] / ratio_ux;
    // unit_y(cross)
    obj -> unit_y[0] = -(obj -> unit_x[2] * obj -> direction[1]);
    obj -> unit_y[1] = obj -> unit_x[2] * obj -> direction[0] - obj -> unit_x[0] * obj -> direction[2];
    obj -> unit_y[2] = obj -> unit_x[0] * obj -> direction[1];
}

camera* create_camera(position pos, int scale) {  // create camera and initialize
    camera *obj = malloc(sizeof(camera));
    if (obj == NULL) exit(1);
    for (int i = 0; i < 3; i++) {  // pos
        obj -> pos[i] = pos[i];
    }
    obj -> distance = magnitude(obj -> pos);
    obj -> scale = scale;  // scale
    set_camera_coord(obj);  // unit_x, unit_y, direction
    return obj;
}

void clear_screen(screen *obj) {
    for (int y = 0; y < obj -> height; y++) {
        for (int x = 0; x < obj -> width; x++) {
            obj -> ground[y][x] = pixel[0];
        }
        obj -> ground[y][obj -> width] = '\0';
    }
}

void move_camera(camera* obj, polar add) {  // +-radia, +-distance
    position temp = {obj -> pos[0], obj -> pos[1], obj -> pos[2]};
    // distance
    double ratio = (obj -> distance + add[2]) / obj -> distance;
    if (ratio <= 0.0000001 && ratio >= -0.0000001) return;  // stop
    obj -> distance += add[2];
    // left-right
    obj -> pos[0] = temp[2] * sin(add[0]) + temp[0] * cos(add[0]);  // x
    obj -> pos[2] = temp[2] * cos(add[0]) - temp[0] * sin(add[0]);  // z
    // up-down
    double tempXZ_len = magnitudeXZ(temp);
    obj -> pos[1] = tempXZ_len * sin(add[1]) + temp[1] * cos(add[1]);  // y
    double ratioXZ = (tempXZ_len * cos(add[1]) - temp[1] * sin(add[1])) / tempXZ_len * ratio;  // xz
    // ratio
    obj -> pos[0] *= ratioXZ;
    obj -> pos[1] *= ratio;
    obj -> pos[2] *= ratioXZ;
    
    if (obj -> pos[0] <= 0.0000001 && obj -> pos[0] >= -0.0000001 && 
        obj -> pos[2] <= 0.0000001 && obj -> pos[2] >= -0.0000001) {  // stop
            for (int i = 0; i < 3; i++) {
                obj -> pos[i] = temp[i];
            }
            return;
    }
    set_camera_coord(obj);
}

void world2screen(vector in, camera *obj, vector2_int out) {  // convert vector from world coord to screen coord
    double x_dot = dot_product(in, obj -> unit_x);
    double y_dot = dot_product(in, obj -> unit_y);
    out[0] = (int)((obj -> scale / obj -> distance) * x_dot + (x_dot >= 0 ? 0.5 : -0.5));
    out[1] = (int)((obj -> scale / obj -> distance) * y_dot + (y_dot >= 0 ? 0.5 : -0.5));
}

int set_line(vector2_int p0, vector2_int p1, line *out) {  // return successful or not
    if (p0[1] == p1[1]) {  // horizontal
        return 0;
    }
    if (p0[1] > p1[1]) {
        out -> pos[0] = p0;
        out -> pos[1] = p1;
        out -> delta_x = (float)(p1[0] - p0[0]) / (p0[1] - p1[1]);
    } else {
        out -> pos[0] = p1;
        out -> pos[1] = p0;
        out -> delta_x = (float)(p0[0] - p1[0]) / (p1[1] - p0[1]);
    }
    return 1;
}

void fill(screen *load_screen, vector2_int p0, vector2_int p1, vector2_int p2, char color) {  // fill color
    if (p0[1] == p1[1] && p1[1] == p2[1]) {  // all flat
        #ifdef DEBUG
            printf("    failed: all\n");
        #endif
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
                load_screen -> ground[y][x] = pixel[1];
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
    int x_start, x_end, y;
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
    y = lines[scan_lines[0]].pos[0][1];
    x_start = lines[scan_lines[0]].pos[0][0];
    x_end = lines[scan_lines[1]].pos[0][0];
    #ifdef DEBUG
        printf("    start: % d, % d\n", x_start, y);
        printf("    end  : % d, % d\n", x_end, y);
    #endif
    // draw
    int change_line = (failed_index == -1 ? 0 : 1);
    for (int i[2] = {0}; ; i[0]++, i[1]++, y--,
         x_start = lines[scan_lines[0]].pos[0][0] + (int)(lines[scan_lines[0]].delta_x * i[0]),
         x_end   = lines[scan_lines[1]].pos[0][0] + (int)(lines[scan_lines[1]].delta_x * i[1])
    ) {
        if (y < 0) break;
        if (y < load_screen -> height) {
            // int sign = (x_end - x_start >= 0 ? 1 : -1);
            // for (int x = x_start; (x - x_end) * sign < 0; x += sign) {
            for (int x = x_start; x <= x_end; x++) {
                if (x < 0) continue;
                if (x >= load_screen -> width) break;
                #ifdef DEBUG
                    if (x == x_start || x == x_end || y == lines[scan_lines[0]].pos[0][1] || y == lines[scan_lines[0]].pos[1][1]) {
                        load_screen -> ground[y][x] = pixel[1];
                    } else {
                        load_screen -> ground[y][x] = pixel[2];
                    }
                    printf("    print: (% d, % d) : [% d, % d] : %c\n", x, y, x_start, x_end, load_screen -> ground[y][x]);
                #else
                    load_screen -> ground[y][x] = color;
                #endif
            }
        }
        // change line
        if (y == lines[scan_lines[0]].pos[1][1]) {
            i[0] = -1;
            scan_lines[0] ^= scan_lines[1] ^ 3;
            change_line++;
        } else if (y == lines[scan_lines[1]].pos[1][1]) {
            i[1] = -1;
            scan_lines[1] ^= scan_lines[0] ^ 3;
            change_line++;
        }
        // stop
        if (change_line >= 2) break;
    }

}

void load(linked_triangle* list, camera* obj, screen* load_screen) {  // 3D -> 2D
    clear_screen(load_screen);
    int a = 1;
    for (triangle *current = list -> head; current != NULL; current = current -> next, a++) {  // load triangle
        double dot_result = dot_product(current -> normal, obj -> direction);
        if (dot_result >= -0.0000001) {  // do not show
            continue;
        }
        // show
        #ifdef DEBUG
            printf("triangle[%d]\n", a);
        #endif
        // get vector parallel to the screen
        double ratio0 = dot_product(current -> pos[0], obj -> direction) / current -> p0_magnitude;
        double ratio1 = dot_product(current -> v1, obj -> direction) / current -> v1_magnitude;
        double ratio2 = dot_product(current -> v2, obj -> direction) / current -> v2_magnitude;
        vector p0, v1, v2;  // position & vector on screen
        for (int i = 0; i < 3; i++) {
            p0[i] = current -> pos[0][i] - obj -> direction[i] * ratio0;
            v1[i] = current -> v1[i] - obj -> direction[i] * ratio1;
            v2[i] = current -> v2[i] - obj -> direction[i] * ratio2;
        }
        // get vector on the screen coordinate
        vector2_int screen_p0, screen_v1, screen_v2;
        world2screen(p0, obj, screen_p0);
        world2screen(v1, obj, screen_v1);
        world2screen(v2, obj, screen_v2);
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
            for (float level = -color_change_interval; i < pixel_type - 1; i++) {
                if (dot_result >= level) {  // else if
                    break;
                }
                level -= color_change_interval;  // next else if
            }
            color = pixel[i];
        #endif
        fill(load_screen, screen_p0,
             (vector2_int){screen_p0[0] + screen_v1[0], screen_p0[1] + screen_v1[1]},
             (vector2_int){screen_p0[0] + screen_v2[0], screen_p0[1] + screen_v2[1]}, color);
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
