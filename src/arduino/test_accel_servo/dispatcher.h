#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

class dispatcher
{
  public: 
    virtual void new_packet_arrived(uint8_t packet_type, uint8_t *data, uint32_t len);
    virtual void loop();
};

#endif