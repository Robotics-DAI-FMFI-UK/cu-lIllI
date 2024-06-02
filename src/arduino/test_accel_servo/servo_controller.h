#ifndef _SERVO_CONTROLLER_H_
#define _SERVO_CONTROLLER_H_

class servo_controller 
{
  public:

    virtual void set_servo(int servo_number, double value);
    virtual double current_position(int servo_number);
    virtual void loop();
};

#endif