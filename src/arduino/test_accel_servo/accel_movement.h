#ifndef _ACCEL_MOVEMENT_H_
#define _ACCEL_MOVEMENT_H_

#include <math.h>

class accel_movement {

private:

  // configured parameters
  double p_start, p_end;
  double t_start, t_end;
  double s_start, s_end;
  double accel_f;

  // calcuated values
  double total_movement_time;
  int direction;
  double a1, a2;
  double s_end_accel;
  double constant_phase_speed;

  // status
  int active;

  double p_time_acceleration_phase(double tm);
  double p_time_decelaration_phase(double tm);
  double p_time_constant_phase(double tm);

  void update_is_active(double tm);

public:

  static const double default_acceleration_fraction;   // 10% of total movement time is for acceleration and 10% for deceleration

  accel_movement();

  void setup(double position_start, double position_end, 
             double time_start, double time_end, 
             double speed_start = 0, double speed_end = 0,
             double acceleration_fraction = default_acceleration_fraction);

  void setup_first(double position_start, double position_end, 
             double time_start, double time_end,
             double next_position_start, double next_position_end, 
             double next_time_start, double next_time_end,
             double acceleration_fraction = default_acceleration_fraction);

  void setup_last(double prev_position_start, double prev_position_end, 
             double prev_time_start, double prev_time_end,
             double position_start, double position_end, 
             double time_start, double time_end,
             double acceleration_fraction = default_acceleration_fraction);

  void setup_next(double prev_position_start, double prev_position_end, 
             double prev_time_start, double prev_time_end,
             double position_start, double position_end, 
             double time_start, double time_end,
             double next_position_start, double next_position_end, 
             double next_time_start, double next_time_end,
             double acceleration_fraction = default_acceleration_fraction);

  double time_start();

  double current_position();

  int is_active();

};

#endif