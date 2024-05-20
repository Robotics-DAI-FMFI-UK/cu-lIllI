#include "Arduino.h"

#include "accel_movement.h"

  const double accel_movement::default_acceleration_fraction = 0.3;   // 10% of total movement time is for acceleration and 10% for deceleration

  accel_movement::accel_movement()
  {
    active = 0;
  }

  void debug_prn(const char *msg, double val)
  {
/*    Serial.print(msg);
    Serial.print(": ");
    Serial.println(val);*/
  }

  void accel_movement::setup(double position_start, double position_end, 
             double time_start, double time_end, 
             double acceleration_fraction)
  {
    p_start = position_start;
    p_end = position_end;
    if (p_end < p_start) direction = -1;
    else direction = 1;
    debug_prn("direction", direction);
    t_start = time_start;
    t_end = time_end;
    total_movement_time = t_end - t_start;
    debug_prn("total_movement_time", total_movement_time);
    accel_f = acceleration_fraction;
    debug_prn("accel_f", accel_f);
    a = fabs(p_end - p_start) / (total_movement_time * total_movement_time * (accel_f * accel_f + accel_f * (1 - 2 * accel_f)));
    debug_prn("a", a);

    double t_accel = accel_f * total_movement_time;
    debug_prn("t_accel", t_accel);
    s_end_accel =  p_start + direction * 0.5 * a * t_accel * t_accel;
    debug_prn("s_end_accel", s_end_accel);
   
    constant_phase_speed = a * accel_f * total_movement_time;
    debug_prn("constant_phase_speed", constant_phase_speed);
    
    active = 1;
  }

  double accel_movement::current_position()
  {
    if (!active) return p_end;
    double tm = millis();
    if (tm < t_start) return p_start;
    if (tm > t_end) 
    {
      active = 0;
      return p_end;
    }

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
    debug_prn("d", direction * 0.5 * a * t * t);
    return p_start + direction * 0.5 * a * t * t;
  }

  double accel_movement::p_time_decelaration_phase(double tm)
  {
    double t_remains = t_end - tm;
    debug_prn("t_remains", t_remains);
    debug_prn("d", direction * 0.5 * a * t_remains * t_remains);
    return p_end - direction * 0.5 * a * t_remains * t_remains;
  }

  double accel_movement::p_time_constant_phase(double tm)
  {
    double t_since_constant_movement = ((tm - t_start)- accel_f * total_movement_time);
    debug_prn("t_since_constant_movement", t_since_constant_movement);
    return s_end_accel + direction * constant_phase_speed * t_since_constant_movement;
  }