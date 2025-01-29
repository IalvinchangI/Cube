#include<stdlib.h>  // NULL, malloc, exit
#include<math.h>  // sin, cos
#include"__object.h"  // camera, vector

// #define DEBUG

#define offset 0.0000001

#include"camera.h"
static void set_camera_coord(camera*);  // set unit_x, unit_y, direction


camera* create_camera(position pos, int scale) {  // create camera and initialize
    camera *obj = malloc(sizeof(camera));
    if (obj == NULL) exit(1);  // <error> cannot allocate memory
    for (int i = 0; i < 3; i++) {  // pos
        obj -> pos[i] = pos[i];
    }
    obj -> distance = magnitude(obj -> pos);
    obj -> scale = scale;  // scale
    set_camera_coord(obj);  // unit_x, unit_y, direction
    return obj;
}

static void set_camera_coord(camera *obj) {  // set unit_x, unit_y, direction
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

void move_camera(camera* obj, polar add) {  // +-radia, +-distance
    position temp = {obj -> pos[0], obj -> pos[1], obj -> pos[2]};

    // distance change
    double ratio = (obj -> distance + add[2]) / obj -> distance;
    if (ratio <= offset && ratio >= -offset) return;  // stop (cannot move)
    obj -> distance += add[2];
    // left-right rotate
    obj -> pos[0] = temp[2] * sin(add[0]) + temp[0] * cos(add[0]);  // x
    obj -> pos[2] = temp[2] * cos(add[0]) - temp[0] * sin(add[0]);  // z
    // up-down rotate
    double tempXZ_len = magnitudeXZ(temp);
    obj -> pos[1] = tempXZ_len * sin(add[1]) + temp[1] * cos(add[1]);  // y
    double ratioXZ = (tempXZ_len * cos(add[1]) - temp[1] * sin(add[1])) / tempXZ_len * ratio;  // xz
    // ratio
    obj -> pos[0] *= ratioXZ;
    obj -> pos[1] *= ratio;
    obj -> pos[2] *= ratioXZ;
    
    // stop (cannot move)
    if (obj -> pos[0] <= offset && obj -> pos[0] >= -offset && 
        obj -> pos[2] <= offset && obj -> pos[2] >= -offset) {
            for (int i = 0; i < 3; i++) {
                obj -> pos[i] = temp[i];
            }
            return;
    }
    // move camera
    set_camera_coord(obj);
    #ifdef DEBUG
        printf("pos: %15.11lf, %15.11lf, %15.11lf\n", player -> pos[0], player -> pos[1], player -> pos[2]);
        printf("ux : %15.11lf, %15.11lf, %15.11lf\n", player -> unit_x[0], player -> unit_x[1], player -> unit_x[2]);
        printf("uy : %15.11lf, %15.11lf, %15.11lf\n", player -> unit_y[0], player -> unit_y[1], player -> unit_y[2]);
        printf("dir: %15.11lf, %15.11lf, %15.11lf\n\n", player -> direction[0], player -> direction[1], player -> direction[2]);
    #endif
}