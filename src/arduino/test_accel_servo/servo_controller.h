#ifndef _SERVO_CONTROLLER_H_
#define _SERVO_CONTROLLER_H_

class servo_controller 
{
  public:

    virtual void set_servo(uint8_t servo_number, double value);
    virtual double current_position(uint8_t servo_number);
    virtual void loop();
};

#endif