#ifndef CAMERA_OBJECT_H
#define CAMERA_OBJECT_H

#include "Object.h"

#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2

#define DISTANCE_FROM_VERTICAL_ANGLE_LIMIT 0.005f

#define DEFAULT_VERTICAL_ANGLE_MIN -HALF_PI + 0.05f
#define DEFAULT_VERTICAL_ANGLE_MAX HALF_PI - 0.05f

#define DEFAULT_DISTANCE_MIN 10.0f;

class Camera : public Object {
public:
  Camera();

  void reset();

  // read-only fields
  //-----------------------------------------------------------------
  const float* c_target = m_target;
  const float& c_target_x = m_target[0];
  const float& c_target_y = m_target[1];
  const float& c_target_z = m_target[2];

  const float* c_vector_up_roll = m_vector_up_roll;
  const float& c_vector_up_roll_x = m_vector_up_roll[0];
  const float& c_vector_up_roll_y = m_vector_up_roll[1];
  const float& c_vector_up_roll_z = m_vector_up_roll[2];

  const float& c_angle_roll = m_angle_roll;

  const float& c_distance = m_distance;
  //-----------------------------------------------------------------

  void move(const float distance_right, const float distance_up, const float distance_forward);
  void move(const float distance_vector[3]);

  void translate(const float vector_x, const float vector_y, const float vector_z);
  void translate(const float vector[3]);

  void translate_to(const float vector_x, const float vector_y, const float vector_z);
  void translate_to(const float vector[3]);

  void rotate_horizontal(const float angle);
  void rotate_vertical(const float angle);

  void rotate_roll(const float angle);

  void revolve_horizontal(const float angle);
  void revolve_vertical(const float angle);

  void revolve_horizontal_from_vector(const float angle, const float vector[3]);
  void revolve_vertical_from_vector(const float angle, const float vector[3]);

  void set_angle_roll(const float angle);

  void set_target(const float point_x, const float point_y, const float point_z);
  void set_target(const float point[3]);

  void zoom_distance(const float distance);
  void set_distance(const float distance);

private:
  // a normalized vector that defines the up vector that determines the roll
  float m_vector_up_roll[3];
  float& m_vector_up_roll_x = m_vector_up_roll[0];
  float& m_vector_up_roll_y = m_vector_up_roll[1];
  float& m_vector_up_roll_z = m_vector_up_roll[2];

  // the target of the camera
  float m_target[3];
  float& m_target_x = m_target[0];
  float& m_target_y = m_target[1];
  float& m_target_z = m_target[2];

  // current roll rotation of the camera
  float m_angle_roll;

  // vertical angle bounds
  float m_angle_vertical_min;
  float m_angle_vertical_max;

  // distance to target and minimum distance
  float m_distance;
  float m_distance_min;
};

#endif