#ifndef _LILLI_COMM_DISPATCHER_H_
#define _LILLI_COMM_DISPATCHER_H_

#include "dispatcher.h"
#include "servo_controller.h"
#include "movement_sequence.h"

/**
 * @class lilli_comm_dispatcher
 * @brief Extends the dispatcher class to handle communication specific to Lilli.
 *
 * This dispatcher is tailored to handle and route messages that are specific to the operations
 * and protocols used with Lilli.
 */
class lilli_comm_dispatcher : public dispatcher
{
  private: 
    int n_servos;
    servo_controller *sc;
    movement_sequence *ms;

  public: 

    lilli_comm_dispatcher(int n_servos, servo_controller *sc);
    ~lilli_comm_dispatcher();

    // PC->Teensy lilli message types
    static const int IMMEDIATE_COMMAND = 1;
    static const int LOAD_SEQUENCE = 2;
    static const int START_SEQUENCE = 3;

    virtual void new_packet_arrived(uint8_t packet_type, uint8_t *data, uint32_t len);
    virtual void loop();
};

#endif