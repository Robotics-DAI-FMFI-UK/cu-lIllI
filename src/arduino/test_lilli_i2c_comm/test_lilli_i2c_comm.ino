#include "i2c_servo_controller.h"
#include "lilli_comm_dispatcher.h"
#include "comm.h"

int num_controllers = 2;
int addrs[] = { 0x40, 0x41};
int num_servos = 32;
int init_pos[] = {320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320 };

i2c_servo_controller *i2csc;
lilli_comm_dispatcher *disp;
comm *communication;

void setup()
{
 Serial.begin(115200);
 Serial.println("i2c_servo_controller");
 
 i2csc = new i2c_servo_controller(num_servos / num_controllers, num_controllers, addrs, init_pos);
 Serial.println("lilli_comm_dispatcher");
 
 disp = new lilli_comm_dispatcher(num_servos, i2csc);
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
