typedef struct {
    char** ground;  // malloc & malloc
    unsigned int width;
    unsigned int height;
} screen;

typedef int vector2_int[2];

// screen
screen* create_screen(unsigned int, unsigned int);  // create screen and initialize
void fill(screen*, vector2_int, vector2_int, vector2_int, char);  // fill color
void show(screen*);  // put load onto screen
