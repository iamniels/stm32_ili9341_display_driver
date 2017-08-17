/**
 * @file systick.c
 * Provide access to the system tick with 1 millisecond resolution
 */

/*********************
 *      INCLUDES
 *********************/
#include "systick.h"
#include "stm32f4xx_hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t systick_get(void)
{
	return HAL_GetTick();
}
/**
 * Get the elapsed milliseconds science a previous time stamp
 * @param prev_tick a previous time stamp from 'systick_get'
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t systick_elaps(uint32_t prev_tick)
{
	volatile uint32_t act_time = systick_get();

	/*If there is no overflow in sys_time
	 simple subtract*/
	if(act_time >= prev_tick) {
		prev_tick = act_time - prev_tick;
	} else {
		prev_tick = UINT32_MAX - prev_tick + 1;
		prev_tick += act_time;
	}
	return prev_tick;
}

/**
 * Add a callback function to the systick interrupt
 * @param cb a function pointer
 * @return true: 'cb' added to the systick callbacks, false: 'cb' not added
 */
bool systick_add_cb(void (*cb) (void))
{
	/* Optionally you might handle adding call backs to sys. tick interrupt*/
	return false;
}

/**
 * Remove a callback function from the systick callbacks
 * @param cb a function pointer (added with 'systick_add_cb')
 */
void systick_rem_cb(void (*cb) (void))
{
	/* Optionally you might handle removing call backs to sys. tick interrupt*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
