#include "Arduino.h"

#include "comm.h"

comm::comm(dispatcher *disp)
{
  comm_state = COMM_STATE_WAIT_HEADER;
  this->packet_dispatcher = disp;
  outgoing_packets = 0;
  bytes_written_of_head_packet = 0;
}

void comm::setup()
{
  Serial.begin(115200);
}

void comm::process_char(uint8_t c)
{
  switch (comm_state)
  {
  case COMM_STATE_WAIT_HEADER:
    if (c == COMM_HEADER_CHAR)
      comm_state = COMM_STATE_WAIT_TYPE;
    break;
  case COMM_STATE_WAIT_TYPE:
    packet_type = c;
    comm_state = COMM_STATE_WAIT_LEN1;
    break;
  case COMM_STATE_WAIT_LEN1:
    len = c;
    comm_state = COMM_STATE_WAIT_LEN2;
    break;
  case COMM_STATE_WAIT_LEN2:
    len += ((uint16_t)c) << 8;
    comm_state = COMM_STATE_WAIT_LEN3;
    break;
  case COMM_STATE_WAIT_LEN3:
    len += ((uint32_t)c) << 16;
    comm_state = COMM_STATE_WAIT_DATA;
    packet = (uint8_t *) malloc(len);
    bytes_read = 0;
    break;
  case COMM_STATE_WAIT_DATA:
    packet[bytes_read++] = c;
    if (bytes_read == len) comm_state = COMM_STATE_WAIT_CRC;
    break;
  case COMM_STATE_WAIT_CRC:
    uint8_t crc = getCRC(&packet_type, 1);
    crc = getCRC((uint8_t *)&len, 1, crc);
    crc = getCRC(1 + ((uint8_t *)&len), 1, crc);
    crc = getCRC(2 + ((uint8_t *)&len), 1, crc);
    crc = getCRC(packet, len, crc);
    if (crc == c)
    {
      unescape_packet(packet, &len);
      packet_dispatcher-> new_packet_arrived(packet_type, packet, len);
    }
    free(packet);
    comm_state = COMM_STATE_WAIT_HEADER;
    break;
  }
}

void comm::unescape_packet(uint8_t *p, uint32_t *len)
{
  uint8_t *q = p;
  uint32_t num_esc_chars = 0;
  for (uint32_t i = 0; i < *len; i++)
  {
    if (*p == 27)  // Escape character detected
    {
      p++;
      if (*p == 28)
      {
        *q = COMM_HEADER_CHAR;  // Replace with original header char
      }
      else *q = *p;  // Replace with original char
      q++;
      p++;
      num_esc_chars++;
    }
    else *(q++) = *(p++);  // Copy normal char
  }
  *len -= num_esc_chars;  // Adjust length to remove escape chars
}

void comm::loop()
{
  while (Serial.available())
  {
    uint8_t c = Serial.read();
    process_char(c);
  }
  int avail = Serial.availableForWrite();
  if (outgoing_packets && avail)
  {
    int32_t remains = outgoing_packets->len - bytes_written_of_head_packet;
    int to_write = min(avail, remains);
    Serial.write(outgoing_packets->data + bytes_written_of_head_packet, to_write);
    bytes_written_of_head_packet += to_write;
    if (bytes_written_of_head_packet == outgoing_packets->len)
    {
      bytes_written_of_head_packet = 0;
      outgoing_packet *to_release = outgoing_packets;
      outgoing_packets = outgoing_packets->next;
      free(to_release);
    }
  }
  packet_dispatcher->loop();
}

void comm::send_packet(uint8_t packet_type, uint32_t len, uint8_t *packet)
{
  int num_esc_chars = 0;
  uint8_t *p = packet;
  for (uint32_t i = 0; i < len; i++)
  {
    if ((*p == COMM_HEADER_CHAR) ||
        (*p == 27)) num_esc_chars++;
    p++;
  }
  
  len += num_esc_chars;

  uint8_t *data = (uint8_t *) malloc(len + num_esc_chars + packet_overhead_size);
  
  p = packet;
  uint8_t *q = data + 5;
  for (uint32_t i = 0; i < len; i++)
    if (*p == COMM_HEADER_CHAR)
    {
      *(q++) = 27;
      *(q++) = 28;
      p++;
    }
    else if (*p == 27)
    {
      *(q++) = 27;
      *(q++) = 27;
      p++;
    }
    else *(q++) = *(p++);
    
  len += num_esc_chars;

  data[0] = COMM_HEADER_CHAR;
  data[1] = packet_type;
  data[2] = len & 255;
  data[3] = (len >> 8) & 255;
  data[4] = (len >> 16) & 255;
  data[len - 1] = getCRC(data + 1, len + 4);

  enqueue_packet(data, len);
}

void comm::enqueue_packet(uint8_t *data, uint32_t len)
{
  outgoing_packet *op = (outgoing_packet *) malloc(sizeof(outgoing_packet));
  op->data = data;
  op->len = len;
  op->next = 0;

  outgoing_packet *q = outgoing_packets;
  if (q == 0) outgoing_packets = op;
  else
  {
    while (q->next) q = q->next;
    q->next = op;
  }
}

uint8_t comm::getCRC(uint8_t message[], uint8_t length, uint8_t previous_crc)
{
  static const unsigned char CRC7_POLY = 0x91;
  uint8_t i, j, crc = previous_crc;
 
  for (i = 0; i < length; i++)
  {
    crc ^= message[i];
    for (j = 0; j < 8; j++)
    {
      if (crc & 1)
        crc ^= CRC7_POLY;
      crc >>= 1;
    }
  }
  return crc;
}