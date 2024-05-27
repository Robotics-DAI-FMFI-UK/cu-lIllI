#include "Arduino.h"
#include "simple_servo_controller.h"

  simple_servo_controller::simple_servo_controller(int n_servos, int *servo_pins, int *init_positions)
  {
      this->n_servos = n_servos;
      servos = new Servo[n_servos];
      positions = new double[n_servos];
      for (int i = 0; i < n_servos; i++)
      {
        servos[i].attach(servo_pins[i]);
        positions[i] = init_positions[i];
        servos[i].write((int)(0.5 + init_positions[i]));
      }  
  }

   simple_servo_controller::~simple_servo_controller()
   {
     for (int i = 0; i < n_servos; i++)
       servos[i].detach();
     delete [] servos;
     delete [] positions;
   }
  
  void simple_servo_controller::set_servo(int servo_number, double value)
  {    
    positions[servo_number] = value;
  }

  void simple_servo_controller::loop()
  {
    static uint32_t last_servos_update = 0;
    uint32_t ms = millis();

    if (ms - last_servos_update > UPDATE_FREQUENCY)
    {
       last_servos_update = UPDATE_FREQUENCY;
       for (int i = 0; i < n_servos; i++)
       {
          servos[i].write((int)(0.5 + positions[i]));
       }
    }
  }