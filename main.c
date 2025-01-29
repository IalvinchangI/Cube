// #define DEBUG
#define EXECUTE_ON_TERMINAL

#include<stdlib.h>  // atoi, exit
#include<conio.h>  // getch
#include"object.h"
#include"vector.h"
#include"input.h"
#include"camera.h"
#include"screen.h"


#define PI (3.141592653589)

/*  3D coordinate
z -> right
x -> forward
y -> up
*/

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
        screen *load_screen = create_screen(atoi(argv[2]), atoi(argv[3]));
    #else
        camera *player = create_camera((position){0, 0, 10}, 100);
        screen *load_screen = create_screen(237, 123);
    #endif
    // first load & show
    load(list, player, load_screen);
    show(load_screen);

    // loop
    while (1) {
        polar add = {0};
        if (control(add)) {
            move_camera(player, add);
            load(list, player, load_screen);
            show(load_screen);
        }
    }
    return 0;
}

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