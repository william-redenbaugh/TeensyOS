#include "EventQueue.hpp"

bool EventQueue::init(uint32_t queue_len){
    queue_lock.lockWaitIndefinite();

    this->queue_len = queue_len; 
    this->data_buffer = (event_data_t*)malloc(sizeof(event_data_t) * queue_len);

    if(this->data_buffer == NULL){
        return false;
    }   

    queue_lock.unlock();

    return true;
}

bool EventQueue::push(event_data_t data){
    this->queue_lock.lockWaitIndefinite();
    // No more space for any more elements in the queue.
    if(this->queue_len == this->current_elements){
        this->queue_lock.unlock();
        return false;
    }
    this->current_elements++;
    this->data_buffer[tail] = data; 
    this->tail++; 
    if(this->tail == this->queue_len)
        this->tail = 0; 
    this->queue_lock.unlock();    
    return true;
}

event_data_t EventQueue::pop(void){
    this->queue_lock.lockWaitIndefinite();
    if(this->current_elements == 0){
        this->queue_lock.unlock();
        event_data_t event;
        return event;
    }
    event_data_t data = this->data_buffer[this->head]; 
    this->current_elements--;
    this->head++;
    if(this->head == this->queue_len)
        this->head = 0;

    this->queue_lock.unlock();
    return data;
}
