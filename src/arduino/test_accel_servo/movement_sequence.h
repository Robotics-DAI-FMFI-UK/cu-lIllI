#ifndef _MOVEMENT_SEQUENCE_H_
#define _MOVEMENT_SEQUENCE_H_

#include <inttypes.h>

#include "servo_controller.h"
#include "accel_movement.h"

#define epsilon_next 0.00001

typedef struct {
  int servo;
  double rel_time_start;
  double rel_time_end;
  double position_start;
  double position_end;
  int next_with_the_same_servo;  // -1 means it is the last
} sequence_step;

class movement_sequence 
{

private:

  int n_servos;
  servo_controller *s_controller;

  int seq_length;

  double time_start;

  double time_paused;  // 0 means not paused

  int seq_max_length;
  sequence_step *sequence;
  int seq_ind;

  accel_movement *movement[2];
  int *cur_movement;

  double rel_latest_time_of_movement;

  void link_previous_with_the_same_servo();
  void update_rel_end_time();
  void double_sequence_array();

  void start_next_step();
  void start_first_step();

public:

  movement_sequence(int nservos, servo_controller *controller);
  ~movement_sequence();
  
  int load(uint8_t *data);

  void append(int servo, double time_start, double time_end, double position_start, double position_end);
  void append(sequence_step *step);

  void start(double ms);
  double start_time();

  void pause();
  void resume();

  double total_duration();

  void loop();
};

#endif