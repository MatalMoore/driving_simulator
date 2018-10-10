#ifndef SHAPE_H
#define SHAPE_H

#include "Object.h"

#define SHAPE_TYPE_CUBOID 0

#define SHAPE_TYPE_DEFAULT SHAPE_TYPE_CUBOID

#define __test_cuboid_plane_outside(point, plane_vector, origin, edge_plane_distance, temp_vector, bool_store) \
      /* find vector to edge of cuboid */ \
      __vector_scalar_opV3(plane_vector, *, edge_plane_distance, temp_vector); \
      /* find point on edge plane of cuboid \ */ \
      __vector_element_opV3(origin, +, temp_vector, temp_vector); \
      /* find vector from the point parameter to this point on the plane */ \
      __vector_element_opV3(point, -, temp_vector, temp_vector); \
      /* test whether dot product of this vector and the plane is positive */ \
      bool_store = __dot_productV3(temp_vector, plane_vector) >= 0.0f;

#define NULL_SHAPE_PTR (Shape*)0

class Shape : public Object {
public:
  Shape(const int shape_type = SHAPE_TYPE_DEFAULT);

  const int& c_shape_type = m_shape_type;

  const float& c_scale_x = m_scale_x;
  const float& c_scale_y = m_scale_y;
  const float& c_scale_z = m_scale_z;

  const float* c_color;
  const float* c_reflectance;
  const float& c_shininess = m_shininess;

  void set_scale(const float scale_x, const float scale_y, const float scale_z);
  void set_scale(const float dimensions[3]);

  void set_scale_x(const float scale_x);
  void set_scale_y(const float scale_y);
  void set_scale_z(const float scale_z);

  void set_color(const float red, const float green, const float blue, const float alpha);
  void set_color(const float color[4]);

  void set_reflectance(const float red, const float green, const float blue, const float alpha);
  void set_reflectance(const float reflectance[4]);

  void set_shininess(const float shininess);

  bool is_point_inside(const float point[3]) const;
  bool is_shape_inside(const Shape& shape) const;

  void draw_GLUT() const;

private:
  // a code that defines what shape the object has
  int m_shape_type;

  float m_scale_x;
  float m_scale_y;
  float m_scale_z;

  float m_color[4];
  float m_reflectance[4];
  float m_shininess;

  void axis_min_max_store(const float axis[3], float& min, float& max) const;
};

#endif