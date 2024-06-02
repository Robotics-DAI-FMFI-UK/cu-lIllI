#ifndef _LILLI_COMM_DISPATCHER_H_
#define _LILLI_COMM_DISPATCHER_H_

#include "dispatcher.h"
#include "servo_controller.h"
#include "movement_sequence.h"
#include "comm.h"

#define PP_DEBUG 0
#define PP_INFO  1
#define PP_ERROR 2
#define PP_WARN  3

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
    comm *communication;

  public: 

    lilli_comm_dispatcher(int n_servos, servo_controller *sc);
    ~lilli_comm_dispatcher();

    // PC->Teensy lilli message types  !do not use header(=1), 27, or any other escaped char!
    static const int IMMEDIATE_COMMAND = 2;
    static const int LOAD_SEQUENCE = 3;
    static const int START_SEQUENCE = 4;
    static const int STOP_SEQUENCE = 5;

    // Teensy->PC lilli message types  // do not use escaped chars!
    static const int PRINT_DEBUG = 2;
    static const int PRINT_INFO = 3;
    static const int PRINT_WARN = 4;
    static const int PRINT_ERROR = 5;

    virtual void set_comm_reference(comm *comm_reference);
    virtual void new_packet_arrived(uint8_t packet_type, uint8_t *data, uint32_t len);
    virtual void loop();

    void send_print_packet(uint8_t pp_type, const char *debug_message);
    void send_print_packet(uint8_t pp_type, const char *debug_message, int value);
};

#endif