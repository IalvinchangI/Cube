// collect useful structure
#include"vector.h"

#ifndef __OBJECT_H_
    #define __OBJECT_H_
    // triangle
    typedef struct _triangle {
        position pos[3];
        double p0_magnitude;  // distance between pos[0] and O(0, 0, 0)
        vector v1;  // p0 -> p1
        vector v2;  // p0 -> p2
        double v1_magnitude;
        double v2_magnitude;
        vector normal;
        struct _triangle *next;
    } triangle;
    typedef struct _linked_triangle {
        triangle *head;  // easy to read
        triangle *end;  // easy to append
    } linked_triangle;


    // camera
    typedef struct _camera {
        position pos;
        double distance;  // the distance between the camera and the origin point of the world
        vector direction;  // the camera is facing
        vector unit_x;  // the unit vector(x) of the coordinate of the screen
        vector unit_y;  // the unit vector(y) of the coordinate of the screen
        double scale;  // magnificaton of the view
    } camera;


    // screen
    typedef struct _screen {
        char** ground;  // malloc & malloc
        unsigned int width;
        unsigned int height;
    } screen;
#endif