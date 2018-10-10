#ifndef ACCELERATION_H
#define ACCELERATION_H

#define DEFAULT_FRICTION_FACTOR 0.90f
#define DEFAULT_MIN_FREE_VELOCITY 1.0

class Acceleration {
public:
  Acceleration();

  const float& c_velocity = m_velocity;
  const float& c_acceleration_current = m_acceleration_current;
  const float& c_friction_factor = m_friction_factor;
  const float& c_min_free_velocity = m_min_free_velocity;

  void set_velocity(const float value);
  void set_acceleration(const float value);
  void set_friction_factor(const float value);
  void set_min_free_velocity(const float value);

  float accelerate(const float seconds);

private:
  float m_velocity;
  float m_acceleration_current;
  float m_friction_factor;
  float m_min_free_velocity;
};

#endif