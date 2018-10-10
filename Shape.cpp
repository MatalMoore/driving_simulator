#include "Shape.h"

#include <gl/glut.h>
#include <gl/gl.h>

#include "macro_constants.h"
#include "vector3.h"
#include "colors.h"

#define DRAW_AXES 0

Shape::Shape(const int shape_type) {
  m_shape_type = shape_type;

  m_scale_x = 1.0f;
  m_scale_y = 1.0f;
  m_scale_z = 1.0f;

  m_color[0] = 0.0f;
  m_color[1] = 0.0f;
  m_color[2] = 0.0f;
  m_color[3] = 1.0f;

  m_reflectance[0] = 0.0f;
  m_reflectance[1] = 0.0f;
  m_reflectance[2] = 0.0f;
  m_reflectance[3] = 1.0f;

  m_shininess = 0.0f;
}

void Shape::set_scale(const float scale_x, const float scale_y, const float scale_z) {
  m_scale_x = scale_x;
  m_scale_y = scale_y;
  m_scale_z = scale_z;
}
void Shape::set_scale(const float dimensions[3]) {
  m_scale_x = dimensions[0];
  m_scale_y = dimensions[1];
  m_scale_z = dimensions[2];
}

void Shape::set_scale_x(const float scale_x) {
  m_scale_x = scale_x;
}
void Shape::set_scale_y(const float scale_y) {
  m_scale_y = scale_y;
}
void Shape::set_scale_z(const float scale_z) {
  m_scale_z = scale_z;
}

void Shape::set_color(const float red, const float green, const float blue, const float alpha) {
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = alpha;
}
void Shape::set_color(const float color[4]) {
  m_color[0] = color[0];
  m_color[1] = color[1];
  m_color[2] = color[2];
  m_color[3] = color[3];
}

void Shape::set_reflectance(const float red, const float green, const float blue, const float alpha) {
  m_reflectance[0] = red;
  m_reflectance[1] = green;
  m_reflectance[2] = blue;
  m_reflectance[3] = alpha;
}
void Shape::set_reflectance(const float reflectance[4]) {
  m_reflectance[0] = reflectance[0];
  m_reflectance[1] = reflectance[1];
  m_reflectance[2] = reflectance[2];
  m_reflectance[3] = reflectance[3];
}

void Shape::set_shininess(const float shininess) {
  m_shininess = shininess;
}

bool Shape::is_point_inside(const float point[3]) const {
  float temp_vector[3];
  float plane_distance;

  bool is_inside = true;
  bool is_on_positive_normal_side;
  bool is_on_negative_normal_side;

  switch (m_shape_type) {
    case SHAPE_TYPE_CUBOID:
      // find if point is on the far side of the right cuboid plane
      plane_distance = m_scale_x;
      __test_cuboid_plane_outside(point, m_vector_right, m_position, plane_distance, temp_vector, is_on_positive_normal_side);

      // find if point is on the far side of the left cuboid plane
      plane_distance = -m_scale_x;
      __test_cuboid_plane_outside(point, m_vector_right, m_position, plane_distance, temp_vector, is_on_negative_normal_side);

      // find if the point is between these two planes
      is_inside &= (!is_on_positive_normal_side == is_on_negative_normal_side);

      // -------------------------------------------------------

      // find if point is on the far side of the up cuboid plane
      plane_distance = m_scale_y;
      __test_cuboid_plane_outside(point, m_vector_up, m_position, plane_distance, temp_vector, is_on_positive_normal_side);

      // find if point is on the far side of the down cuboid plane
      plane_distance = -m_scale_y;
      __test_cuboid_plane_outside(point, m_vector_up, m_position, plane_distance, temp_vector, is_on_negative_normal_side);

      // find if the point is between these two planes
      is_inside &= (!is_on_positive_normal_side == is_on_negative_normal_side);

      // -------------------------------------------------------

      // find if point is on the far side of the forward cuboid plane
      plane_distance = m_scale_z;
      __test_cuboid_plane_outside(point, m_vector_forward, m_position, plane_distance, temp_vector, is_on_positive_normal_side);

      // find if point is on the far side of the backward cuboid plane
      plane_distance = -m_scale_z;
      __test_cuboid_plane_outside(point, m_vector_forward, m_position, plane_distance, temp_vector, is_on_negative_normal_side);

      // find if the point is between these two planes
      is_inside &= (!is_on_positive_normal_side == is_on_negative_normal_side);

      return is_inside;

  default:
    return false;
  }
}

