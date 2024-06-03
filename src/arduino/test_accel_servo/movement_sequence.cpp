#include "Arduino.h"

#include "movement_sequence.h"
#include "accel_movement.h"

  movement_sequence::movement_sequence(uint8_t nservos, servo_controller *controller)
  {
    n_servos = nservos;
    s_controller = controller;

    movement[0] = new accel_movement[nservos];
    movement[1] = new accel_movement[nservos];
    
    cur_movement = new uint8_t[nservos];
    for (uint8_t i = 0; i < nservos; i++) cur_movement[i] = 0;

    max_servo_speed = new double[nservos];
    for (uint8_t i = 0; i < nservos; i++)
    {
      max_servo_speed[i] = DEFAULT_MAX_SERVO_SPEED;
      movement[0][i].set_max_speed(max_servo_speed[i]);
      movement[1][i].set_max_speed(max_servo_speed[i]);
    }

    seq_length = 0;
    time_start = 0;
    time_paused = 0;
    rel_latest_time_of_movement = 0;
    seq_max_length = 16;
    sequence = (sequence_step *) malloc(seq_max_length * sizeof(sequence_step));
  }
  
  movement_sequence::~movement_sequence()
  {
    free(sequence);
    delete []  movement[0];
    delete []  movement[1];
    delete [] cur_movement;
    delete [] max_servo_speed;
  }
 
  void movement_sequence::double_sequence_array()
  {
    seq_max_length *= 2;
    sequence = (sequence_step *) realloc(sequence, seq_max_length * sizeof(sequence_step));  
  }

  void movement_sequence::update_rel_end_time()
  {
    if (sequence[seq_length].rel_time_end > rel_latest_time_of_movement)
      rel_latest_time_of_movement = sequence[seq_length].rel_time_end;
  }

  void movement_sequence::append(uint8_t servo, double time_start, double time_end, double position_start, double position_end)
  {
    if ((servo >= n_servos) || (servo < 0)) return;
    if (seq_max_length == seq_length) double_sequence_array();
    sequence[seq_length].servo = servo;
    sequence[seq_length].rel_time_start = time_start;
    sequence[seq_length].rel_time_end = time_end;
    sequence[seq_length].position_start = position_start;
    sequence[seq_length].position_end = position_end;
    sequence[seq_length].next_with_the_same_servo = -1;
    update_rel_end_time();
    link_previous_with_the_same_servo();
    seq_length++;
  }

  void movement_sequence::link_previous_with_the_same_servo()
  {
    int16_t i = seq_length - 1;
    while (i >= 0)
    {
      if (sequence[i].servo == sequence[seq_length].servo)
      {
        if (fabs(sequence[i].rel_time_end - sequence[seq_length].rel_time_start) < epsilon_next)        
          sequence[i].next_with_the_same_servo = seq_length;        
        break;        
      }
      i--;
    }
  }

  void movement_sequence::append(sequence_step *step)
  {
    if ((step->servo >= n_servos) || (step->servo < 0)) return;
    if (seq_max_length == seq_length) double_sequence_array();
    sequence[seq_length] = *step;
    update_rel_end_time();
    link_previous_with_the_same_servo();
    seq_length++;
  }

  void movement_sequence::start(double ms)
  {
    time_start = ms;
    seq_ind = -1;
  }

  void movement_sequence::stop()
  {
    time_start = time_start + rel_latest_time_of_movement + 1;    
  }

  double movement_sequence::start_time()
  {
    return time_start;
  }

  void movement_sequence::pause()
  {
    time_paused = millis();
  }

  void movement_sequence::resume()
  {
    time_start += millis() - time_paused;
    time_paused = 0;
  }

  double movement_sequence::total_duration()
  {
    if (seq_length == 0) return 0;
    return rel_latest_time_of_movement;
  }

  void movement_sequence::loop()
  {
    if (seq_length == 0) return;
    if (time_paused != 0) return;
    double ms = millis();
    if (ms < time_start) return;
    if (ms > time_start + rel_latest_time_of_movement) return;

    while ((seq_ind + 1 < seq_length) && 
           (ms >= time_start + sequence[seq_ind + 1].rel_time_start))
    {      
      //Serial.println("start next step");
      start_next_step();
    }

    for (uint8_t i = 0; i < n_servos; i++)
    {      
      double pos = movement[cur_movement[i]][i].current_position();
      if (pos >= 0) s_controller->set_servo(i, pos);
    }
  }

  void movement_sequence::start_next_step()
  {    
     seq_ind++;
     uint8_t s = sequence[seq_ind].servo;
     uint8_t cur_ind = cur_movement[s];
     cur_movement[s] ^= 1;
     int16_t next_with_same = sequence[seq_ind].next_with_the_same_servo;

     if (!movement[cur_ind][s].is_active())
     {
        start_first_step();
     }
     else if (next_with_same == -1)
     {
        return;
     }
     else
     {
       int16_t next_next_with_same = sequence[next_with_same].next_with_the_same_servo;

        if (next_next_with_same == -1)
        {
          movement[cur_ind][s].setup_last(
            sequence[seq_ind].position_start,
            sequence[seq_ind].position_end,
            time_start + sequence[seq_ind].rel_time_start,
            time_start + sequence[seq_ind].rel_time_end,
            sequence[next_with_same].position_start,
            sequence[next_with_same].position_end,
            time_start + sequence[next_with_same].rel_time_start,
            time_start + sequence[next_with_same].rel_time_end);
        }
        else
        {
          movement[cur_ind][s].setup_next(            
            sequence[seq_ind].position_start,
            sequence[seq_ind].position_end,
            time_start + sequence[seq_ind].rel_time_start,
            time_start + sequence[seq_ind].rel_time_end,
            sequence[next_with_same].position_start,
            sequence[next_with_same].position_end,
            time_start + sequence[next_with_same].rel_time_start,
            time_start + sequence[next_with_same].rel_time_end,
            sequence[next_next_with_same].position_start,
            sequence[next_next_with_same].position_end,
            time_start + sequence[next_next_with_same].rel_time_start,
            time_start + sequence[next_next_with_same].rel_time_end);
        }
     }
  }

  void movement_sequence::start_first_step()
  {
    uint8_t s = sequence[seq_ind].servo;
    uint8_t cur_ind = cur_movement[s];
    int16_t next_with_same = sequence[seq_ind].next_with_the_same_servo;
    int16_t next_next_with_same = -1;
    if (next_with_same != -1) 
      next_next_with_same = sequence[next_with_same].next_with_the_same_servo;

    if (next_with_same == -1)
    {
      movement[cur_ind][s].setup(
                          sequence[seq_ind].position_start,
                          sequence[seq_ind].position_end,
                          time_start + sequence[seq_ind].rel_time_start,
                          time_start + sequence[seq_ind].rel_time_end);
    }
    else if (next_next_with_same == -1)
    {
      movement[cur_ind][s].setup_first(
        sequence[seq_ind].position_start,
        sequence[seq_ind].position_end,
        time_start + sequence[seq_ind].rel_time_start,
        time_start + sequence[seq_ind].rel_time_end,
        sequence[next_with_same].position_start,
        sequence[next_with_same].position_end,
        time_start + sequence[next_with_same].rel_time_start,
        time_start + sequence[next_with_same].rel_time_end);
      movement[1 - cur_ind][s].setup_last(
        sequence[seq_ind].position_start,
        sequence[seq_ind].position_end,
        time_start + sequence[seq_ind].rel_time_start,
        time_start + sequence[seq_ind].rel_time_end,
        sequence[next_with_same].position_start,
        sequence[next_with_same].position_end,
        time_start + sequence[next_with_same].rel_time_start,
        time_start + sequence[next_with_same].rel_time_end);
    }
    else
    {
      movement[cur_ind][s].setup_first(
        sequence[seq_ind].position_start,
        sequence[seq_ind].position_end,
        time_start + sequence[seq_ind].rel_time_start,
        time_start + sequence[seq_ind].rel_time_end,
        sequence[next_with_same].position_start,
        sequence[next_with_same].position_end,
        time_start + sequence[next_with_same].rel_time_start,
        time_start + sequence[next_with_same].rel_time_end);
      movement[1 - cur_ind][s].setup_next(
        sequence[seq_ind].position_start,
        sequence[seq_ind].position_end,
        time_start + sequence[seq_ind].rel_time_start,
        time_start + sequence[seq_ind].rel_time_end,
        sequence[next_with_same].position_start,
        sequence[next_with_same].position_end,
        time_start + sequence[next_with_same].rel_time_start,
        time_start + sequence[next_with_same].rel_time_end,
        sequence[next_next_with_same].position_start,
        sequence[next_next_with_same].position_end,
        time_start + sequence[next_next_with_same].rel_time_start,
        time_start + sequence[next_next_with_same].rel_time_end);
    }
  }
