#include "Arduino.h"

#include "i2c_servo_controller.h"

  i2c_servo_controller::i2c_servo_controller(uint8_t n_servos_per_controller, uint8_t n_controllers, uint8_t servos_used, uint8_t *i2c_addrs, uint8_t *servo_numbers, uint16_t *init_positions)
  {
    n_servos = servos_used;
    this->n_controllers = n_controllers;
    this->n_servos_per_controller = n_servos_per_controller;
    servos_nums = servo_numbers;
    controllers = (Adafruit_PWMServoDriver **) malloc(sizeof(void *) * n_controllers);
    for (uint8_t i = 0; i < n_controllers; i++)
    {
      controllers[i] = new Adafruit_PWMServoDriver(i2c_addrs[i]);
      controllers[i]->begin();
      controllers[i]->setPWMFreq(60);
    }

    positions = new double[n_servos];
    for (uint8_t i = 0; i < n_servos; i++)
    {      
      positions[i] = init_positions[i];
      write_to_servo(i);
      delay(600); 
    }  
  }

  i2c_servo_controller::~i2c_servo_controller()
  {
    for (uint8_t i = 0; i < n_controllers; i++)
      delete controllers[i];
    free(controllers);
    delete [] positions;
  }

  void i2c_servo_controller::write_to_servo(uint8_t servo_number)
  {
    uint8_t s = servos_nums[servo_number];
    controllers[s / n_servos_per_controller]->setPWM(s % n_servos_per_controller, 0, (int)(0.5 + positions[servo_number]));
  }
  
  void i2c_servo_controller::set_servo(uint8_t servo_number, double value)
  {    
    positions[servo_number] = value;
  }

  double i2c_servo_controller:: current_position(uint8_t servo_number)
  {
    return positions[servo_number];
  }

  void i2c_servo_controller::loop()
  {
    static uint32_t last_servos_update = 0;
    uint32_t ms = millis();

    if (ms - last_servos_update > UPDATE_FREQUENCY)
    {
       last_servos_update = ms;
       for (uint8_t i = 0; i < n_servos; i++)
          write_to_servo(i);
    }
  }
