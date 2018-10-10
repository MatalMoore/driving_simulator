#include <gl/glut.h>
#include <gl/gl.h>

#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "Camera.h"
#include "Shape.h"
#include "Acceleration.h"

#include "macro_constants.h"
#include "vector3.h"
#include "colors.h"

#define ESC_KEY 27
#define SCROLL_WHEEL_FORWARD 3
#define SCROLL_WHEEL_BACK 4

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

#define CAMERA_Y_MIN 0.1f

#define GROUND_THICKNESS 5.0f

#define BOUNDARY_SIZE 200.0f
#define BOUNDARY_HEIGHT 1.0f
#define BOUNDARY_THICKNESS 3.0f

#define WALL_HEIGHT 0.4f

#define __update_fullscreen_dimensions() \
  glGetIntegerv(GL_VIEWPORT, fullscreen_viewport); \
  fullscreen_width = fullscreen_viewport[2] - fullscreen_viewport[0]; \
  fullscreen_height = fullscreen_viewport[3] - fullscreen_viewport[1]; \
  fullscreen_ratio_wh = (float)fullscreen_width / (float)fullscreen_height;

const static float LIGHT_INTENSITY[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
const static float LIGHT_POSITION[4] = { 2.0f, 6.0f, 3.0f, 0.0f };

const static steady_clock frame_clock;
static steady_clock::time_point start_of_last_frame;
static duration<float> time_since_last_frame;
static float average_frame_rate = 0.0f;
static float seconds_since_last_frame;

static bool is_shift_pressed = false;
static bool is_ctrl_pressed = false;
static bool is_alt_pressed = false;

static bool is_exit_pressed = false;

static bool is_go_forward_pressed = false;
static bool is_go_backward_pressed = false;
static bool is_turn_right_pressed = false;
static bool is_turn_left_pressed = false;

// unused key presses
//--------------------------------------------------------------
static bool is_camera_move_right_pressed = false;
static bool is_camera_move_left_pressed = false;
static bool is_camera_move_up_pressed = false;
static bool is_camera_move_down_pressed = false;
static bool is_camera_revolve_right_pressed = false;
static bool is_camera_revolve_left_pressed = false;
static bool is_camera_revolve_up_pressed = false;
static bool is_camera_revolve_down_pressed = false;
static bool is_camera_zoom_in_pressed = false;
static bool is_camera_zoom_out_pressed = false;

static bool is_translate_x_positive_pressed = false;
static bool is_translate_x_negative_pressed = false;
static bool is_translate_y_positive_pressed = false;
static bool is_translate_y_negative_pressed = false;
static bool is_translate_z_positive_pressed = false;
static bool is_translate_z_negative_pressed = false;

static bool is_turn_up_pressed = false;
static bool is_turn_down_pressed = false;
//--------------------------------------------------------------

static unsigned int keyStatus;

static int fullscreen_viewport[4] = { 0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT };
static int fullscreen_width = fullscreen_viewport[2] - fullscreen_viewport[0];
static int fullscreen_height = fullscreen_viewport[3] - fullscreen_viewport[1];
static float fullscreen_ratio_wh = (float)fullscreen_width / (float)fullscreen_height;

static int main_window;

static Camera* main_camera;
static Camera user_camera;
static Camera overhead_camera;

static vector<Shape*> rendered_shapes;
static vector<Shape*> collideable_shapes;

static Shape* main_shape;
static Shape shape_user;

static Shape shape_ground;
static Shape shape_boundary_x_positive;
static Shape shape_boundary_x_negative;
static Shape shape_boundary_z_positive;
static Shape shape_boundary_z_negative;

static Acceleration* main_acceleration;
static Acceleration acceleration_shape_user;

static float move_distance;

void display();
void myMouse(const int button, const int state, const int x, const int y);
void myKeyboard(unsigned char key, const int x, const int y);
void myKeyboardUp(unsigned char key, const int x, const int y);
void mySpecial(const int key, const int x, const int y);
void mySpecialUp(const int key, const int x, const int y);

void handle_input();
void handle_movement();

Shape* get_colliding_shape(const Shape& colliding_shape, const vector<Shape*> collideable_shapes);

int main(int argc, char **argv) {
  string dummy;

  glutInit(&argc, argv);

  cout << "Welcome to Driving Simulator by Matthew Moore." << endl;
  cout << endl;
  cout << "Press ESC to exit." << endl;
  cout << endl;
  cout << "Hold W or the UP arrow to go forward." << endl;
  cout << "Hold S or the DOWN arrow to go backward." << endl;
  cout << "Hold D or the RIGHT arrow to turn right." << endl;
  cout << "Hold A or the LEFT arrow to turn left." << endl;
  cout << endl;
  cout << "Press TAB to switch between camera views." << endl;

  Sleep(2500);

  start_of_last_frame = frame_clock.now();

  // set up window
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  main_window = glutCreateWindow("Driving Simulator");

  // set to fullcreen mode
  glutFullScreen();

  // set display and input functions
  glutDisplayFunc(display);
  glutMouseFunc(myMouse);
  glutKeyboardFunc(myKeyboard);
  glutKeyboardUpFunc(myKeyboardUp);
  glutSpecialFunc(mySpecial);
  glutSpecialUpFunc(mySpecialUp);

  // set shading mode
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  // set up rendering viewport
  glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

  // initialize the main camera
  main_camera = &user_camera;

  // initialize the selected shape and acceleration
  main_shape = &shape_user;
  main_acceleration = &acceleration_shape_user;

  // initialize user_camera
  user_camera.translate_to(0.0f, 3.0f, 15.0f);
  user_camera.set_target(ZERO_VECTOR);

  // initialize overhead_camera
  overhead_camera.translate_to(0.0f, BOUNDARY_SIZE*0.4, 0.0f);
  overhead_camera.set_target(ZERO_VECTOR);

  // initialize shape_user
  shape_user.set_color(COLOR_GREY);
  shape_user.set_reflectance(0.5f, 0.5f, 0.5f, 1.0f);
  shape_user.set_shininess(10.0f);
  shape_user.translate(0.0f, 0.375f, 0.0f);
  shape_user.set_scale(1.75f, 0.75f, 3.0f);
  rendered_shapes.push_back(&shape_user);
  collideable_shapes.push_back(&shape_user);

  // initialize shape_ground
  shape_ground.set_color(COLOR_GRASS_GREEN);
  shape_ground.translate(0.0f, -GROUND_THICKNESS*0.5f, 0.0f);
  shape_ground.set_scale(BOUNDARY_SIZE, GROUND_THICKNESS, BOUNDARY_SIZE);
  rendered_shapes.push_back(&shape_ground);

  // initialize shape_boundary_x_positive
  shape_boundary_x_positive.set_color(COLOR_WALL_GREY);
  shape_boundary_x_positive.translate((BOUNDARY_SIZE + BOUNDARY_THICKNESS)*0.5f, (BOUNDARY_HEIGHT - GROUND_THICKNESS)*0.5f, 0.0f);
  shape_boundary_x_positive.set_scale(BOUNDARY_THICKNESS, BOUNDARY_HEIGHT + GROUND_THICKNESS, BOUNDARY_SIZE + BOUNDARY_THICKNESS);
  rendered_shapes.push_back(&shape_boundary_x_positive);
  collideable_shapes.push_back(&shape_boundary_x_positive);

  // initialize shape_boundary_x_negative
  shape_boundary_x_negative.set_color(COLOR_WALL_GREY);
  shape_boundary_x_negative.translate(-(BOUNDARY_SIZE + BOUNDARY_THICKNESS)*0.5f, (BOUNDARY_HEIGHT - GROUND_THICKNESS)*0.5f, 0.0f);
  shape_boundary_x_negative.set_scale(BOUNDARY_THICKNESS, BOUNDARY_HEIGHT + GROUND_THICKNESS, BOUNDARY_SIZE + BOUNDARY_THICKNESS);
  rendered_shapes.push_back(&shape_boundary_x_negative);
  collideable_shapes.push_back(&shape_boundary_x_negative);

  // initialize shape_boundary_z_positive
  shape_boundary_z_positive.set_color(COLOR_WALL_GREY);
  shape_boundary_z_positive.translate(0.0f, (BOUNDARY_HEIGHT - GROUND_THICKNESS)*0.5f, BOUNDARY_SIZE*0.5f);
  shape_boundary_z_positive.set_scale(BOUNDARY_SIZE, BOUNDARY_HEIGHT + GROUND_THICKNESS, BOUNDARY_THICKNESS);
  rendered_shapes.push_back(&shape_boundary_z_positive);
  collideable_shapes.push_back(&shape_boundary_z_positive);

  // initialize shape_boundary_z_negative
  shape_boundary_z_negative.set_color(COLOR_WALL_GREY);
  shape_boundary_z_negative.translate(0.0f, (BOUNDARY_HEIGHT - GROUND_THICKNESS)*0.5f, -BOUNDARY_SIZE*0.5f);
  shape_boundary_z_negative.set_scale(BOUNDARY_SIZE, BOUNDARY_HEIGHT + GROUND_THICKNESS, BOUNDARY_THICKNESS);
  rendered_shapes.push_back(&shape_boundary_z_negative);
  collideable_shapes.push_back(&shape_boundary_z_negative);

  // start rendering loop
  glutMainLoop();

  return 0;
}

void display() {
  if (is_exit_pressed) {
    exit(NOERROR);
  }

  // update frame rate statistics
  time_since_last_frame = frame_clock.now() - start_of_last_frame;
  seconds_since_last_frame = time_since_last_frame.count();
  average_frame_rate = average_frame_rate*0.7f + (1.0f / time_since_last_frame.count())*0.3f;
  start_of_last_frame = frame_clock.now();

  handle_input();
  handle_movement();

  // set user_camera
  user_camera.set_target(main_shape->c_position);
  user_camera.set_distance(15.0f);
  user_camera.revolve_horizontal_from_vector(0.0f, main_shape->c_vector_forward);
  user_camera.revolve_vertical_from_vector(-PI/32.0f, main_shape->c_vector_forward);
  user_camera.translate(0.0f, 2.0f, 0.0f);

  // set overhead_camera
  overhead_camera.translate_to(shape_user.c_position_x, overhead_camera.c_position_y, shape_user.c_position_z);
  overhead_camera.set_angle_horizontal(shape_user.c_angle_horizontal);

  __update_fullscreen_dimensions();

  glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_INTENSITY);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(30.0, fullscreen_ratio_wh, 0.1, (double)BOUNDARY_SIZE*2.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.40f, 0.55f, 0.9f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set the camera
  gluLookAt(
    main_camera->c_position_x, main_camera->c_position_y, main_camera->c_position_z
    , main_camera->c_target_x, main_camera->c_target_y, main_camera->c_target_z
    , main_camera->c_vector_up_roll_x, main_camera->c_vector_up_roll_y, main_camera->c_vector_up_roll_z
    );

  for (size_t i = 0; i < rendered_shapes.size(); i++) {
    rendered_shapes[i]->draw_GLUT();
  }

  // finish drawing and swap buffers for efficient display
  //   (implicit glFlush())
  glutSwapBuffers();

  // continue drawing
  glutPostRedisplay();
}

void myMouse(const int button, const int state, const int x, const int y) {
  switch (button) {
  default:
    break;
  }
}

void myKeyboard(unsigned char key, const int x, const int y) {
  keyStatus = glutGetModifiers();

  is_shift_pressed = ((keyStatus & GLUT_ACTIVE_SHIFT) != 0);
  is_ctrl_pressed = ((keyStatus & GLUT_ACTIVE_CTRL) != 0);
  is_alt_pressed = ((keyStatus & GLUT_ACTIVE_ALT) != 0);

  switch (key) {
  case ESC_KEY:
    is_exit_pressed = true;
    break;
  case 'd':
  case 'D':
    is_turn_right_pressed = true;
    break;
  case 'a':
  case 'A':
    is_turn_left_pressed = true;
    break;
  case 'w':
  case 'W':
    is_go_forward_pressed = true;
    break;
  case 's':
  case 'S':
    is_go_backward_pressed = true;
    break;
  case '\t':
    if (main_camera == &user_camera) {
      main_camera = &overhead_camera;
    }
    else {
      main_camera = &user_camera;
    }
    break;
  default:
    break;
  }
}

void myKeyboardUp(unsigned char key, const int x, const int y) {
  keyStatus = glutGetModifiers();

  is_shift_pressed = ((keyStatus & GLUT_ACTIVE_SHIFT) != 0);
  is_ctrl_pressed = ((keyStatus & GLUT_ACTIVE_CTRL) != 0);
  is_alt_pressed = ((keyStatus & GLUT_ACTIVE_ALT) != 0);

  switch (key) {
  case 'd':
  case 'D':
    is_turn_right_pressed = false;
    break;
  case 'a':
  case 'A':
    is_turn_left_pressed = false;
    break;
  case 'w':
  case 'W':
    is_go_forward_pressed = false;
    break;
  case 's':
  case 'S':
    is_go_backward_pressed = false;
    break;
  default:
    break;
  }
}

void mySpecial(const int key, const int x, const int y) {
  keyStatus = glutGetModifiers();

  is_shift_pressed = ((keyStatus & GLUT_ACTIVE_SHIFT) != 0);
  is_ctrl_pressed = ((keyStatus & GLUT_ACTIVE_CTRL) != 0);
  is_alt_pressed = ((keyStatus & GLUT_ACTIVE_ALT) != 0);

  switch (key) {
  case GLUT_KEY_RIGHT:
    is_turn_right_pressed = true;
    break;
  case GLUT_KEY_LEFT:
    is_turn_left_pressed = true;
    break;
  case GLUT_KEY_UP:
    is_go_forward_pressed = true;
    break;
  case GLUT_KEY_DOWN:
    is_go_backward_pressed = true;
    break;
  default:
    break;
  }
}

void mySpecialUp(const int key, const int x, const int y) {
  keyStatus = glutGetModifiers();

  is_shift_pressed = ((keyStatus & GLUT_ACTIVE_SHIFT) != 0);
  is_ctrl_pressed = ((keyStatus & GLUT_ACTIVE_CTRL) != 0);
  is_alt_pressed = ((keyStatus & GLUT_ACTIVE_ALT) != 0);

  switch (key) {
  case GLUT_KEY_RIGHT:
    is_turn_right_pressed = false;
    break;
  case GLUT_KEY_LEFT:
    is_turn_left_pressed = false;
    break;
  case GLUT_KEY_UP:
    is_go_forward_pressed = false;
    break;
  case GLUT_KEY_DOWN:
    is_go_backward_pressed = false;
    break;
  default:
    break;
  }
}

Shape* get_colliding_shape(const Shape& colliding_shape, const vector<Shape*> collideable_shapes) {
  for (size_t i = 0; i < collideable_shapes.size(); i++) {
    if (&colliding_shape != collideable_shapes[i]) {
      if (colliding_shape.is_shape_inside(*collideable_shapes[i])) {
        return collideable_shapes[i];
      }
    }
  }

  return NULL_SHAPE_PTR;
}

void handle_input() {
  // is_camera_move_right_pressed
  // is_camera_move_left_pressed
  // is_camera_move_up_pressed
  // is_camera_move_down_pressed
  // is_camera_revolve_right_pressed
  // is_camera_revolve_left_pressed
  // is_camera_revolve_up_pressed
  // is_camera_revolve_down_pressed
  // is_camera_zoom_in_pressed
  // is_camera_zoom_out_pressed

  // is_translate_x_positive_pressed
  // is_translate_x_negative_pressed
  // is_translate_y_positive_pressed
  // is_translate_y_negative_pressed
  // is_translate_z_positive_pressed
  // is_translate_z_negative_pressed

  // is_go_forward_pressed
  // is_go_backward_pressed
  // is_turn_right_pressed
  // is_turn_left_pressed
  // is_turn_up_pressed
  // is_turn_down_pressed

  // horizontal camera movement
  if (is_camera_move_right_pressed == is_camera_move_left_pressed) {
  }
  else if (is_camera_move_right_pressed) {
    main_camera->move(-10.0f*seconds_since_last_frame, 0.0f, 0.0f);
  }
  else {
    main_camera->move(10.0f*seconds_since_last_frame, 0.0f, 0.0f);
  }

  // vertical camera movement
  if (is_camera_move_up_pressed == is_camera_move_down_pressed) {
  }
  else if (is_camera_move_up_pressed) {
    main_camera->move(0.0f, 10.0f*seconds_since_last_frame, 0.0f);
  }
  else {
    main_camera->move(0.0f, -10.0f*seconds_since_last_frame, 0.0f);
    if (main_camera->c_position_y < CAMERA_Y_MIN) {
      main_camera->move(0.0f, 10.0f*seconds_since_last_frame, 0.0f);
    }
  }

  // horizontal camera revolution
  if (is_camera_revolve_right_pressed == is_camera_revolve_left_pressed) {
  }
  else if (is_camera_revolve_right_pressed) {
    main_camera->revolve_horizontal(-2.0f*seconds_since_last_frame);
  }
  else {
    main_camera->revolve_horizontal(2.0f*seconds_since_last_frame);
  }

  // vertical camera revolution
  if (is_camera_revolve_up_pressed == is_camera_revolve_down_pressed) {
  }
  else if (is_camera_revolve_up_pressed) {
    main_camera->revolve_vertical(2.0f*seconds_since_last_frame);
    if (main_camera->c_position_y < CAMERA_Y_MIN) {
      main_camera->revolve_vertical(-2.0f*seconds_since_last_frame);
    }
  }
  else {
    main_camera->revolve_vertical(-2.0f*seconds_since_last_frame);
  }

  // camera zoom
  if (is_camera_zoom_in_pressed == is_camera_zoom_out_pressed) {
  }
  else if (is_camera_zoom_in_pressed) {
    main_camera->zoom_distance(55.0f*seconds_since_last_frame);
  }
  else {
    main_camera->zoom_distance(-55.0f*seconds_since_last_frame);
    if (main_camera->c_position_y < CAMERA_Y_MIN) {
      main_camera->zoom_distance(55.0f*seconds_since_last_frame);
    }
  }

  // translate x
  if (is_translate_x_positive_pressed == is_translate_x_negative_pressed) {
  }
  else if (is_translate_x_positive_pressed) {
    main_shape->translate(5.0f*seconds_since_last_frame, 0.0f, 0.0f);
  }
  else {
    main_shape->translate(-5.0f*seconds_since_last_frame, 0.0f, 0.0f);
  }

  // translate y
  if (is_translate_y_positive_pressed == is_translate_y_negative_pressed) {
  }
  else if (is_translate_y_positive_pressed) {
    main_shape->translate(0.0f, 5.0f*seconds_since_last_frame, 0.0f);
  }
  else {
    main_shape->translate(0.0f, -5.0f*seconds_since_last_frame, 0.0f);
  }

  // translate z
  if (is_translate_z_positive_pressed == is_translate_z_negative_pressed) {
  }
  else if (is_translate_z_positive_pressed) {
    main_shape->translate(0.0f, 0.0f, 5.0f*seconds_since_last_frame);
  }
  else {
    main_shape->translate(0.0f, 0.0f, -5.0f*seconds_since_last_frame);
  }

  // movement
  if (is_go_forward_pressed == is_go_backward_pressed) {
    main_acceleration->set_acceleration(0.0f);
  }
  else if (is_go_forward_pressed) {
    main_acceleration->set_acceleration(30.0f);
  }
  else {
    main_acceleration->set_acceleration(-12.0f);
  }

  // turn horizontal
  if (main_acceleration->c_velocity != 0.0f) {
    if (is_turn_right_pressed == is_turn_left_pressed) {
    }
    else if (is_turn_right_pressed) {
      main_shape->rotate_horizontal(1.0f*seconds_since_last_frame);
      if (get_colliding_shape(*main_shape, collideable_shapes) != NULL_SHAPE_PTR) {
        main_shape->rotate_horizontal(-1.0f*seconds_since_last_frame);
      }
    }
    else {
      main_shape->rotate_horizontal(-1.0f*seconds_since_last_frame);
      if (get_colliding_shape(*main_shape, collideable_shapes) != NULL_SHAPE_PTR) {
        main_shape->rotate_horizontal(1.0f*seconds_since_last_frame);
      }
    }
  }

  // turn vertical
  if (is_turn_up_pressed == is_turn_down_pressed) {
  }
  else if (is_turn_up_pressed) {
    main_shape->rotate_vertical(-2.0f*seconds_since_last_frame);
  }
  else {
    main_shape->rotate_vertical(2.0f*seconds_since_last_frame);
  }
}

void handle_movement() {
  move_distance = acceleration_shape_user.accelerate(seconds_since_last_frame)*seconds_since_last_frame;
  shape_user.move(0.0f, 0.0f, move_distance);
  if (get_colliding_shape(shape_user, collideable_shapes) != NULL_SHAPE_PTR) {
    shape_user.move(0.0f, 0.0f, -move_distance);
    acceleration_shape_user.set_velocity(0.0f);
  }
}