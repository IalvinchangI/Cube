#include<stdlib.h>  // NULL, malloc, free, exit
#include<stdio.h>  // printf, scanf, getchar, EOF, FILE, fopen, fclose, fscanf
#include<stdarg.h>  // va_list, va_start, va_arg, va_end
#include"__object.h"  // linked_triangle, triangle, vector

#include"input.h"
static void set_triangle(triangle*);  // set p0, v1, v2, normal, next
static void append(linked_triangle*, triangle*);  // insert new triangle at the end of the list
static void clear_buffer(void);


// triangle

static void set_triangle(triangle* node) {  // set p0, v1, v2, normal, next
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

static void append(linked_triangle* list, triangle* node) {  // insert new triangle at the end of the list
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



// input

static void clear_buffer(void) {
    char a;
    while ((a = getchar()) && a != '\n' && a != EOF);
}

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
                if (node == NULL) exit(1);  // <error> cannot allocate memory
                printf("triangle[%d]\n", count);
                for (int i = 0; i < 3; i++) {
                    printf("    pos[%d]: ", i);
                    clear_buffer();
                    check = scanf("%lf, %lf, %lf", (node -> pos[i]), (node -> pos[i]) + 1, (node -> pos[i]) + 2);
                    if (check != 3) {  // end scan
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
            if (file == NULL) exit(1);  // <error> cannot open file
            while (1) {
                node = malloc(sizeof(triangle));
                if (node == NULL) exit(1);  // <error> cannot allocate memory
                for (int i = 0; i < 3; i++) {
                    check = fscanf(file, "%lf, %lf, %lf", (node -> pos[i]), (node -> pos[i]) + 1, (node -> pos[i]) + 2);
                    if (check != 3) {  // end scan
                        free(node);
                        goto END_FILE;
                    }
                }
                set_triangle(node);
                append(list, node);
            }
            END_FILE:
            fclose(file);
            va_end(v);
            break;
    }
    return list;
}
