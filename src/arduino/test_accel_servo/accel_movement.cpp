#include "Arduino.h"

#include "accel_movement.h"

const double accel_movement::default_acceleration_fraction = 0.4;   // 10% of total movement time is for acceleration and 10% for deceleration

accel_movement::accel_movement()
{
  active = 0;
}

void accel_movement::set_max_speed(double max_speed)
{
  this->max_speed = max_speed;
}

void debug_prn(const char *msg, double val)
{
/*    Serial.print(msg);
  Serial.print(": ");
  Serial.println(val);*/
}

void accel_movement::setup_first(double position_start, double position_end,
           double time_start, double time_end,
           double next_position_start, double next_position_end,
           double next_time_start, double next_time_end,
           double acceleration_fraction)
{
   double speed_end = 0;
   if ((next_position_end > next_position_start) == (position_end > position_start))
      speed_end = (fabs(next_position_end - next_position_start) /
                   (next_time_end - next_time_start) +
                   fabs(position_end - position_start) /
                   (time_end - time_start)) / 2;
   setup(position_start, position_end,
         time_start, time_end,
          0, speed_end,
          acceleration_fraction);
}

void accel_movement::setup_last(double prev_position_start, double prev_position_end,
           double prev_time_start, double prev_time_end,
           double position_start, double position_end,
           double time_start, double time_end,
           double acceleration_fraction)
{
   double speed_start = 0;
   if ((prev_position_end > prev_position_start) == (position_end > position_start))
      speed_start = (fabs(prev_position_end - prev_position_start) /
                   (prev_time_end - prev_time_start) +
                   fabs(position_end - position_start) /
                   (time_end - time_start)) / 2;
   setup(position_start, position_end,
         time_start, time_end,
          speed_start, 0,
          acceleration_fraction);
}

void accel_movement::setup_next(double prev_position_start, double prev_position_end,
           double prev_time_start, double prev_time_end,
           double position_start, double position_end,
           double time_start, double time_end,
           double next_position_start, double next_position_end,
           double next_time_start, double next_time_end,
           double acceleration_fraction)
{
  double speed_end = 0;
  double speed_start = 0;
  if ((prev_position_end > prev_position_start) == (position_end > position_start))
    speed_start = (fabs(prev_position_end - prev_position_start) /
                 (prev_time_end - prev_time_start) +
                 fabs(position_end - position_start) /
                 (time_end - time_start)) / 2;
  if ((next_position_end > next_position_start) == (position_end > position_start))
    speed_end = (fabs(next_position_end - next_position_start) /
                 (next_time_end - next_time_start) +
                 fabs(position_end - position_start) /
                 (time_end - time_start)) / 2;
  setup(position_start, position_end,
       time_start, time_end,
        speed_start, speed_end,
        acceleration_fraction);
}

void accel_movement::setup(double position_start, double position_end,
           double time_start, double time_end,
           double speed_start, double speed_end,
           double acceleration_fraction)
{
  p_start = position_start;
  p_end = position_end;
  s_start = speed_start;
  s_end = speed_end;
  if (p_end < p_start) direction = -1;
  else direction = 1;
  debug_prn("direction", direction);
  t_start = time_start;
  t_end = time_end;
  total_movement_time = t_end - t_start;
  debug_prn("total_movement_time", total_movement_time);
  accel_f = acceleration_fraction;
  debug_prn("accel_f", accel_f);

  a1 = fabs(p_end - p_start) / (total_movement_time * total_movement_time * accel_f * (1 - accel_f)) +
        speed_start * (accel_f - 2) / (2 * total_movement_time * accel_f * (1 - accel_f)) -
        speed_end / (2 * total_movement_time * (1 - accel_f));

  a2 = a1 - (speed_end - speed_start) / (accel_f * total_movement_time);

  debug_prn("a1", a1);
  debug_prn("a2", a2);

  double t_accel = accel_f * total_movement_time;
  debug_prn("t_accel", t_accel);
  p_end_accel =  p_start + direction * (0.5 * a1 * t_accel * t_accel + speed_start * t_accel);
  debug_prn("p_end_accel", p_end_accel);

  constant_phase_speed = speed_start + a1 * accel_f * total_movement_time;
  debug_prn("constant_phase_speed", constant_phase_speed);
  if (constant_phase_speed > max_speed)
    setup_with_max_speed();
}

//todo
void accel_movement::setup_with_max_speed()
{
  // same formulas as in usual setup,
  // but instead of known % of time for acceleration/deceleration
  // it is unknown, but known is the constant_speed = max_speed

  // find a1, a2, accel_f
}

void accel_movement::update_is_active(double tm)
{
  if (tm < t_start) active = 0;
  else if (tm > t_end) active = 0;
  else active = 1;
}

double accel_movement::current_position()
{
  double tm = millis();
  update_is_active(tm);
  if (!active) return -1;

  if ((tm - t_start) < accel_f * total_movement_time)
    return p_time_acceleration_phase(tm);
  else if ((tm - t_start) > (1 - accel_f) * total_movement_time)
    return p_time_decelaration_phase(tm);
  else
    return p_time_constant_phase(tm);
}

double accel_movement::p_time_acceleration_phase(double tm)
{
  double t = tm - t_start;
  debug_prn("d", direction * 0.5 * a1 * t * t);
  return p_start + direction * (0.5 * a1 * t * t + s_start * t);
}

double accel_movement::p_time_decelaration_phase(double tm)
{
  double t_remains = t_end - tm;
  debug_prn("t_remains", t_remains);
  debug_prn("d", direction * (0.5 * a2 * t_remains * t_remains + s_end * t_remains));
  return p_end - direction * (0.5 * a2 * t_remains * t_remains + s_end * t_remains);
}

double accel_movement::p_time_constant_phase(double tm)
{
  double t_since_constant_movement = ((tm - t_start)- accel_f * total_movement_time);
  debug_prn("t_since_constant_movement", t_since_constant_movement);
  return p_end_accel + direction * constant_phase_speed * t_since_constant_movement;
}

double accel_movement::time_start()
{
  return t_start;
}

uint8_t accel_movement::is_active()
{
  return active;
}