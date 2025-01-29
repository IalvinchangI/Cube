#include<stdlib.h>
#include<stdio.h>
#include"fill.h"

int main(void) {
    screen *load_screen = create_screen(50, 30);
    fill(load_screen,
         (vector2_int){1, 1},
         (vector2_int){20, 20},
         (vector2_int){30, 1},
         '#'
    );
    printf("start............................................|\n");
    show(load_screen);
    printf("end..............................................|\n");
    system("pause");
    fill(load_screen,
         (vector2_int){49, 29},
         (vector2_int){21, 21},
         (vector2_int){31, 2},
         '%'
    );
    printf("start............................................|\n");
    show(load_screen);
    printf("end..............................................|\n");
    return 0;
}