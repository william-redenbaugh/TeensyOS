#include "teensy_coms.h"

// Defining the stack space that we are putting all of our message management stuff into 
#define MESSAGE_MANAGEMENT_STACK_SIZE 128
static uint32_t message_management_stack[MESSAGE_MANAGEMENT_STACK_SIZE]; 

/*
* @brief: Struct that helps us deal with message subroutine callbackets
* @notes: Switched from linkedlist to predefined array for preformance reasons. 
*/
struct MessageSubroutine{
    // Pass in function as arguement
    void (*func)(MessageReq *ptr);
    // Which type of message we are looking for
    MessageData_MessageType msg_type;
    // Whether or not we want to disable the callback    
    bool en = false;
    // If we "delete" a callback, since we statically allocated all this 
    // Stuff, we just say whether or not the message subroute is used or not. 
    bool used = false; 
};
// No message subroutines 
#define MAX_MESSAGE_CALLBACK_NUM 64

// List of callback pointers's information to execute 
static MessageSubroutine message_subroutine_list[MAX_MESSAGE_CALLBACK_NUM]; 

// Currently Active callbacks
static uint32_t callback_num = 0;

// Lock surrounding the messagings. 
static MutexLock MessageMutex;  

// Function that let's us deal with our running threads. 
static void run_messaging_once(void);

// The input buffer that we read our latest data into. 
static uint8_t in_arr_buffer[4096];

// Thread ID for the message management thread. 
static os_thread_id_t message_management_thread_id; 

/*
*   @brief The thread function that we will do all of our message management stuff in
*/
void message_management_thread(void *parameters){
    uint32_t last_millis; 
    for(;;){
        last_millis = millis(); 

        // We run the required stuff for our program. 
        run_messaging_once(); 

        // How long did our program take to run
        uint32_t time_offset = millis() - last_millis;
        // If we are not running overtime(aka subroutine took longer than 14 milliseconds) 
        if((int)(time_offset - 14) > 0)
            // Then we delay for the remainder. 
            os_thread_delay_ms(14 - time_offset);
    }
}

/*
*   @brief Once we have gotten a message header, then we read the remainder of the message. 
*/
static void get_rest_of_message(uint32_t msg_size){
    // No point in reading if there isn't anything. 
    if(msg_size){
        // While we are waiting for message data to come in
        while(os_usb_serial_bytes_available() < msg_size)
            os_thread_delay_ms(1);
   
        // Read in the serial buffer. 
        os_usb_serial_read(in_arr_buffer,(size_t)msg_size); 
    }
}

/* 
* @brief Checks which of our request->response systems need to be interrupted 
* @notes Should only be called from within the udp_message_management file
* params MessageData message_data
* returns none
*/
static void check_req_res(MessageData message_data){
    uint32_t n = 0;  
    uint32_t x = 0; 
    while(1){
        // Once we have exited out of 
        // The total number of active callbacks 
        // In our array, we exit out of the loop
        if(x >= callback_num)
            break; 

        // If the message type matches, we call the function
        if(message_subroutine_list[n].msg_type == message_data.message_type && message_subroutine_list[n].en == true && message_subroutine_list[n].used == true){
            MessageReq msg_req = {in_arr_buffer,(int)message_data.message_size};
            // Call callback function
            message_subroutine_list[n].func(&msg_req); 
        }

        // If that thread is being used, then we increment 
        // The amount of callbacks we have checked. 
        if(message_subroutine_list[n].used)
            x++; 
        
        n++; 
    }
}

/*
*   @brief Function that get's called roughly every 14ms to check thread stuff. 
*/
static void run_messaging_once(void){
    // Making things "threadsafe"
    MessageMutex.lockWaitIndefinite(); 
    
    // If more than 16 bytes are available, then it's likely the serial buffer has been filled. 
    if(os_usb_serial_bytes_available() >= 16){ 
        // Transfer the data from the serial buffer 
        // Our local buffer. 
        uint8_t arr[16]; 
        os_usb_serial_read(arr, 16);
        
        // Unpack the information
        MessageData message_data = unpack_message_data(arr, 16);
        
        // How big is the remainder of the message. 
        get_rest_of_message(message_data.message_size); 
        check_req_res(message_data);
    }
    // Relinquise the resource lock 
    MessageMutex.unlock(); 
}

