#include "Arduino.h"

#include "lilli_comm_dispatcher.h"
#include "movement_sequence_parser.h"
#include "movement_sequence.h"

lilli_comm_dispatcher::lilli_comm_dispatcher(int n_servos, servo_controller *sc)
{
  this->n_servos = n_servos;
  this->sc = sc;
  ms = 0;
}

lilli_comm_dispatcher::~lilli_comm_dispatcher()
{
  if (ms) delete ms;
}

void lilli_comm_dispatcher::set_comm_reference(comm *comm_reference)
{
  communication = comm_reference;
}

void lilli_comm_dispatcher::new_packet_arrived(uint8_t packet_type, uint8_t *data, uint32_t len)
{
  uint8_t servo;
  uint16_t target_position;

  switch (packet_type)
  {
    case IMMEDIATE_COMMAND: servo = *(data++);
                            target_position = movement_sequence_parser::get_uint16_t(&data);
                            //send_print_packet(PP_INFO, "immcmd received");
                            if (ms) delete ms;
                            ms = new movement_sequence(n_servos, sc);
                            ms->append(servo, 0, 1000, sc->current_position(servo), target_position);
                            ms->start(millis());                          
                            break;
    case LOAD_SEQUENCE: movement_sequence_parser msp;
                        if (ms) delete ms;
                        ms = new movement_sequence(n_servos, sc);
                        msp.load(ms, data);
                        break;   
    case START_SEQUENCE: //todo extract time
                         uint32_t tm = 0;
                         ms->start(tm);
                         break;                        
  }
}

void lilli_comm_dispatcher::loop()
{
  if (ms) ms->loop();
  sc->loop();
}

void lilli_comm_dispatcher::send_print_packet(uint8_t pp_type, const char *debug_message)
{
  communication->send_packet(pp_type + PRINT_DEBUG, strlen(debug_message), (const uint8_t *)debug_message);
}

void lilli_comm_dispatcher::send_print_packet(uint8_t pp_type, const char *debug_message, int value)
{
  int len = strlen(debug_message);
  char num_buffer[8];
  itoa(value, num_buffer, 10);
  char msg[10 + len];
  memcpy(msg, debug_message, len);
  msg[len++] = ':';
  msg[len++] = ' ';
  int len2 = strlen(num_buffer);
  memcpy(msg + len, num_buffer, len2 + 1);

  communication->send_packet(pp_type + PRINT_DEBUG, len + len2 + 1, (const uint8_t *)msg);
}