/*
 * wifi_sta.c
 *
 *  Created on: Jul 21, 2022
 *      Author: Hashib
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "user_config.h"

#include "wifi_sta.h"

static struct espconn Conn;
esp_tcp ConnTcp;
extern int ets_uart_printf(const char *fmt, ...);
int (*console_printf)(const char *fmt, ...) = ets_uart_printf;
static char macaddr[6];
static ETSTimer WiFiLinker;
static tConnState connState = WIFI_CONNECTING;
static unsigned char tcpReconCount;

static void platform_reconnect(struct espconn *);
static void wifi_check_ip(void *arg);


static void ICACHE_FLASH_ATTR tcpclient_sent_cb(void *arg)
{
	struct espconn *pespconn = arg;
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("tcpclient_sent_cb\r\n");
	#endif
	#ifdef LWIP_DEBUG
	list_espconn_tcp(pespconn);
	list_lwip_tcp_psc();
	#endif
	espconn_disconnect(pespconn);
}

static void ICACHE_FLASH_ATTR tcpclient_discon_cb(void *arg)
{
	struct espconn *pespconn = arg;
	connState = TCP_DISCONNECTED;
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("tcpclient_discon_cb\r\n");
	#endif
	if (pespconn == NULL)
	{
		#ifdef PLATFORM_DEBUG
		ets_uart_printf("tcpclient_discon_cb - conn is NULL!\r\n");
		#endif
		return;
	}
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("Will reconnect in 2s...\r\n");
	#endif
	os_timer_disarm(&WiFiLinker);
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)platform_reconnect, pespconn);
	os_timer_arm(&WiFiLinker, 2000, 0);
}

static void ICACHE_FLASH_ATTR tcpclient_connect_cb(void *arg)
{
	struct espconn *pespconn = arg;
	tcpReconCount = 0;
	char payload[128];
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("tcpclient_connect_cb\r\n");
	#endif
	espconn_regist_sentcb(pespconn, tcpclient_sent_cb);
	connState = TCP_CONNECTED;
	os_sprintf(payload, MACSTR ",%s", MAC2STR(macaddr), "ESP8266");
	sint8 espsent_status = espconn_sent(pespconn, payload, strlen(payload));
	if(espsent_status == ESPCONN_OK) {
		connState = TCP_SENT_DATA;
		#ifdef PLATFORM_DEBUG
		console_printf("Data sent, payload: %s\r\n", payload);
		#endif
	} else {
		connState = TCP_SENDING_DATA_ERROR;
		#ifdef PLATFORM_DEBUG
		console_printf("Error while sending data.\r\n");
		#endif
	}
	#ifdef LWIP_DEBUG
	list_espconn_tcp(pespconn);
	list_lwip_tcp_psc();
	#endif
}

static void ICACHE_FLASH_ATTR tcpclient_recon_cb(void *arg, sint8 err)
{
	struct espconn *pespconn = arg;
	connState = TCP_DISCONNECTED;

	#ifdef PLATFORM_DEBUG
	console_printf("tcpclient_recon_cb\r\n");
	if (err != ESPCONN_OK)
		console_printf("Connection error: %d - %s\r\n", err, ((err>-16)&&(err<1))? sEspconnErr[-err] : "?");
    #endif
	#ifdef LWIP_DEBUG
	list_espconn_tcp(pespconn);
	list_lwip_tcp_psc();
	#endif

    if (++tcpReconCount >= 5)
    {
		connState = TCP_CONNECTING_ERROR;
		tcpReconCount = 0;
		#ifdef PLATFORM_DEBUG
		console_printf("tcpclient_recon_cb, 5 failed TCP attempts!\r\n");
		console_printf("Will reconnect in 10s...\r\n");
		#endif
		os_timer_disarm(&WiFiLinker);
		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)platform_reconnect, pespconn);
		os_timer_arm(&WiFiLinker, 10000, 0);
    }
    else
    {
		#ifdef PLATFORM_DEBUG
		console_printf("Will reconnect in 2s...\r\n");
		#endif
		os_timer_disarm(&WiFiLinker);
		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)platform_reconnect, pespconn);
		os_timer_arm(&WiFiLinker, 2000, 0);
	}
}

static void ICACHE_FLASH_ATTR platform_reconnect(struct espconn *pespconn)
{
	#ifdef PLATFORM_DEBUG
	console_printf("platform_reconnect\r\n");
	#endif
	wifi_check_ip(NULL);
}

static void ICACHE_FLASH_ATTR senddata()
{
	os_timer_disarm(&WiFiLinker);
	char info[150];
	char tcpserverip[15];
	Conn.proto.tcp = &ConnTcp;
	Conn.type = ESPCONN_TCP;
	Conn.state = ESPCONN_NONE;
	os_sprintf(tcpserverip, "%s", TCPSERVERIP);
	uint32_t ip = ipaddr_addr(tcpserverip);
	os_memcpy(Conn.proto.tcp->remote_ip, &ip, 4);
	Conn.proto.tcp->local_port = espconn_port();
	Conn.proto.tcp->remote_port = TCPSERVERPORT;
	espconn_regist_connectcb(&Conn, tcpclient_connect_cb);
	espconn_regist_reconcb(&Conn, tcpclient_recon_cb);
	espconn_regist_disconcb(&Conn, tcpclient_discon_cb);
	#ifdef PLATFORM_DEBUG
	console_printf("Start espconn_connect to " IPSTR ":%d\r\n", IP2STR(Conn.proto.tcp->remote_ip), Conn.proto.tcp->remote_port);
	#endif

	#endif
	sint8 espcon_status = espconn_connect(&Conn);
	#ifdef PLATFORM_DEBUG
	switch(espcon_status)
	{
		case ESPCONN_OK:
			console_printf("TCP created.\r\n");
			break;
		case ESPCONN_RTE:
			console_printf("Error connection, routing problem.\r\n");
			break;
		case ESPCONN_TIMEOUT:
			console_printf("Error connection, timeout.\r\n");
			break;
		default:
			console_printf("Connection error: %d\r\n", espcon_status);
	}
	#endif
	if(espcon_status != ESPCONN_OK) {
		#ifdef LWIP_DEBUG
		list_espconn_tcp(&Conn);
		list_lwip_tcp_psc();
		#endif
		os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&WiFiLinker, 1000, 0);
	}
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
				ets_uart_printf("Start TCP connecting...\r\n");
				#endif
				connState = TCP_CONNECTING;
				senddata();
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

void ICACHE_FLASH_ATTR wifi_connect(void)
{

	//if(wifi_get_opmode() != USE_WIFI_MODE)
	{
		#ifdef PLATFORM_DEBUG
		console_printf("ESP8266 is %s mode, restarting in %s mode...\r\n", WiFiMode[wifi_get_opmode()], WiFiMode[USE_WIFI_MODE]);
		#endif
		if(USE_WIFI_MODE & STATION_MODE)
			setup_wifi_st_mode();
	}

	if(USE_WIFI_MODE & STATION_MODE)
		wifi_get_macaddr(STATION_IF, macaddr);

	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);

	if(wifi_station_get_auto_connect() == 0)
		wifi_station_set_auto_connect(1);

	#ifdef PLATFORM_DEBUG
	console_printf("Wi-Fi mode: %s\r\n", WiFiMode[wifi_get_opmode()]);

	if(USE_WIFI_MODE & STATION_MODE)
	{
		struct station_config stationConfig;
		if(wifi_station_get_config(&stationConfig)) {
			console_printf("STA config: SSID: %s, PASSWORD: %s\r\n",
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




