#include "Arduino.h"

#include "lilli_comm_dispatcher.h"
#include "movement_sequence_parser.h"
#include "movement_sequence.h"

lilli_comm_dispatcher::lilli_comm_dispatcher(uint8_t n_servos, servo_controller *sc)
{
  this->n_servos = n_servos;
  this->sc = sc;
  time_origin = 0;
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
  uint32_t start_time;
  movement_sequence_parser *msp;

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
    case LOAD_SEQUENCE: msp = new movement_sequence_parser; 
                        if (ms) delete ms;
                        //send_print_packet(PP_INFO, "loading sequence");
                        ms = new movement_sequence(n_servos, sc);
                        if (!msp->load(ms, data))                        
                          send_print_packet(PP_ERROR, msp->get_last_error());
                        delete msp;                        
                        break;
    case START_SEQUENCE: start_time = movement_sequence_parser::get_uint24_t(&data);
                         if (start_time == 0) start_time = millis() - time_origin;     
                         //send_print_packet(PP_INFO, "current time", millis());             
                         //send_print_packet(PP_INFO, "starting sequence at", time_origin + start_time); 
                         //send_print_packet(PP_INFO, "seqlen", (uint32_t)ms->seq_length); 
                         //send_print_packet(PP_INFO, "time_start", (uint32_t)ms->time_start); 
                         ms->start(time_origin + start_time);
                        //send_print_packet(PP_INFO, "total dura", (uint32_t)ms->total_duration());
                         break;    
    case RESET_TIME_ORIGIN: time_origin = millis();
                            //send_print_packet(PP_INFO, "time origin was reset");
                            break;         
    case STOP_SEQUENCE: if (ms) ms->stop();
                            break;    
    case PAUSE_SEQUENCE: if (ms) ms->pause();
                            break;
    case RESUME_SEQUENCE: if (ms) ms->resume();
                            break;                                                        
  }
}

void lilli_comm_dispatcher::report_receive_error(const char *err)
{
  communication->send_packet(PRINT_ERROR, strlen(err), (const uint8_t *)err);
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
  memcpy(msg + len, num_buffer, len2);

  communication->send_packet(pp_type + PRINT_DEBUG, len + len2, (const uint8_t *)msg);
}