/* 
* @brief Easy method for dealing with new messages coming into the system 
* @notes Just makes callbacks easy to deal with, you still need to deal with the deserialization
* And unpacking yourself. 
* params MessageData_MessageType which type of message data are we sending?
* params void(*func)(MessageReq *ptr) callback for dealing with subroutines. 
* returns Callbackor not we actually go the information we needed and the handler ID
*/
extern MessageCallbackSetupReturn add_message_callback(MessageData_MessageType msg_type, void(*func)(MessageReq *ptr)){
    // Message we are going to return to the ownership function
    MessageCallbackSetupReturn setup_return; 

    // Lock the resource mutex
    MessageMutex.lockWaitIndefinite();

    // Which spot in our array are we writing our code into 
    uint16_t msg_id = 0;  
    
    // We are running through our array, and the first spot we find that is 
    // False, we choose that as the spot for our callback spot/ID
    for(msg_id = 0; msg_id < MAX_MESSAGE_CALLBACK_NUM; msg_id++){
        if(message_subroutine_list[msg_id].used == false)
            break;     
    }
    
    // If we ran out of spots to put our message subroutine stuff
    if((msg_id+1) == MAX_MESSAGE_CALLBACK_NUM){
        setup_return.setup_status = CALLBACK_ADD_FAIL_MAX_NUM_REACHED;
        return setup_return; 
    }

    // Flags that tell machine that we are using this spot, and that 
    // This callback is enabled 
    message_subroutine_list[msg_id].en = true; 
    // Lets program know that this callback space is being used
    message_subroutine_list[msg_id].used = true; 
    
    // The type of message that we are using
    message_subroutine_list[msg_id].msg_type = msg_type;
    // Pointer call to the function of the program.  
    message_subroutine_list[msg_id].func = func; 

    // Let's owner function know that we were able to setup the subroutine
    // And the id of our message handler. 
    setup_return.callback_handler_id = msg_id; 
    // Let's controller function know that the subroutine was added successfullly. 
    setup_return.setup_status = CALLBACK_ADD_SUCCESS; 

    // Since we have another callback in our program 
    callback_num++; 
    
    // Relinquish the mutex. 
    MessageMutex.unlock();
    return setup_return; 
}

/*
*   @brief Removes a message callback, so we aren't getting callbacks from that anymore 
*   @params uint32_t callback_handler_id(which thread are we trying to remove)
*   @returns Whether or not we were able to remove the callback and why. 
*/
extern MessageCallbackSetupStatus remove_message_callback(uint32_t callback_handler_id){
    if(callback_handler_id >= MAX_MESSAGE_CALLBACK_NUM)
        return CALLBACK_REMOVE_OUT_OF_BOUMDS; 

    // Since we are no longer using this space
    // It will be there to use for the next person to ask for a callback!
    message_subroutine_list[callback_handler_id].used = false; 
    callback_num--; 
    return CALLBACK_REMOVE_SUCCESS; 
}

/*
*   @brief  Starts up all of the message management stuff so we can get messages!
*   @notes  Just call this, and then attach whatever event driven messaging stuff you feel you need to do 
*/
void message_callbacks_begin(void){
    // Starts up the serial interface.
    os_usb_serial_begin(); 
    
    // We have our own thread that deals with the message management stuff. 
    message_management_thread_id = os_add_thread(&message_management_thread, NULL, MESSAGE_MANAGEMENT_STACK_SIZE, &message_management_stack);
}

/*
*   @brief Kills the message management thread. 
*/
void message_callbacks_end(void){
    // We kill our thread if prompted to end our message management callbacks
    os_kill_thread(message_management_thread_id); 
}