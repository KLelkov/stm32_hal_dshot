/*
 * dshot.h
 *
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 *      Konstantin Lelkov modified this file according to his unclear goals and needs.
 */


#ifndef __DSHOT_H__
#define __DSHOT_H__


#include "tim.h"    	// header from stm32cubemx code generate
#include <stdbool.h>	
#include <math.h>


/* User Configuration */
// Timer Clock
#define TIMER_CLOCK				84000000	// Hz

// MOTOR 1 (PA3) - TIM2 Channel 1, DMA1 Stream 5
#define MOTOR_1_TIM             (&htim2)
#define MOTOR_1_TIM_CHANNEL     TIM_CHANNEL_1


/* Definition */
#define MHZ_TO_HZ(x) 			((x) * 1000000)

#define DSHOT1200_HZ     		MHZ_TO_HZ(24)
#define DSHOT600_HZ     		MHZ_TO_HZ(12)
#define DSHOT300_HZ     		MHZ_TO_HZ(6)
#define DSHOT150_HZ     		MHZ_TO_HZ(3)

#define MOTOR_BIT_0            	7
#define MOTOR_BIT_1            	14
#define MOTOR_BITLENGTH        	20

#define DSHOT_FRAME_SIZE       	16
#define DSHOT_DMA_BUFFER_SIZE   18 /* resolution + frame reset (2us) */

#define DSHOT_MIN_THROTTLE      48
#define DSHOT_MAX_THROTTLE     	2047
#define DSHOT_RANGE 			(DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

#define THRESHOLD_HIGH  5.0 * 84
#define THRESHOLD_LOW 2.5 * 84


/* Enumeration */
typedef enum
{
    DSHOT150,
    DSHOT300,
    DSHOT600,
	DSHOT1200
} dshot_type_e;


/* Functions */
void dshot_init(dshot_type_e dshot_type);
void dshot_write(uint16_t motor_value);


#endif /* __DSHOT_H__ */
