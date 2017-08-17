/**
 * @file indev.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "indev.h"

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
 * Initialize your input devices here
 */
void indev_init(void)
{

}

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
bool indev_get(uint8_t indev_id, int16_t * x, int16_t * y)
{
	/*Check for invalid ids*/
	if(indev_id >= INDEV_NUM) {
		*x = 0;
		*y = 0;
		return false;
	}


	if(indev_id == 0) {
		/*Read your first input device here (e.g. touchpad)*/
	} else if(indev_id == 1) {
		/*Read your second input device here (e.g. buttons)*/
	}

	return false;	/*In last case return with false*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/