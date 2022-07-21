/*
 * wifi_sta.c
 *
 *  Created on: Jul 21, 2022
 *      Author: Hashib
 */
#include "driver/wifi_sta.h"

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

esp_tcp ConnTcp;
extern int ets_uart_printf(const char *fmt, ...);

static char macaddr[6];
static ETSTimer WiFiLinker;

//IMP: Extern this var-------------------------------------------------------------------------##
tConnState connState = WIFI_CONNECTING;

static void platform_reconnect(struct espconn *);
static void wifi_check_ip(void *arg);



static void ICACHE_FLASH_ATTR platform_reconnect(struct espconn *pespconn)
{
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("platform_reconnect\r\n");
	#endif
	wifi_check_ip(NULL);
}

static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;
	os_timer_disarm(&WiFiLinker);
	switch(wifi_station_get_connect_status())
	{
		case STATION_GOT_IP:
			wifi_get_ip_info(STATION_IF, &ipConfig);
			if(ipConfig.ip.addr != 0) {
				connState = WIFI_CONNECTED;
				#ifdef PLATFORM_DEBUG
				ets_uart_printf("WiFi connected\r\n");
				#endif

				return;
			}
			break;
		case STATION_WRONG_PASSWORD:
			connState = WIFI_CONNECTING_ERROR;
			#ifdef PLATFORM_DEBUG
			ets_uart_printf("WiFi connecting error, wrong password\r\n");
			#endif
			break;
		case STATION_NO_AP_FOUND:
			connState = WIFI_CONNECTING_ERROR;
			#ifdef PLATFORM_DEBUG
			ets_uart_printf("WiFi connecting error, ap not found\r\n");
			#endif
			break;
		case STATION_CONNECT_FAIL:
			connState = WIFI_CONNECTING_ERROR;
			#ifdef PLATFORM_DEBUG
			ets_uart_printf("WiFi connecting fail\r\n");
			#endif
			break;
		default:
			connState = WIFI_CONNECTING;
			#ifdef PLATFORM_DEBUG
			ets_uart_printf("WiFi connecting...\r\n");
			#endif
	}
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&WiFiLinker, 1000, 0);
}

void setup_wifi_st_mode(void)
{
	wifi_set_opmode((wifi_get_opmode()|STATIONAP_MODE)&USE_WIFI_MODE);
	struct station_config stconfig;
	wifi_station_disconnect();
	wifi_station_dhcpc_stop();
	if(wifi_station_get_config(&stconfig))
	{
		os_memset(stconfig.ssid, 0, sizeof(stconfig.ssid));
		os_memset(stconfig.password, 0, sizeof(stconfig.password));
		os_sprintf(stconfig.ssid, "%s", WIFI_CLIENTSSID);
		os_sprintf(stconfig.password, "%s", WIFI_CLIENTPASSWORD);
		if(!wifi_station_set_config(&stconfig))
		{
			#ifdef PLATFORM_DEBUG
			ets_uart_printf("ESP8266 not set station config!\r\n");
			#endif
		}
	}
	wifi_station_connect();
	wifi_station_dhcpc_start();
	wifi_station_set_auto_connect(1);
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("ESP8266 in STA mode configured.\r\n");
	#endif
}
void wifi_connect(void)
{
	ets_uart_printf("In wifi connect\n");
	//if(wifi_get_opmode() != USE_WIFI_MODE)

	if(USE_WIFI_MODE & STATION_MODE)
			setup_wifi_st_mode();

	if(USE_WIFI_MODE & STATION_MODE)
		wifi_get_macaddr(STATION_IF, macaddr);

	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);

	if(wifi_station_get_auto_connect() == 0)
		wifi_station_set_auto_connect(1);

	#ifdef PLATFORM_DEBUG

	if(USE_WIFI_MODE & STATION_MODE)
	{
		struct station_config stationConfig;
		if(wifi_station_get_config(&stationConfig)) {
			ets_uart_printf("STA config: SSID: %s, PASSWORD: %s\r\n",
				stationConfig.ssid,
				stationConfig.password);
		}
	}
	#endif

	// Wait for Wi-Fi connection and start TCP connection
	os_timer_disarm(&WiFiLinker);
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&WiFiLinker, 1000, 0);


	#ifdef PLATFORM_DEBUG
	ets_uart_printf("ESP8266 platform started!\r\n");
	#endif
}





