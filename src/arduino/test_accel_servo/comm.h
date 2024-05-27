#ifndef _COMM_H_
#define _COMM_H_

#include "dispatcher.h"

typedef struct op_struct {
   uint32_t len;
   uint8_t *data;
   struct op_struct *next;
} outgoing_packet;

class comm 
{

private:
 
  dispatcher *packet_dispatcher;  

  static const uint8_t COMM_STATE_WAIT_HEADER = 1; 
  static const uint8_t COMM_STATE_WAIT_TYPE = 2; 
  static const uint8_t COMM_STATE_WAIT_LEN1 = 3; 
  static const uint8_t COMM_STATE_WAIT_LEN2 = 4; 
  static const uint8_t COMM_STATE_WAIT_LEN3 = 5; 
  static const uint8_t COMM_STATE_WAIT_DATA = 6; 
  static const uint8_t COMM_STATE_WAIT_CRC = 7;

  static const uint8_t COMM_HEADER_CHAR = 1;

  static const uint8_t packet_overhead_size = 6;  //header, type, 3xlen, crc

  uint8_t comm_state;
  uint8_t *packet;
  uint8_t packet_type;
  uint32_t len;
  uint32_t bytes_read;

  outgoing_packet *outgoing_packets;
  uint32_t bytes_written_of_head_packet;

  uint8_t getCRC(uint8_t message[], uint8_t length, uint8_t previous_crc = 0);

  void process_char(uint8_t c);
  void unescape_packet(uint8_t *p, uint32_t *len);
  void enqueue_packet(uint8_t *data, uint32_t len);
   
public:

  comm(dispatcher *packet_dispatcher);

  void setup();
  void loop();

  void send_packet(uint8_t packet_type, uint32_t len, uint8_t *packet);

};

#endif