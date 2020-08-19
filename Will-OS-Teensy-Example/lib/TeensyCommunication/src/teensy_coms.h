#ifndef _TEENSY_COMS_H
#define _TEENSY_COMS_H

// Include Arduino's primary libraries. 
#include <Arduino.h> 

// Include our RTOS stuff
#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"
#include "OS/OSSignalKernel.h"

// Include our RTOS device drivers. 
#include "HAL/OSSerial.h"

// Include our protobuffer messages
#include "messagedata.pb.h"
#include "relay_msg.pb.h"
#include "temp_hum.pb.h"
#include "clock_program.pb.h"
// Helper library that speeds up development
// Involving deserialization
#include "proto_msg_unpack.h"

// Used so we know whether or not our callback initialization was accepted. 
enum MessageSubroutineSetupStatus{
    SUBROUTINE_ADD_SUCCESS, 
    SUBROUTINE_ADD_FAIL_MAX_NUM_REACHED, 
    SUBROUTINE_ADD_FAIL_UNKNOWN, 
    SUBROUTINE_REMOVE_SUCCESS, 
    SUBROUTINE_REMOVE_OUT_OF_BOUMDS,
    SUBROUTINE_REMOVE_FAIL_UNKNOWN
};

// Struct that contains information helping us 
// Deal with message subroutine stuff. 
struct MessageSubroutineSetupReturn{
    MessageSubroutineSetupStatus setup_status; 
    uint32_t callback_handler_id; 
};

// Whenever we get a message. we process this. 
// Note only use the data found in here(including the data pointers)
// When the callback is in scope. Otherwise you might be reading the wrong data. 
struct MessageReq{
    // Pointer to our message information
    uint8_t *data_ptr;
    // Pointer to our message length
    int data_len; 
    // If the caller requested that we send something back
    bool return_message; 
};

/* 
* @brief Easy method for dealing with new messages coming into the system 
* @notes Just makes callbacks easy to deal with, you still need to deal with the deserialization
* And unpacking yourself. 
* params MessageData_MessageType which type of message data are we sending?
* params void(*func)(MessageReq *ptr) callback for dealing with subroutines. 
* returns MessageSubroutineSetupStatus whether or not we actually go the information we needed and the handler ID
*/
extern MessageSubroutineSetupReturn add_message_callback(MessageData_MessageType msg_type, void(*func)(MessageReq *ptr));

/*
*   @brief Removes a message callback, so we aren't getting callbacks from that anymore 
*   @params uint32_t callback_handler_id(which thread are we trying to remove)
*   @returns Whether or not we were able to remove the callback and why. 
*/
extern MessageSubroutineSetupStatus remove_message_callback(uint32_t callback_handler_id); 

/*
*   @brief  Starts up all of the message management stuff so we can get messages!
*   @notes  Just call this, and then attach whatever event driven messaging stuff you feel you need to do 
*/
void message_management_begin(void);

/*
*   @brief Kills the message management thread. 
*/
void message_management_end(void);

#endif 