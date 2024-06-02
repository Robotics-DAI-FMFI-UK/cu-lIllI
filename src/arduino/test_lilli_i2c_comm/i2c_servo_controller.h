#ifndef _I2C_SERVO_CONTROLLER_H_
#define _I2C_SERVO_CONTROLLER_H_

#include <Adafruit_PWMServoDriver.h>
#include "servo_controller.h"

class i2c_servo_controller : public servo_controller
{
private:
  int n_servos;
  int n_controllers;
  int n_servos_per_controller;
  Adafruit_PWMServoDriver **controllers;
  double *positions;

  static const int UPDATE_FREQUENCY = 15;  // ms

  void write_to_servo(int servo_number);

public:
  i2c_servo_controller(int n_servos_per_controller, int n_controllers, int *i2c_addrs, int *init_positions);
  ~i2c_servo_controller();
  virtual void set_servo(int servo_number, double value);
  virtual double current_position(int servo_number);
  virtual void loop();
};

#endif