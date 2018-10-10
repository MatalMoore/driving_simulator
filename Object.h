#ifndef OBJECT_H
#define OBJECT_H

#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2

class Object {
public:
  Object();

  void reset();

  // read-only fields
  //-----------------------------------------------------------------
  const float* c_position = m_position;
  const float& c_position_x = m_position[0];
  const float& c_position_y = m_position[1];
  const float& c_position_z = m_position[2];

  const float* c_vector_forward = m_vector_forward;
  const float& c_vector_forward_x = m_vector_forward[0];
  const float& c_vector_forward_y = m_vector_forward[1];
  const float& c_vector_forward_z = m_vector_forward[2];

  const float* c_vector_up = m_vector_up;
  const float& c_vector_up_x = m_vector_up[0];
  const float& c_vector_up_y = m_vector_up[1];
  const float& c_vector_up_z = m_vector_up[2];

  const float& c_angle_horizontal = m_angle_horizontal;
  const float& c_angle_vertical = m_angle_vertical;
  //-----------------------------------------------------------------

  virtual void move(const float distance_right, const float distance_up, const float distance_forward);
  virtual void move(const float distance_vector[3]);

  virtual void translate(const float vector_x, const float vector_y, const float vector_z);
  virtual void translate(const float vector[3]);

  virtual void translate_to(const float vector_x, const float vector_y, const float vector_z);
  virtual void translate_to(const float vector[3]);

  virtual void rotate_horizontal(const float angle);
  virtual void rotate_vertical(const float angle);

  void rotate_horizontal_from_vector(const float angle, const float vector[3]);
  void rotate_vertical_from_vector(const float angle, const float vector[3]);

  void set_angle_horizontal(const float angle);
  void set_angle_vertical(const float angle);

protected:
  // the position of the object
  float m_position[3];
  float& m_position_x = m_position[0];
  float& m_position_y = m_position[1];
  float& m_position_z = m_position[2];

  // a normalized vector that defines the forward orientation of the object
  float m_vector_forward[3];
  float& m_vector_forward_x = m_vector_forward[0];
  float& m_vector_forward_y = m_vector_forward[1];
  float& m_vector_forward_z = m_vector_forward[2];

  // a normalized vector that defines the up orientation of the object
  float m_vector_up[3];
  float& m_vector_up_x = m_vector_up[0];
  float& m_vector_up_y = m_vector_up[1];
  float& m_vector_up_z = m_vector_up[2];

  // a normalized vector that defines the right orientation of the object
  float m_vector_right[3];
  float& m_vector_right_x = m_vector_right[0];
  float& m_vector_right_y = m_vector_right[1];
  float& m_vector_right_z = m_vector_right[2];

  // current rotation of the object
  float m_angle_horizontal;
  float m_angle_vertical;

  // used for storage of a rotation matrix instead of allocating one for each method call
  float m_vector_rotation[9];
  float* m_matrix_rotation[3];

private:
  typedef Object __this;
};

#endif