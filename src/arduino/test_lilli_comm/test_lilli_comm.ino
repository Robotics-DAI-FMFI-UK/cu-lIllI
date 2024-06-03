#include "simple_servo_controller.h"
#include "lilli_comm_dispatcher.h"
#include "comm.h"

int num_servos = 3;
int pins[] = {2, 3, 4};
int init_pos[] = {90, 90, 90};

simple_servo_controller *ssc;
lilli_comm_dispatcher *disp;
comm *communication;

void setup()
{
 Serial.begin(115200);
 Serial.println("simple_servo_controller");
 
 ssc = new simple_servo_controller(num_servos, pins, init_pos);
 Serial.println("lilli_comm_dispatcher");
 
 disp = new lilli_comm_dispatcher(num_servos, ssc);
 Serial.println("comm");
 communication = new comm(disp);

  Serial.println("comm->setup");
  communication->setup();
  
  Serial.println("disp->send_pp");
  disp->send_print_packet(PP_INFO, "Lilli Teensy says hello");
}

long last_msg = 0;

void loop()
{
  if (millis() - last_msg > 10000)
  {
    disp->send_print_packet(PP_INFO, "tick");
    last_msg = millis();
  }
  communication->loop();
}