bool Shape::is_shape_inside(const Shape& shape) const {
  bool is_inside;

  float this_axis_min;
  float this_axis_max;

  float shape_axis_min;
  float shape_axis_max;

  switch (m_shape_type) {
  case SHAPE_TYPE_CUBOID:
    is_inside = true;

    // get min and max point values along the right vector of this
    axis_min_max_store(m_vector_right, this_axis_min, this_axis_max);
    shape.axis_min_max_store(m_vector_right, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    // get min and max point values along the up vector of this
    axis_min_max_store(m_vector_up, this_axis_min, this_axis_max);
    shape.axis_min_max_store(m_vector_up, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    // get min and max point values along the forward vector of this
    axis_min_max_store(m_vector_forward, this_axis_min, this_axis_max);
    shape.axis_min_max_store(m_vector_forward, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    // get min and max point values along the right vector of shape
    axis_min_max_store(shape.m_vector_right, this_axis_min, this_axis_max);
    shape.axis_min_max_store(shape.m_vector_right, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    // get min and max point values along the up vector of shape
    axis_min_max_store(shape.m_vector_up, this_axis_min, this_axis_max);
    shape.axis_min_max_store(shape.m_vector_up, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    // get min and max point values along the forward vector of shape
    axis_min_max_store(shape.m_vector_forward, this_axis_min, this_axis_max);
    shape.axis_min_max_store(shape.m_vector_forward, shape_axis_min, shape_axis_max);

    // check whether they intersect
    is_inside &= (this_axis_min < shape_axis_max && shape_axis_min < this_axis_max);

    return is_inside;

  default:
    return false;
  }
}

void Shape::draw_GLUT() const {
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, m_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_reflectance);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &m_shininess);

  glPushMatrix();

  glTranslatef(m_position_x, m_position_y, m_position_z);
  glRotatef(m_angle_vertical*RAD_TO_DEG, m_vector_right_x, 0.0f, m_vector_right_z);
  glRotatef(-m_angle_horizontal*RAD_TO_DEG, 0.0f, 1.0f, 0.0f);
  glScalef(m_scale_x, m_scale_y, m_scale_z);

  switch (m_shape_type) {
  case SHAPE_TYPE_CUBOID:
    glutSolidCube(1.0);
    break;
  default:
    break;
  }

  glPopMatrix();

#if DRAW_AXES
  glPushMatrix();
  glTranslatef(m_position_x, m_position_y, m_position_z);

  glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, COLOR_RED);
  glTranslatef(m_vector_right_x*2.0f, m_vector_right_y*2.0f, m_vector_right_z*2.0f);
  glScalef(0.2f, 0.2f, 0.2f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, COLOR_GREEN);
  glTranslatef(m_vector_up_x*2.0f, m_vector_up_y*2.0f, m_vector_up_z*2.0f);
  glScalef(0.2f, 0.2f, 0.2f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, COLOR_BLUE);
  glTranslatef(m_vector_forward_x*2.0f, m_vector_forward_y*2.0f, m_vector_forward_z*2.0f);
  glScalef(0.2f, 0.2f, 0.2f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPopMatrix();
#endif
}

void Shape::axis_min_max_store(const float axis[3], float& min, float& max) const {
  float cuboid_corner_point[3];

  float offset_x = m_scale_x * 0.5f;
  float offset_y = m_scale_y * 0.5f;
  float offset_z = m_scale_z * 0.5f;

  float projected_length;

  // find right-up-forward corner
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // initialize min and max
  min = projected_length;
  max = projected_length;

  //----------------------------------------------------------------------------------

  // find right-up-backward corner
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find right-down-forward corner
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find right-down-backward corner
  __vector_element_op_and_scalar_opV3(m_position, +, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find left-up-forward corner
  __vector_element_op_and_scalar_opV3(m_position, -, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find left-up-backward corner
  __vector_element_op_and_scalar_opV3(m_position, -, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find left-down-forward corner
  __vector_element_op_and_scalar_opV3(m_position, -, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, +, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }

  //----------------------------------------------------------------------------------

  // find left-down-backward corner
  __vector_element_op_and_scalar_opV3(m_position, -, m_vector_right, *, offset_x, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_up, *, offset_y, cuboid_corner_point);
  __vector_element_op_and_scalar_opV3(cuboid_corner_point, -, m_vector_forward, *, offset_z, cuboid_corner_point);

  // find its projected length along axis
  projected_length = __dot_productV3(axis, cuboid_corner_point);

  // update min and max
  if (min > projected_length) {
    min = projected_length;
  }
  if (max < projected_length) {
    max = projected_length;
  }
}