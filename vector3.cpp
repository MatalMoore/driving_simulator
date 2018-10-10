#include "vector3.h"

#include <cmath>
using namespace std;

void normalizeV3(float vector[3]) {
  float dividend = sqrtf(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
  __vector_scalar_opV3(vector, / , dividend, vector);
}

float divideV3(const float vector[3], const float dividend[3]) {
  float quotient;

  // divide and check for NaN
  quotient = vector[0] / dividend[0];
  if (quotient == quotient) {
    return quotient;
  }

  // divide and check for NaN
  quotient = vector[1] / dividend[1];
  if (quotient == quotient) {
    return quotient;
  }

  // divide
  return vector[2] / dividend[2];
}

float side_of_vector_signV3(const float vector[3], const float point[3], const float normal[3]) {
  float cross_vector[3];

  __cross_productV3(point, vector, cross_vector);

  // accounts for truncation error
  return
    (cross_vector[0] * normal[0] >= -1e-6f)
    && (cross_vector[1] * normal[1] >= -1e-6f)
    && (cross_vector[2] * normal[2] >= -1e-6f)
    ? 1.0f : -1.0f;
}