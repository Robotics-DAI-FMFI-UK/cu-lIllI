#ifndef _COMM_H_
#define _COMM_H_

class comm;

#include "dispatcher.h"

// Structure representing an outgoing packet
typedef struct op_struct {
  uint32_t len;              // Length of the packet
  uint8_t *data;             // Pointer to the packet data
  struct op_struct *next;    // Pointer to the next packet in the queue
} outgoing_packet;

/**
 * Class `comm` manages communication using a serial protocol.
 */
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
  uint8_t crc;
  uint8_t escaped;

  outgoing_packet *outgoing_packets;
  uint32_t bytes_written_of_head_packet;

  /**
   * Compute the CRC for a given message.
   * @param message The message data.
   * @param length The length of the message data.
   * @param previous_crc The previous CRC value (default is 0).
   * @return The computed CRC value.
   */
  uint8_t getCRC(uint8_t message[], uint8_t length, uint8_t previous_crc = 0);

  void process_char(uint8_t c);

  uint8_t original_value_of_escaped_char(uint8_t escaped_char);
  void escape_one_char(uint8_t *p, uint32_t *index);


  /**
   * Remove escape characters from the received packet.
   * @param p Pointer to the packet data.
   * @param len Pointer to the length of the packet data.
   */
  void unescape_packet(uint8_t *p, uint32_t *len);

  /**
   * Enqueue a packet into the outgoing messages queue.
   * @param data Pointer to the packet data.
   * @param len Length of the packet data.
   */
  void enqueue_packet(uint8_t *data, uint32_t len);
   
public:

  /**
   * Constructor for the `comm` class.
   * @param packet_dispatcher Pointer to the dispatcher class.
   */
  comm(dispatcher *packet_dispatcher);

  /**
   * Initialize the serial communication.
   */
  void setup();

  /**
   * Main loop to process incoming and outgoing messages.
   */
  void loop();

  /**
   * Send a packet.
   * @param packet_type The type of the packet.
   * @param len The length of the packet data.
   * @param packet Pointer to the packet data.
   */
  void send_packet(uint8_t packet_type, uint32_t len, const uint8_t *packet);

};

#endif