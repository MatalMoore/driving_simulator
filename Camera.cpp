#include "Camera.h"

#include "macro_constants.h"

#include <cmath>
#include <iostream>
using namespace std;

#include "vector3.h"

Camera::Camera() {
  reset();
}

void Camera::reset() {
  __super::reset();

  __vector_element_assign_opV3(m_vector_up_roll, =, UNIT_VECTOR_Y);
  __vector_element_assign_opV3(m_target, =, UNIT_VECTOR_Z);

  m_angle_roll = 0.0f;

  m_angle_vertical_min = DEFAULT_VERTICAL_ANGLE_MIN;
  m_angle_vertical_max = DEFAULT_VERTICAL_ANGLE_MAX;

  m_distance = DEFAULT_DISTANCE_MIN;
  m_distance_min = DEFAULT_DISTANCE_MIN;
}

void Camera::move(const float distance_right, const float distance_up, const float distance_forward) {
  __super::move(distance_right, distance_up, distance_forward);

  m_target_x += m_vector_right_x*distance_right + m_vector_up_x*distance_up + m_vector_forward_x*distance_forward;
  m_target_y += m_vector_right_y*distance_right + m_vector_up_y*distance_up + m_vector_forward_y*distance_forward;
  m_target_z += m_vector_right_z*distance_right + m_vector_up_z*distance_up + m_vector_forward_z*distance_forward;
}
void Camera::move(const float distance_vector[3]) {
  __super::move(distance_vector);

  float temp_vector[3];

  __vector_scalar_opV3(m_vector_right, *, distance_vector[DIM_X], temp_vector);
  __vector_element_assign_opV3(m_target, +=, temp_vector);

  __vector_scalar_opV3(m_vector_up, *, distance_vector[DIM_Y], temp_vector);
  __vector_element_assign_opV3(m_target, +=, temp_vector);

  __vector_scalar_opV3(m_vector_forward, *, distance_vector[DIM_Z], temp_vector);
  __vector_element_assign_opV3(m_target, +=, temp_vector);
}


void Camera::translate(const float vector_x, const float vector_y, const float vector_z) {
  __super::translate(vector_x, vector_y, vector_z);

  m_target_x += vector_x;
  m_target_y += vector_y;
  m_target_z += vector_z;
}
void Camera::translate(const float vector[3]) {
  __super::translate(vector);

  __vector_element_assign_opV3(m_target, +=, vector);
}

void Camera::translate_to(const float vector_x, const float vector_y, const float vector_z) {
  __super::translate_to(vector_x, vector_y, vector_z);

  m_target_x = m_position_x + m_vector_forward_x*m_distance;
  m_target_y = m_position_y + m_vector_forward_y*m_distance;
  m_target_z = m_position_z + m_vector_forward_z*m_distance;
}
void Camera::translate_to(const float vector[3]) {
  __super::translate_to(vector);

  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_forward, *, m_distance, m_target);
}

void Camera::rotate_horizontal(const float angle) {
  __super::rotate_horizontal(angle);

  float temp_vector[3];

  // apply rotation matrix to the roll vector
  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_up_roll, temp_vector);
  __vector_element_assign_opV3(m_vector_up_roll, =, temp_vector);

  // recalculate camera target
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_forward, *, m_distance, m_target);

  // ensure that the roll vector remains normalized (truncation error)
  normalizeV3(m_vector_up_roll);
}

void Camera::rotate_vertical(const float angle) {
  // limits vertical camera rotation
  if (m_angle_vertical + angle > m_angle_vertical_max) {
    __super::rotate_vertical(m_angle_vertical_max - m_angle_vertical);
  }
  else if (m_angle_vertical + angle < m_angle_vertical_min) {
    __super::rotate_vertical(m_angle_vertical_min - m_angle_vertical);
  }
  else {
    __super::rotate_vertical(angle);
  }

  float temp_vector[3];

  // apply rotation matrix to the roll vector
  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_up_roll, temp_vector);
  __vector_element_assign_opV3(m_vector_up_roll, =, temp_vector);

  // recalculate camera target
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_forward, *, m_distance, m_target);

  // ensure that the roll vector remains normalized (truncation error)
  normalizeV3(m_vector_up_roll);
}

void Camera::rotate_roll(const float angle) {
  float cos_of_angle = cosf(angle);
  float sin_of_angle = sinf(angle);

  float temp_vector[3];

  // create rotation matrix
  m_matrix_rotation[0][0] = cos_of_angle + m_vector_forward_x*m_vector_forward_x*(1.0f - cos_of_angle);
  m_matrix_rotation[0][1] = m_vector_forward_x*m_vector_forward_y*(1.0f - cos_of_angle) - m_vector_forward_z*sin_of_angle;
  m_matrix_rotation[0][2] = m_vector_forward_x*m_vector_forward_z*(1.0f - cos_of_angle) + m_vector_forward_y*sin_of_angle;

  m_matrix_rotation[1][0] = m_vector_forward_y*m_vector_forward_z*(1.0f - cos_of_angle) + m_vector_forward_z*sin_of_angle;
  m_matrix_rotation[1][1] = cos_of_angle + m_vector_forward_y*m_vector_forward_y*(1.0f - cos_of_angle);
  m_matrix_rotation[1][2] = m_vector_forward_y*m_vector_forward_z*(1.0f - cos_of_angle) - m_vector_forward_x*sin_of_angle;

  m_matrix_rotation[2][0] = m_vector_forward_z*m_vector_forward_x*(1.0f - cos_of_angle) - m_vector_forward_y*sin_of_angle;
  m_matrix_rotation[2][1] = m_vector_forward_z*m_vector_forward_y*(1.0f - cos_of_angle) + m_vector_forward_x*sin_of_angle;
  m_matrix_rotation[2][2] = cos_of_angle + m_vector_forward_z*m_vector_forward_z*(1.0f - cos_of_angle);

  // apply rotation matrix to roll vector
  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_up_roll, temp_vector);
  __vector_element_assign_opV3(m_vector_up_roll, =, temp_vector);

  // ensure that the roll vector remains normalized (truncation error)
  normalizeV3(m_vector_up_roll);

  // calculate angle between up vector and roll vector
  m_angle_roll = acosf(__dot_productV3(m_vector_up, m_vector_up_roll));
  m_angle_roll *= side_of_vector_signV3(m_vector_up, m_vector_up_roll, m_vector_forward);
}

