//
// Created by niels on 30-1-17.
//

#include "semaphore.h"
#include "stm32f4xx_hal.h"

bool get_lock(volatile uint32_t* lock)
{
  bool result = true;

  if(0 == __LDREXW(lock)){
    if(0 != __STREXW(1, lock)){
      result = false;
    }
  } else {
    result = false;
  }
  __DMB();
  return result;
}

/**
 * Blocking function to get lock
 *
 * @param lock
 */
void wait_for_lock(volatile uint32_t* lock){
  uint8_t status = 0;
  do {
    while (__LDREXW(lock) != 0); // Wait until lock is free
      status = __STREXW(1, lock); // Try to set lock
  } while (status!=0); // Retry until lock successfully
  __DMB();		// Do not start any other memory access until memory barrier is completed
  return;
}


void free_lock(volatile uint32_t* lock)
{
  __DMB(); // Ensure memory operations completed before releasing the lock
  *lock = 0;
  return;
}