#ifndef _SIMPLE_SERVO_CONTROLLER_H_
#define _SIMPLE_SERVO_CONTROLLER_H_

#include <Servo.h>
#include "servo_controller.h"

class simple_servo_controller : public servo_controller
{
private:
  int n_servos;
  Servo *servos;
  double *positions;

public:
  simple_servo_controller(int n_servos, int *servo_pins, int *init_positions);
  ~simple_servo_controller();
  virtual void set_servo(int servo_number, double value);
  void loop();
};

#endif