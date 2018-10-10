#include "Acceleration.h"

#include "vector3.h"

Acceleration::Acceleration() {
  m_velocity = 0.0f;
  m_acceleration_current = 0.0f;
  m_friction_factor = DEFAULT_FRICTION_FACTOR;
  m_min_free_velocity = DEFAULT_MIN_FREE_VELOCITY;
}

void Acceleration::set_velocity(const float value) {
  m_velocity = value;
}

void Acceleration::set_acceleration(const float value) {
  m_acceleration_current = value;
}

void Acceleration::set_friction_factor(const float value) {
  m_friction_factor = value;
}

void Acceleration::set_min_free_velocity(const float value) {
  m_min_free_velocity = value;
}

float Acceleration::accelerate(const float seconds) {
  m_velocity = (m_velocity + m_acceleration_current*seconds)*(1.0f - m_friction_factor*seconds);

  if (m_acceleration_current == 0.0f
    && m_velocity < m_min_free_velocity
    && m_velocity > -m_min_free_velocity
    )
  {
    m_velocity = 0.0f;
  }

  return m_velocity;
}