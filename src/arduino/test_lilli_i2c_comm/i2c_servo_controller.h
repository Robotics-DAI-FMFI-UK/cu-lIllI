#ifndef _I2C_SERVO_CONTROLLER_H_
#define _I2C_SERVO_CONTROLLER_H_

#include <Adafruit_PWMServoDriver.h>
#include "servo_controller.h"

class i2c_servo_controller : public servo_controller
{
private:
  uint8_t n_servos;
  uint8_t n_controllers;
  uint8_t n_servos_per_controller;
  uint8_t *servos_nums;
  Adafruit_PWMServoDriver **controllers;
  double *positions;

  static const uint8_t UPDATE_FREQUENCY = 15;  // ms

  void write_to_servo(uint8_t servo_number);

public:
  i2c_servo_controller(uint8_t n_servos_per_controller, uint8_t n_controllers, uint8_t servos_used, uint8_t *i2c_addrs, uint8_t *servo_numbers, uint16_t *init_positions);
  ~i2c_servo_controller();
  virtual void set_servo(uint8_t servo_number, double value);
  virtual double current_position(uint8_t servo_number);
  virtual void loop();
};

#endif