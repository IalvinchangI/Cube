// triangle
void print_linked_triangle(linked_triangle*);  // print triangle

// input
enum input_mode {input_mode_SCAN = 0, input_mode_FILE = 1};
linked_triangle* input(int, ...);  // input data by input_mode(int)