void Camera::revolve_horizontal(const float angle) {
  float temp_vector[3];

  // store the original target
  __vector_element_assign_opV3(temp_vector, =, m_target);

  // rotate by the negation of the angle
  rotate_horizontal(-angle);

  // translate by the vector from the new target to the old target
  __vector_element_opV3(temp_vector, -, m_target, temp_vector);
  translate(temp_vector);
}
void Camera::revolve_vertical(const float angle) {
  float temp_vector[3];

  // store the original target
  __vector_element_assign_opV3(temp_vector, =, m_target);

  // rotate by the negation of the angle
  rotate_vertical(-angle);

  // translate by the vector from the new target to the old target
  __vector_element_opV3(temp_vector, -, m_target, temp_vector);
  translate(temp_vector);
}

void Camera::revolve_horizontal_from_vector(const float angle, const float vector[3]) {
  float temp_vector[3];

  // store the original target
  __vector_element_assign_opV3(temp_vector, =, m_target);

  // rotate by the negation of the angle
  rotate_horizontal_from_vector(-angle, vector);

  // translate by the vector from the new target to the old target
  __vector_element_opV3(temp_vector, -, m_target, temp_vector);
  translate(temp_vector);
}
void Camera::revolve_vertical_from_vector(const float angle, const float vector[3]) {
  float temp_vector[3];

  // store the original target
  __vector_element_assign_opV3(temp_vector, =, m_target);

  // rotate by the negation of the angle
  rotate_vertical_from_vector(-angle, vector);

  // translate by the vector from the new target to the old target
  __vector_element_opV3(temp_vector, -, m_target, temp_vector);
  translate(temp_vector);
}

void Camera::set_angle_roll(const float angle) {
  rotate_roll(angle - m_angle_roll);
}

void Camera::set_target(const float point_x, const float point_y, const float point_z) {
  float point[3] = { point_x,point_y,point_z };
  set_target(point);
}
void Camera::set_target(const float point[3]) {
  float temp_vector[3];
  float angle_vector[3];
  float normal_vector[3];

  float angle;

  float cos_of_angle;
  float sin_of_angle;

  // find vector projected to XZ-plane from m_position to point
  __vector_element_opV3(point, -, m_position, temp_vector);
  temp_vector[DIM_Y] = 0.0f;
  normalizeV3(temp_vector);

  // find the angle from the Z unit vector to this projected vector
  angle = acosf(__dot_productV3(UNIT_VECTOR_Z, temp_vector));
  angle *= side_of_vector_signV3(UNIT_VECTOR_Z, temp_vector, UNIT_VECTOR_Y);

  if (angle == angle) {
    // rotate horixontally to this angle
    set_angle_horizontal(angle);
  }

  // rotate Z unit vector in the XZ-plane by -m_angle_horizontal
  cos_of_angle = cosf(-m_angle_horizontal);
  sin_of_angle = sinf(-m_angle_horizontal);

  m_matrix_rotation[0][0] = cos_of_angle;
  m_matrix_rotation[0][1] = 0.0f;
  m_matrix_rotation[0][2] = sin_of_angle;

  m_matrix_rotation[1][0] = 0.0f;
  m_matrix_rotation[1][1] = 1.0f;
  m_matrix_rotation[1][2] = 0.0f;

  m_matrix_rotation[2][0] = -sin_of_angle;
  m_matrix_rotation[2][1] = 0.0f;
  m_matrix_rotation[2][2] = cos_of_angle;

  __matrix_mult_M3x3_V3(m_matrix_rotation, UNIT_VECTOR_Z, angle_vector);
  normalizeV3(angle_vector);

  // find vector from m_position to point
  __vector_element_opV3(point, -, m_position, temp_vector);
  normalizeV3(temp_vector);

  // find angle between the rotated Z vector and this vector
  angle = acosf(__dot_productV3(angle_vector, temp_vector));
  __cross_productV3(angle_vector, temp_vector, normal_vector);
  angle *= -side_of_vector_signV3(angle_vector, temp_vector, normal_vector);

  if (angle != angle) {
    angle = 0.0f;
  }

  // rotate vertically to this angle
  set_angle_vertical(angle);

  // set m_target
  __vector_element_assign_opV3(m_target, =, point);

  // recalculate distance
  m_distance = __distanceV3(m_position, m_target);

  // enforce minimum distance
  zoom_distance(0.0f);
}

void Camera::zoom_distance(const float distance) {
  float new_distance;

  if (m_distance - distance < m_distance_min) {
    new_distance = m_distance - m_distance_min;
  }
  else if (m_distance - distance < 0.0f) {
    new_distance = m_distance;
  }
  else {
    new_distance = distance;
  }

  // move m_position forward by new_distance
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_forward, *, new_distance, m_position);

  m_distance -= new_distance;
}
void Camera::set_distance(const float distance) {
  zoom_distance(m_distance - distance);
}