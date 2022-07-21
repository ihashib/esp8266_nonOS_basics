/*
 * wifi_sta.h
 *
 *  Created on: Jul 21, 2022
 *      Author: Hashib
 */

#ifndef USER_WIFI_WIFI_STA_H_
#define USER_WIFI_WIFI_STA_H_

typedef enum {
	WIFI_CONNECTING,
	WIFI_CONNECTING_ERROR,
	WIFI_CONNECTED,
	TCP_DISCONNECTED,
	TCP_CONNECTING,
	TCP_CONNECTING_ERROR,
	TCP_CONNECTED,
	TCP_SENDING_DATA_ERROR,
	TCP_SENT_DATA
} tConnState;

const char *WiFiMode[] =
{
		"NULL",		// 0x00
		"STATION",	// 0x01
};


#ifdef PLATFORM_DEBUG
// enum espconn state, see file /include/lwip/api/err.c
const char *sEspconnErr[] =
{
		"Ok",                    // ERR_OK          0
		"Out of memory error",   // ERR_MEM        -1
		"Buffer error",          // ERR_BUF        -2
		"Timeout",               // ERR_TIMEOUT    -3
		"Routing problem",       // ERR_RTE        -4
		"Operation in progress", // ERR_INPROGRESS -5
		"Illegal value",         // ERR_VAL        -6
		"Operation would block", // ERR_WOULDBLOCK -7
		"Connection aborted",    // ERR_ABRT       -8
		"Connection reset",      // ERR_RST        -9
		"Connection closed",     // ERR_CLSD       -10
		"Not connected",         // ERR_CONN       -11
		"Illegal argument",      // ERR_ARG        -12
		"Address in use",        // ERR_USE        -13
		"Low-level netif error", // ERR_IF         -14
		"Already connected"      // ERR_ISCONN     -15
};
#endif




#endif /* USER_WIFI_WIFI_STA_H_ */
