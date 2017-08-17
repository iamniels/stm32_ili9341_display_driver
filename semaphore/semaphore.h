//
// Created by niels on 30-1-17.
//

#ifndef GUIDUINO_SEMAPHORE_H
#define GUIDUINO_SEMAPHORE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Non-blocking function to get mutex.
 *
 * @param lock
 * @return true in case lock is obtained, false otherwise
 */
bool get_lock(volatile uint32_t* lock);
void free_lock(volatile uint32_t* lock);
void wait_for_lock(volatile uint32_t* lock);

#endif //GUIDUINO_SEMAPHORE_H
