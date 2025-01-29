#ifndef VECTOR_H_
    #define VECTOR_H_
    typedef double position[3];
    typedef double vector[3];
    typedef int vector2_int[2];

    double magnitude(vector);
    double magnitudeXZ(vector);
    double magnitude2_int(vector2_int);
    double dot_product(vector, vector);
#endif