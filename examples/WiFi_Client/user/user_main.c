/*
	WiFi Client
 */

#include <ets_sys.h>
#include <osapi.h>
#include "user_interface.h"
#include <os_type.h>
#include "driver/uart.h"
#include "driver/wifi_sta.h"
#include "blink/blink.h"
#include "debug/debug.h"


#define DELAY 1000 /* milliseconds */

extern tConnState connState;

LOCAL os_timer_t OS_TIMER;


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
	enum flash_size_map size_map = system_get_flash_size_map();
	uint32 rf_cal_sec = 0;

	switch (size_map) {
	case FLASH_SIZE_4M_MAP_256_256:
		rf_cal_sec = 128 - 8;
		break;

	case FLASH_SIZE_8M_MAP_512_512:
		rf_cal_sec = 256 - 5;
		break;

	case FLASH_SIZE_16M_MAP_512_512:
	case FLASH_SIZE_16M_MAP_1024_1024:
		rf_cal_sec = 512 - 5;
		break;

	case FLASH_SIZE_32M_MAP_512_512:
	case FLASH_SIZE_32M_MAP_1024_1024:
		rf_cal_sec = 1024 - 5;
		break;

	default:
		rf_cal_sec = 0;
		break;
	}

	return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{


}

void ICACHE_FLASH_ATTR user_init(void)
{
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	uint8_t wifi_retry = 0;

	wifi_connect();

	blink_init();

	// Set up a timer to send the message
	// os_timer_disarm(ETSTimer *ptimer)
	os_timer_disarm(&OS_TIMER);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	os_timer_setfn(&OS_TIMER, (os_timer_func_t *)blink_cb, (void *)0);
	//void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	DEBUG_UART_PRINT("Program init");

	//wifi();

	os_timer_arm(&OS_TIMER, DELAY, 1);

}

