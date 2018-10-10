#include "Object.h"

#include<cmath>
using namespace std;

#include "vector3.h"
#include "macro_constants.h"

Object::Object() {
  reset();
}

void Object::reset() {
  __vector_element_assign_opV3(m_position, =, ZERO_VECTOR);

  __vector_element_assign_opV3(m_vector_right, =, UNIT_VECTOR_X);
  __vector_element_assign_opV3(m_vector_up, =, UNIT_VECTOR_Y);
  __vector_element_assign_opV3(m_vector_forward, =, UNIT_VECTOR_Z);

  m_angle_horizontal = 0.0f;
  m_angle_vertical = 0.0f;

  m_matrix_rotation[0] = m_vector_rotation + 0;
  m_matrix_rotation[1] = m_vector_rotation + 3;
  m_matrix_rotation[2] = m_vector_rotation + 6;
}

void Object::move(const float distance_right, const float distance_up, const float distance_forward) {
  m_position_x += m_vector_right_x*distance_right + m_vector_up_x*distance_up + m_vector_forward_x*distance_forward;
  m_position_y += m_vector_right_y*distance_right + m_vector_up_y*distance_up + m_vector_forward_y*distance_forward;
  m_position_z += m_vector_right_z*distance_right + m_vector_up_z*distance_up + m_vector_forward_z*distance_forward;
}
void Object::move(const float distance_vector[3]) {
  float temp_vector[3];
  __vector_scalar_opV3(m_vector_right, *, distance_vector[DIM_X], temp_vector);
  __vector_element_assign_opV3(m_position, +=, temp_vector);

  __vector_scalar_opV3(m_vector_up, *, distance_vector[DIM_Y], temp_vector);
  __vector_element_assign_opV3(m_position, +=, temp_vector);

  __vector_scalar_opV3(m_vector_forward, *, distance_vector[DIM_Z], temp_vector);
  __vector_element_assign_opV3(m_position, +=, temp_vector);
}


void Object::translate(const float vector_x, const float vector_y, const float vector_z) {
  m_position_x += vector_x;
  m_position_y += vector_y;
  m_position_z += vector_z;
}
void Object::translate(const float vector[3]) {
  __vector_element_assign_opV3(m_position, +=, vector);
}

void Object::translate_to(const float vector_x, const float vector_y, const float vector_z) {
  m_position_x = vector_x;
  m_position_y = vector_y;
  m_position_z = vector_z;
}
void Object::translate_to(const float vector[3]) {
  __vector_element_assign_opV3(m_position, =, vector);
}

void Object::rotate_horizontal(const float angle) {
  float cos_of_angle = cosf(-angle);
  float sin_of_angle = sinf(-angle);

  float temp_vector[3];

  // create simnplified rotation matrix
  m_matrix_rotation[0][0] = cos_of_angle;
  m_matrix_rotation[0][1] = 0.0f;
  m_matrix_rotation[0][2] = sin_of_angle;

  m_matrix_rotation[1][0] = 0.0f;
  m_matrix_rotation[1][1] = 1.0f;
  m_matrix_rotation[1][2] = 0.0f;

  m_matrix_rotation[2][0] = -sin_of_angle;
  m_matrix_rotation[2][1] = 0.0f;
  m_matrix_rotation[2][2] = cos_of_angle;

  // apply rotation matrix to each basis vector
  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_right, temp_vector);
  __vector_element_assign_opV3(m_vector_right, =, temp_vector);

  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_up, temp_vector);
  __vector_element_assign_opV3(m_vector_up, =, temp_vector);

  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_forward, temp_vector);
  __vector_element_assign_opV3(m_vector_forward, =, temp_vector);

  // ensure that the basis vectors remain normalized (truncation error)
  normalizeV3(m_vector_right);
  normalizeV3(m_vector_up);
  normalizeV3(m_vector_forward);

  // project m_vector_forward onto XZ_plane and normalize
  __vector_element_assign_opV3(temp_vector, =, m_vector_forward);
  temp_vector[DIM_Y] = 0.0f;
  normalizeV3(temp_vector);

  // find the angle from the Z unit vector to this projected vector
  m_angle_horizontal = acosf(__dot_productV3(UNIT_VECTOR_Z, temp_vector));
  m_angle_horizontal *= side_of_vector_signV3(UNIT_VECTOR_Z, temp_vector, UNIT_VECTOR_Y);
}

