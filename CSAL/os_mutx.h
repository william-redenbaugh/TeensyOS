#ifndef _OS_MUTX_H
#define _OS_MUTX_H

#include <Arduino.h>
#include "OS/OSMutexKernel.h"

typedef struct os_mut_t{
    MutexLock lock;
}os_mut_t;

/**
 * @brief Initializer for our mutex module
*/
int os_mut_init(os_mut_t *mut);

/**
 * @brief Allows us to acquuire our Mutex
 * @param os_mut_t *pointer to mutex
*/
int os_mut_entry(os_mut_t *mut, uint32_t timeout_ms);

/**
 * @brief Attempts to get into the mutex
*/
int os_mut_try_entry(os_mut_t *mut);

/**
 * @brief Deconstructor for the mutex
*/
int os_mut_deinit(os_mut_t *mut);

/**
 * @brief How many re-entrants into the lock
 * @param os_mut_t *pointer to mutex
*/
int os_mut_count(os_mut_t *mut);

/**
 * @brief Waits indefinitely until the Mutex has a rentrant lock available
 * @param os_mut_t *pointer to mutex
*/
int os_mut_entry_wait_indefinite(os_mut_t *mut);

/**
 * @brief When we want to unlock our mutex
 * @param os_mut_t *pointer to mutex
*/
int os_mut_exit(os_mut_t *mut);

#endif