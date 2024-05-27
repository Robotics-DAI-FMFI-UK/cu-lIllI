#ifndef _MOVEMENT_SEQUENCE_PARSER_H_
#define _MOVEMENT_SEQUENCE_PARSER_H_

#include <inttypes.h>

#include "movement_sequence.h"

class movement_sequence_parser 
{
  private:

    const char *last_error;

  public:

   static uint16_t get_uint16_t(uint8_t **data);
   static uint32_t get_uint24_t(uint8_t **data);
   static void get_two_uint12_t(uint8_t **data, uint16_t *v1, uint16_t *v2);
   
   int load(movement_sequence *ms, uint8_t *data);
   char *get_last_error();
};

#endif