#include "OSSignalKernel.h"

#ifdef SIGNALING_MODULE

/*!
*   @brief Signals a bit
*   @param thread_signal_t which signal we are setting
*/
void OSSignal::signal(thread_signal_t thread_signal){
    this->bits |= (1 << (uint32_t)thread_signal);
}

/*!
*   @brief clears a bit
*   @param thread_signal_t which signal we are clearing
*/
void OSSignal::clear(thread_signal_t thread_signal){
    this->bits &= ~(1 << (uint32_t)thread_signal);
}

/*!
*   @brief Checks to see if a bit is cleared or set
*   @param thread_signal_t which bit we want to check
*   @returns The status of the signal we are checking 
*/
bool OSSignal::check(thread_signal_t thread_signal){
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return true; 
    return false;    
}



/*!
*   @brief Checks to see if a bit is cleared or set
*   @param thread_signal_t which bit we want to check
*   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
*   @returns whether or or not we we able to get set bits or not
*/
bool OSSignal::wait(thread_signal_t thread_signal, uint32_t timeout_ms){
    // Checking case immediatly. 
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
        return true; 

    // Starting system tick
    uint32_t start = millis();

    while(1){
        if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
            return true; 

        if(timeout_ms && (millis() - start > timeout_ms))
            return false;

        _os_yield();   
    }
}

/*!
*   @brief Checks to see if a bit is cleared or set. If bit was set, then we cleared it. 
*   @param thread_signal_t which bit we want to check
*   @param uint32_t timeout_ms timeout or max time we are willing to wait for bits to clear
*   @returns whether or or not we we able to get set bits or not
*/
bool OSSignal::wait_n_clear(thread_signal_t thread_signal, uint32_t timeout_ms){
    
    // Checking case immediatly. 
    if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal)){
        this->clear(thread_signal);
        return true; 
    }

    // Starting system tick
    uint32_t start = millis();

    while(1){
        if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal)){
            this->clear(thread_signal);
            return true;  
        }
            
        if(timeout_ms && (millis() - start > timeout_ms))
            return false;

        _os_yield();   
    }
}

/*!
*   @brief Waits for bits to be set indefinitly
*   @param thread_signal_t thread_signal to be sete
*/
void OSSignal::wait_notimeout(thread_signal_t thread_signal){
    while(1){
        if(OS_CHECK_BIT(this->bits, (uint32_t)thread_signal))
            return; 

        _os_yield();   
    }   
}

/*!
*   @returns The raw bits that we are using
*/
uint32_t OSSignal::bits_return(void){
    return this->bits; 
}
#endif