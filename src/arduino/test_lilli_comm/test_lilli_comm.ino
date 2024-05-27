#include "simple_servo_controller.h"
#include "lilli_comm_dispatcher.h"
#include "comm.h"

int num_servos = 3;
int pins[] = {2, 3, 4};
int init_pos[] = {90, 90, 90};

simple_servo_controller ssc(num_servos, pins, init_pos);
lilli_comm_dispatcher disp(num_servos, &ssc);
comm communication(&disp);

void setup()
{
  communication.setup();
}

void loop()
{
  communication.loop();
}
