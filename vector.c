#include<math.h>  // sqrt

#include"vector.h"

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