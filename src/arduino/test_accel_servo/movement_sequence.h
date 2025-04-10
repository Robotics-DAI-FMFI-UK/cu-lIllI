#ifndef _MOVEMENT_SEQUENCE_H_
#define _MOVEMENT_SEQUENCE_H_

#include <inttypes.h>

#include "servo_controller.h"
#include "accel_movement.h"

#define epsilon_next 0.00001

#define DEFAULT_MAX_SERVO_SPEED 0.5

typedef struct {
  uint8_t servo;
  double rel_time_start;
  double rel_time_end;
  double position_start;
  double position_end;
  int16_t next_with_the_same_servo;  // -1 means it is the last
} sequence_step;

class movement_sequence 
{

  friend class movement_sequence_parser;

 // friend class lilli_comm_dispatcher;

private:

  uint8_t n_servos;
  servo_controller *s_controller;

  int16_t seq_length;

  double time_start;

  double time_paused;  // 0 means not paused

  int16_t seq_max_length;
  sequence_step *sequence;
  int16_t seq_ind;

  accel_movement *movement[2];
  uint8_t *cur_movement;
  double *max_servo_speed;

  double rel_latest_time_of_movement;



  void link_previous_with_the_same_servo();
  void update_rel_end_time();
  void double_sequence_array();

  void start_next_step();
  void start_first_step();

public:

  movement_sequence(uint8_t nservos, servo_controller *controller);
  ~movement_sequence();

  uint8_t load(uint8_t *data);

  /**
   * @brief Appends a new movement to the sequence.
   * @param servo Number of the servo to move.
   * @param time_start Start time of the movement.
   * @param time_end End time of the movement.
   * @param position_start Starting position for the movement.
   * @param position_end Ending position for the movement.
   */
  void append(uint8_t servo, double time_start, double time_end, double position_start, double position_end);
  void append(sequence_step *step);

  /**
   * @brief Starts the execution of the movement sequence.
   * @param ms Time in milliseconds when the sequence should start.
   */
  void start(double ms);

  /**
   * @brief Stops the execution of the movement sequence.
   */
  void stop();

  /**
   * @brief Returns the start time of the sequence.
   * @return double Start time in milliseconds.
   */
  double start_time();

  /**
   * @brief Pauses the execution of the movement sequence.
   */
  void pause();

  /**
   * @brief Resumes the execution of the paused movement sequence.
   */
  void resume();

  /**
   * @brief Returns total duration of the movement sequence.
   * @return double Total duration in milliseconds.
   */
  double total_duration();

  /**
   * @brief Main loop to handle movement execution. Should be called repeatedly.
   */
  void loop();
};

#endif