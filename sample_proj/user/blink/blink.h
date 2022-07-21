/*
 * blink.h
 *
 *  Created on: Jul 20, 2022
 *      Author: Hashib
 */

#ifndef USER_BLINK_BLINK_H_
#define USER_BLINK_BLINK_H_

//#include "blink/blink.h"


#include "user_interface.h"
#include <stdint.h>
#include <gpio.h>

// see eagle_soc.h for these definitions
#define LED_GPIO 2
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC FUNC_GPIO2

#define DELAY 1000 /* milliseconds */

uint8_t led_state=0;

void blink_init()
{
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
}

LOCAL void ICACHE_FLASH_ATTR blink_cb(void *arg)
{
	GPIO_OUTPUT_SET(LED_GPIO, led_state);
	led_state ^=1;
}



#endif /* USER_BLINK_BLINK_H_ */
