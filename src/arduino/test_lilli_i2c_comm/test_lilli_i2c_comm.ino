#include "i2c_servo_controller.h"
#include "lilli_comm_dispatcher.h"
#include "comm.h"

uint8_t servos_per_controller = 16;
uint8_t num_controllers = 2;
uint8_t addrs[] = { 0x41, 0x40};
uint8_t num_servos = 25;
uint16_t init_pos[] = { 328, 320, 400, 480, 309, 
                        180, 468, 349, 290, 200, 
                        445, 320, 303, 312, 207, 
                        260, 417, 470, 312, 320, 
                        290, 170, 206, 420, 280 };

uint8_t servo_numbers[] = {  0,  1,  2,  3,  4, 
                             9, 10, 11, 12, 13, 
                            14, 15, 16, 17, 18, 
                            19, 20, 21, 22, 23, 
                            24, 25, 26, 28, 29 };

i2c_servo_controller *i2csc;
lilli_comm_dispatcher *disp;
comm *communication;

void setup()
{
 Serial.begin(115200);
 //Serial.println("i2c_servo_controller");
 
 i2csc = new i2c_servo_controller(servos_per_controller, num_controllers, num_servos, addrs, servo_numbers, init_pos);
 //Serial.println("lilli_comm_dispatcher");
 
 disp = new lilli_comm_dispatcher(num_servos, i2csc);
 //Serial.println("comm");
 communication = new comm(disp);

  //Serial.println("comm->setup");
  communication->setup();
  
  //Serial.println("disp->send_pp");
  disp->send_print_packet(PP_INFO, "Lilli Teensy says hello");
}

uint32_t last_msg = 0;

void loop()
{
  if (millis() - last_msg > 10000)
  {
    disp->send_print_packet(PP_INFO, "tick");
    last_msg = millis();
  }
  communication->loop();
}
