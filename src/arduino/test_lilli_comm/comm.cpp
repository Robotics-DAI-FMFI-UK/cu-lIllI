#include "Arduino.h"

#include "comm.h"

//this one goes away...

#include "lilli_comm_dispatcher.h"

comm::comm(dispatcher *disp)
{
  comm_state = COMM_STATE_WAIT_HEADER;
  this->packet_dispatcher = disp;
  outgoing_packets = 0;
  bytes_written_of_head_packet = 0;
  disp->set_comm_reference(this);
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
    {
      comm_state = COMM_STATE_WAIT_TYPE;
      //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "header ok");
    }
    break;
  case COMM_STATE_WAIT_TYPE:
    //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "type ", c);
    packet_type = c;
    comm_state = COMM_STATE_WAIT_LEN1;
    escaped = 0;
    crc = getCRC(&packet_type, 1);    
    break;
  case COMM_STATE_WAIT_LEN1:
    crc = getCRC(&c, 1, crc);
    if (!escaped && (c == 27)) escaped = 1;
    else
    {
      if (escaped) c = original_value_of_escaped_char(c);
      len = c;
      escaped = 0;
      comm_state = COMM_STATE_WAIT_LEN2;
    }    
    break;
  case COMM_STATE_WAIT_LEN2:
    crc = getCRC(&c, 1, crc);
    if (!escaped && (c == 27)) escaped = 1;
    else
    {
      if (escaped) if (escaped) c = original_value_of_escaped_char(c);
      len += ((uint16_t)c) << 8;     
      escaped = 0;
      comm_state = COMM_STATE_WAIT_LEN3;
      //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "len2 ", len);
    }      
    break;
  case COMM_STATE_WAIT_LEN3:
    crc = getCRC(&c, 1, crc);
    if (!escaped && c == 27) escaped = 1;
    else 
    {
      if (escaped) c = original_value_of_escaped_char(c);
      len += ((uint32_t)c) << 16;
      if (len > 0)
      {
        comm_state = COMM_STATE_WAIT_DATA;
        packet = (uint8_t *) malloc(len);
        bytes_read = 0;
        //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "wait data", len);
      }
      else 
      {
        //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "skip to crc");
        comm_state = COMM_STATE_WAIT_CRC;
        bytes_read = 0;
        escaped = 0;
      }
    }
    break;
  case COMM_STATE_WAIT_DATA:
    packet[bytes_read++] = c;
    //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "d", c);
    if (bytes_read == len) 
    {
      comm_state = COMM_STATE_WAIT_CRC;
      escaped = 0;
      //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "wait crc");
    }
    break;
  case COMM_STATE_WAIT_CRC:
    //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "to verify crc");
    if (!escaped && c == 27) escaped = 1;
    else 
    {
      crc = getCRC(packet, len, crc);
      if (escaped) c = original_value_of_escaped_char(c);
      //((lilli_comm_dispatcher *)packet_dispatcher)->send_print_packet(PP_INFO, "pak rcvd");
      if (crc == c)
      {
        unescape_packet(packet, &len);
        packet_dispatcher->new_packet_arrived(packet_type, packet, len);
      }
      else packet_dispatcher->report_receive_error("crc mismatch");
      if (len > 0) free(packet);
      comm_state = COMM_STATE_WAIT_HEADER;
    }
    break;
  }
}

uint8_t comm::original_value_of_escaped_char(uint8_t escaped_char)
{
  if (escaped_char == 28) return  COMM_HEADER_CHAR;  // Replace with original header char
  return escaped_char;  // Replace with original char
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
      *q = original_value_of_escaped_char(*p);  
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

void comm::escape_one_char(uint8_t *p, uint32_t *index)
{
  if (*p == COMM_HEADER_CHAR) 
  {
     *(p++) = 27;
     *p = 28;
     (*index)++;
  }
  else if (*p == 27)
  {
     *(++p) = 27;     
     (*index)++;
  } 
}

void comm::send_packet(uint8_t packet_type, uint32_t len, const uint8_t *packet)
{
  uint32_t num_esc_chars = 0;
  const uint8_t *p = packet;
  for (uint32_t i = 0; i < len; i++)
  {
    if ((*p == COMM_HEADER_CHAR) ||
        (*p == 27)) num_esc_chars++;
    p++;
  }

  uint32_t new_len = len + num_esc_chars;

  uint8_t header[8];
  header[0] = COMM_HEADER_CHAR;
  header[1] = packet_type;
  header[2] = new_len & 255;
  uint32_t index_after_escapes = 3;
  escape_one_char(header + 2, &index_after_escapes);  
  header[index_after_escapes] = (new_len >> 8) & 255;
  escape_one_char(header + index_after_escapes, &index_after_escapes);
  index_after_escapes++;
  header[index_after_escapes] = (new_len >> 16) & 255;
  escape_one_char(header + index_after_escapes, &index_after_escapes);
  index_after_escapes++;
  
  new_len += index_after_escapes + 1;  // + CRC byte

  uint8_t *data = (uint8_t *) malloc(new_len + 1);  // +1 for the case CRC is escaped
  memcpy(data, header, index_after_escapes);
  
  p = packet;
  uint8_t *q = data + index_after_escapes;
  uint32_t escape_increment = 0;
  for (uint32_t i = 0; i < len; i++)
  {
    *(q) = *(p++);
    escape_increment = 1;
    escape_one_char(q, &escape_increment);
    q += escape_increment;
  }
  // CRC is calculated from the whole packet including header, but not the first header char
  data[new_len - 1] = getCRC(data + 1, new_len - 2);
  q = data + (new_len - 1);
  escape_increment = 0;
  escape_one_char(q, &escape_increment);  
  enqueue_packet(data, new_len + escape_increment);
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
/*
  Serial.println("Enqueued:");
  for (uint32_t i = 0; i < len; i++)
  {
    Serial.print(i / 10);
    Serial.print(i % 10);
    Serial.print(" ");
    Serial.print(data[i] / 100);
    Serial.print((data[i] % 100) / 10);
    Serial.print(data[i] % 10);
    Serial.print(" ");
    if (data[i] >= 32) Serial.println((char)data[i]);
    else Serial.println(".");
  }
  */
}

uint8_t comm::getCRC(uint8_t message[], uint32_t length, uint8_t previous_crc)
{
  static const unsigned char CRC7_POLY = 0x91;
  uint32_t i;
  uint8_t j, crc = previous_crc;
 
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