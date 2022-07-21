/*
 * wifi_sta.h
 *
 *  Created on: Jul 21, 2022
 *      Author: Hashib
 */

#ifndef USER_WIFI_WIFI_STA_H_
#define USER_WIFI_WIFI_STA_H_

#include <osapi.h>
#include "user_interface.h"
#include "user_config.h"
#include "espconn.h"
#include <string.h>

typedef enum {
	WIFI_CONNECTING,
	WIFI_CONNECTING_ERROR,
	WIFI_CONNECTED,
	WIFI_RETRY_COUNT = 5
} tConnState;

void wifi_connect(void);



#endif /* USER_WIFI_WIFI_STA_H_ */
