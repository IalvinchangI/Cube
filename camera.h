typedef double polar[3];  // {left-right, up-down, distance}

camera* create_camera(position, int);  // create camera and initialize
void move_camera(camera*, polar);  // +-radia, +-distance