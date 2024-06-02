#include "Arduino.h"

#include "movement_sequence_parser.h"



uint16_t movement_sequence_parser::get_uint16_t(uint8_t **data)
{
  uint16_t x = (**data);
  (*data)++;
  x += ((uint16_t)(**data)) << 8;
  (*data)++;
  return x;
}

uint32_t movement_sequence_parser::get_uint24_t(uint8_t **data)
{
  uint32_t x = (**data);
  (*data)++;
  x += ((uint16_t)(**data)) << 8;
  (*data)++;
  x += ((uint32_t)(**data)) << 16;
  (*data)++;
  return x;
}

void movement_sequence_parser::get_two_uint12_t(uint8_t **data, uint16_t *v1, uint16_t *v2)
{
  uint8_t b1 = (**data);
  (*data)++;
  uint8_t b2 = (**data);
  (*data)++;
  uint8_t b3 = (**data);
  (*data)++;

  *v1 = (((uint16_t)b1) << 4) | (b2 >> 4);
  *v2 = (((uint16_t)b3) << 4) | (b2 & 15);
}

  
/* n_servos (uint8_t)
   n_steps  (uint16_t)
   servo (uint8_t), time_start (24bit), time_end(24bit), position_start(12bit), position_end(12bit)
*/
int movement_sequence_parser::load(movement_sequence *ms, uint8_t *data, lilli_comm_dispatcher *disp)
{
  disp->send_print_packet(PP_INFO, "n_steps", n_steps);
  if (*data != ms->n_servos)
  {
    last_error = "number servos mismatch";
    return 0;
  }
  data++;
  uint16_t n_steps = get_uint16_t(&data);

  disp->send_print_packet(PP_INFO, "n_steps", n_steps);

  uint32_t last_time_start = 0;

  for (int i = 0; i < n_steps; i++)
  {
    uint8_t servo = *(data++);
    disp->send_print_packet(PP_INFO, "step with servo", servo);
    if ((servo < 0) || (servo >= ms->n_servos))
    {
      last_error = "servo# out of range";
      return 0;
    }
    uint32_t time_start = get_uint24_t(&data);
    if (time_start < last_time_start)
    {
      last_error = "time starts are not ordered";
      return 0;
    }
    last_time_start = time_start;
    uint32_t time_end = get_uint24_t(&data);
    uint16_t position_start, position_end;
    get_two_uint12_t(&data, &position_start, &position_end);
    ms->append(servo, time_start, time_end, position_start, position_end);
  }

  return 1;
}

const char *movement_sequence_parser::get_last_error()
{
  return last_error;
}