void Object::rotate_vertical(const float angle) {
  float cos_of_angle;
  float sin_of_angle;

  float temp_vector[3];
  float normal_vector[3];
  float angle_vector[3];

  // rotate Z unit vector in the XZ-plane by -m_angle_horizontal + half of pi
  cos_of_angle = cosf(-m_angle_horizontal + HALF_PI);
  sin_of_angle = sinf(-m_angle_horizontal + HALF_PI);

  m_matrix_rotation[0][0] = cos_of_angle;
  m_matrix_rotation[0][1] = 0.0f;
  m_matrix_rotation[0][2] = sin_of_angle;

  m_matrix_rotation[1][0] = 0.0f;
  m_matrix_rotation[1][1] = 1.0f;
  m_matrix_rotation[1][2] = 0.0f;

  m_matrix_rotation[2][0] = -sin_of_angle;
  m_matrix_rotation[2][1] = 0.0f;
  m_matrix_rotation[2][2] = cos_of_angle;

  __matrix_mult_M3x3_V3(m_matrix_rotation, UNIT_VECTOR_Z, normal_vector);
  normalizeV3(normal_vector);

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

  // start rotating the Object
  cos_of_angle = cosf(angle);
  sin_of_angle = sinf(angle);

  // create simplified rotation matrix using the normal vector
  m_matrix_rotation[0][0] = cos_of_angle + normal_vector[DIM_X]*normal_vector[DIM_X]*(1.0f - cos_of_angle);
  m_matrix_rotation[0][1] = -normal_vector[DIM_Z]*sin_of_angle;
  m_matrix_rotation[0][2] = normal_vector[DIM_X]*normal_vector[DIM_Z]*(1.0f - cos_of_angle);

  m_matrix_rotation[1][0] = normal_vector[DIM_Z]*sin_of_angle;
  m_matrix_rotation[1][1] = cos_of_angle;
  m_matrix_rotation[1][2] = -normal_vector[DIM_X]*sin_of_angle;

  m_matrix_rotation[2][0] = normal_vector[DIM_Z]*normal_vector[DIM_X]*(1.0f - cos_of_angle);
  m_matrix_rotation[2][1] = normal_vector[DIM_X]*sin_of_angle;
  m_matrix_rotation[2][2] = cos_of_angle + normal_vector[DIM_Z]*normal_vector[DIM_Z]*(1.0f - cos_of_angle);

  // apply rotation matrix to each basis vector
  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_right, temp_vector);
  __vector_element_assign_opV3(m_vector_right, =, temp_vector);

  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_up, temp_vector);
  __vector_element_assign_opV3(m_vector_up, =, temp_vector);

  __matrix_mult_M3x3_V3(m_matrix_rotation, m_vector_forward, temp_vector);
  __vector_element_assign_opV3(m_vector_forward, =, temp_vector);

  // ensure that the basis vectors remain normalized (truncation error)
  normalizeV3(m_vector_right);
  normalizeV3(m_vector_up);
  normalizeV3(m_vector_forward);

  // find the angle from the angle vector to m_vector_forward
  m_angle_vertical = acosf(__dot_productV3(angle_vector, m_vector_forward));
  m_angle_vertical *= -side_of_vector_signV3(angle_vector, m_vector_forward, m_vector_right);

  if (m_angle_vertical != m_angle_vertical) {
    m_angle_vertical = 0.0f;
  }
}

void Object::rotate_horizontal_from_vector(const float angle, const float vector[3]) {
  float projected_vector[3];
  float new_angle;

  // project vector onto XZ-plane
  __vector_element_assign_opV3(projected_vector, =, vector);
  projected_vector[DIM_Y] = 0.0f;
  normalizeV3(projected_vector);

  // find angle from Z unit vector to this vector
  new_angle = acosf(__dot_productV3(UNIT_VECTOR_Z, projected_vector));
  new_angle *= side_of_vector_signV3(UNIT_VECTOR_Z, projected_vector, UNIT_VECTOR_Y);

  if (new_angle == new_angle) {
    // rotate horizontally by the sum of this angle and the angle parameter
    set_angle_horizontal(new_angle + angle);
  }
}

void Object::rotate_vertical_from_vector(const float angle, const float vector[3]) {
  float projected_vector[3];
  float normal_vector[3];

  float new_angle;

  // project vector onto XZ-plane
  __vector_element_assign_opV3(projected_vector, =, vector);
  projected_vector[DIM_Y] = 0.0f;
  normalizeV3(projected_vector);

  // find angle from projected vector to vector
  new_angle = acosf(__dot_productV3(projected_vector, vector));
  __cross_productV3(projected_vector, vector, normal_vector);
  new_angle *= side_of_vector_signV3(projected_vector, vector, normal_vector);

  if (new_angle != new_angle) {
    new_angle = 0.0f;
  }

  // rotate vertically by the sum of this angle and the angle parameter
  set_angle_vertical(new_angle + angle);
}

void Object::set_angle_horizontal(const float angle) {
  rotate_horizontal(angle - m_angle_horizontal);
}
void Object::set_angle_vertical(const float angle) {
  rotate_vertical(angle - m_angle_vertical);
}