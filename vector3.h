#ifndef VECTOR3_H
#define VECTOR3_H

const static float ZERO_VECTOR[3] = { 0.0f, 0.0f, 0.0f };
const static float UNIT_VECTOR_X[3] = { 1.0f, 0.0f, 0.0f };
const static float UNIT_VECTOR_Y[3] = { 0.0f, 1.0f, 0.0f };
const static float UNIT_VECTOR_Z[3] = { 0.0f, 0.0f, 1.0f };

void normalizeV3(float vector[3]);

float divideV3(const float vector[3], const float dividend[3]);
float side_of_vector_signV3(const float vector[3], const float point[3], const float normal[3]);

// subtracts increment to get below max, then adds increment to get at or above min
#define __bound_float(value, min, max, increment) \
  while (value >= max) { value -= increment; } \
  while (value < min) { value += increment; }

// yields the magnitude of vector
#define __magnitudeV3(vector) \
  sqrtf(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2])

// streams the elements of vector to output
#define __output_vector3(vector, output) \
  output << '[' \
  << ' ' << vector[0] \
  << ' ' << vector[1] \
  << ' ' << vector[2] \
  << " ]";

// yields the distance from vec0 to vec1
#define __distanceV3(vec0, vec1) \
  sqrtf( \
    (vec1[0] - vec0[0])*(vec1[0] - vec0[0]) \
    + (vec1[1] - vec0[1])*(vec1[1] - vec0[1]) \
    + (vec1[2] - vec0[2])*(vec1[2] - vec0[2]) \
  )

// yields the dot product of vec0 and vec1
#define __dot_productV3(vec0, vec1) \
  (vec0[0]*vec1[0] + vec0[1]*vec1[1] + vec0[2]*vec1[2])

// stores the dot product of vec0 and vec1 in vec_store (vec_store should be different from the other paramaters)
#define __cross_productV3(vec0, vec1, vec_store) \
  vec_store[0] = (vec0[1]*vec1[2] - vec0[2]*vec1[1]); \
  vec_store[1] = -(vec0[0]*vec1[2] - vec0[2]*vec1[0]); \
  vec_store[2] = (vec0[0]*vec1[1] - vec0[1]*vec1[0]);

// stores the negation of vector as vec_store (vec_store can be the same as any parameter)
#define __negativeV3(vector, vec_store) \
  vec_store[0] = -vector[0]; \
  vec_store[1] = -vector[1]; \
  vec_store[2] = -vector[2];

// element-wise assignment
#define __vector_element_assign_opV3(vec0, op, vec1) \
  vec0[0] op vec1[0]; \
  vec0[1] op vec1[1]; \
  vec0[2] op vec1[2];

// scalar assignment
#define __vector_scalar_assign_opV3(vector, op, scalar) \
  vector[0] op scalar; \
  vector[1] op scalar; \
  vector[2] op scalar;

// operation on each element of vector and scalar, stored as vec_store (vec_store can be the same as any parameter)
#define __vector_scalar_opV3(vector, op, scalar, vec_store) \
  vec_store[0] = vector[0] op scalar; \
  vec_store[1] = vector[1] op scalar; \
  vec_store[2] = vector[2] op scalar;

// function call on each element of vector, stored as vec_store (vec_store can be the same as any parameter)
#define __vector_scalar_funcV3(func, vector, vec_store) \
  vec_store[0] = func(vector[0]); \
  vec_store[1] = func(vector[1]); \
  vec_store[2] = func(vector[2]);

// element-wise operation on vec0 and vec1, stored as vec_store (vec_store can be the same as any parameter)
#define __vector_element_opV3(vec0, op, vec1, vec_store) \
  vec_store[0] = vec0[0] op vec1[0]; \
  vec_store[1] = vec0[1] op vec1[1]; \
  vec_store[2] = vec0[2] op vec1[2];

// scalar operation on vec1 and scalar, then element-wise operation on vec0 and the result, stored as vec_store (vec_store can be the same as any parameter)
#define __vector_element_op_and_scalar_opV3(vec0, op0, vec1, op1, scalar, vec_store) \
  vec_store[0] = vec0[0] op0 vec1[0] op1 scalar; \
  vec_store[1] = vec0[1] op0 vec1[1] op1 scalar; \
  vec_store[2] = vec0[2] op0 vec1[2] op1 scalar;

// performs matrix multiplication of matrix and vector, stored as vec_store (vec_store should be different from the other paramaters)
#define __matrix_mult_M3x3_V3(matrix, vector, vec_store) \
  vec_store[0] = matrix[0][0]*vector[0] + matrix[0][1]*vector[1] + matrix[0][2]*vector[2]; \
  vec_store[1] = matrix[1][0]*vector[0] + matrix[1][1]*vector[1] + matrix[1][2]*vector[2]; \
  vec_store[2] = matrix[2][0]*vector[0] + matrix[2][1]*vector[1] + matrix[2][2]*vector[2];

#endif