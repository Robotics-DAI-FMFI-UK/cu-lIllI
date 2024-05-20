#ifndef _ACCEL_MOVEMENT_H_
#define _ACCEL_MOVEMENT_H_

#include <math.h>

class accel_movement {

private:

  // configured parameters
  double p_start, p_end;
  double t_start, t_end;
  double accel_f;

  // calcuated values
  double total_movement_time;
  int direction;
  double a;
  double s_end_accel;
  double constant_phase_speed;

  // status
  int active;

  double p_time_acceleration_phase(double tm);
  double p_time_decelaration_phase(double tm);
  double p_time_constant_phase(double tm);

public:

  static const double default_acceleration_fraction;   // 10% of total movement time is for acceleration and 10% for deceleration

  accel_movement();

  void setup(double position_start, double position_end, 
             double time_start, double time_end, 
             double acceleration_fraction = default_acceleration_fraction);

  double current_position();

};

#endif