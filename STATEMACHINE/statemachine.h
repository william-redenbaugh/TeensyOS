#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include "OS/error.hpp"

/**
 * @brief Function called when we enter a specific state
*/
typedef void (*statemachine_entry_function_t)(void*);

/**
 * @brief Function called when we exit a specific state
*/
typedef void (*statemachine_exit_function_t)(void*);

/**
 * @brief Function called when we switch between different events
*/
typedef void (*event_function_t)(void*);

/**
 * @brief Structure containing specific event submission metadata
*/
typedef struct event_submission{
    event_function_t event_cb_function;
    void * cb_param_data;
    int event_id;
    int next_state;
    bool active;

}event_submission_t;

/**
 * @brief Stucture containing all state specifc data for handling statemachine
*/
typedef struct statemachine_state
{
    int state;
    statemachine_entry_function_t entry_function;
    statemachine_exit_function_t exit_function;
    void *exit_function_param;
    void *entry_function_param;

    int num_events;
    event_submission_t *events_list;
} statemachine_state_t;


/**
 * @brief Statemachine handler containing all data for entire statemachine
*/
typedef struct statemachine{
    int current_state;
    int latest_event;
    int num_states;
    int num_events;
    statemachine_state_t *states_list;
}statemachine_t;

/**
 * @brief Generates a new statemachine
*/
statemachine_t *init_new_statemachine(const int num_states, const int num_events, const int init_state, const statemachine_state_t *states_list);

/**
 * @brief Submits event to existing statemachine
*/
int statemachine_submit_event(statemachine_t *statemachine, int event);

/**
 * @brief Without using events, we can switch states
 * @note still calls entry and exit functions for states
*/
int statemachine_set_state(statemachine_t *statemachine, int next_state);

/**
 * @brief Attach events to statemachine
*/
int set_statemachine_event_cb(statemachine_t *statemachine, int state, int event, int next_state, event_function_t func, void *params);

/**
 * @brief Clear events in the statemachine
*/
int clear_statemachine_event_cb(statemachine_t *statemachine, int state, int event);
#endif