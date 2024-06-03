#ifndef _ACCEL_MOVEMENT_H_
#define _ACCEL_MOVEMENT_H_

#include <math.h>

/**
 * The accel_movement class manages accelerated movements.
 * It provides methods to calculate interpolated position at specific time
 */
class accel_movement {

private:

  double max_speed; 

  // configured parameters
  double p_start;    ///< Start position of the movement
  double p_end;      ///< End position of the movement
  double t_start;    ///< Start time of the movement
  double t_end;      ///< End time of the movement
  double s_start;    ///< Start speed at the beginning of the movement
  double s_end;      ///< End speed at the end of the movement
  double accel_f;    ///< Acceleration factor used for calculating acceleration phases

  // calcuated values
  double total_movement_time; ///< Total time for the movement
  int8_t direction;              ///< Direction of movement (1 for forward, -1 for reverse)
  double a1;                  ///< Acceleration during the acceleration phase
  double a2;                  ///< Acceleration during the deceleration phase
  double p_end_accel;         ///< Position at the end of the acceleration phase
  double constant_phase_speed;///< Speed during the constant speed phase

  // status
  uint8_t active;

  double p_time_acceleration_phase(double tm);      ///< Calculates position during acceleration phase
  double p_time_decelaration_phase(double tm);      ///< Calculates position during deceleration phase
  double p_time_constant_phase(double tm);          ///< Calculates position during constant speed phase

  void update_is_active(double tm);   ///< Updates the activity status based on time
  void setup_with_max_speed();        ///< Setup the movement with maximum speed

public:

  static const double default_acceleration_fraction;   // 10% of total movement time is for acceleration and 10% for deceleration

  /**
   * Constructor for the accel_movement class.
   */
  accel_movement();


  /**
   * @brief Set the maximum speed for the movement.
   * @param max_speed New maximum speed.
   */
  void set_max_speed(double max_speed);

  /**
   * @brief Initialize movement parameters.
   *
   * @param position_start The starting position of the movement.
   * @param position_end The ending position of the movement.
   * @param time_start The starting time of the movement.
   * @param time_end The ending time of the movement.
   * @param speed_start The starting speed of the movement.
   * @param speed_end The ending speed of the movement.
   * @param acceleration_fraction The fraction of the total movement time to be used for acceleration and deceleration.
   */
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

  /**
   * Calculates the position at current time.
   *
   * @return The calculated position.
   */
  double current_position();

  uint8_t is_active();

};

#endif