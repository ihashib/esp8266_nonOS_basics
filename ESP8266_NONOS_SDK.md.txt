*SDK Setup*
#NonOS SDK
# 2.2.1 (tested)
#        https://programs74.ru/udkew-en.html
#        http://dl.programs74.ru/get.php?file=EspressifESP8266DevKit
#         Note: Install in C drive.
# Latest (not tested yet)
#         https://github.com/espressif/ESP8266_NONOS_SDK/releases
#        https://github.com/espressif/ESP8266_RTOS_SDK/releases
#MinGW
# Download
# https://sourceforge.net/projects/mingw/files/Installer/
# https://sourceforge.net/projects/mingw/files/latest/download?source=files
# https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe/download
# Install
# After installer comes to this page, select the following components
        #mingw32-base, mingw32-gcc-g++, msys-base
#  
         # Eclipse C/C++ IDE for developers
# download
# https://www.eclipse.org/downloads/packages/
# Install
*Project Setup*
# Open Eclipse
# Open existing project in Eclipse
# Espressif > examples > ESP8266 > Blinky2
# Project tools  
# Build targets
# all : builds all targets
# clear: clears built targets
# flash: flashes to ESP8266                
//Do not use flash here 
# Makefile
# SPI_MODE                = DIO                //set flash mode
# ESPBAUD                = 115200        //set flash baudrate
# ESPPORT                = COM3        //set ESP8266 com port
# Config files
# In the Espressif > examples > ESP8266 file, you will find common and settings files. They are necessary files.
# In case of different ESP8266 flash memory sizes, you need to change the memory settings in the settings files.
# Open settings.mk with notepad
# Check comport:
#  ESPPORT                ?= COM3
# For our ESP8266MOD generic board, we used Non-FOTA addresses.
# # 4 : 4096 KB (512 KB + 512 KB)
# SPI_SIZE_MAP ?= 4
         * # then save and exit
# If you are using FOTA addressing, look up the official guide (4.2:FOTA)
# https://www.openhacks.com/uploadsproductos/2a-esp8266-sdk_getting_started_guide_en.pdf
*Flash Program to ESP8266*
# Downloads
# https://www.espressif.com/en/support/download/other-tools
# Download Flash Download Tools
# Flash program
# Open ESP8266 Download Tool
# In SPIDownload tab
# We need to set file directory and addresses for them
#   
# esp_init_data_default.bin (addr: 0x3FC000) and blank.bin (addr: 0x3FE000) is found in C:\Espressif\ESP8266_SDK\bin\
# eagle.flash.bin( addr: 0x00000) and eable.irom0text.bin (addr: 0x10000) are found in the project directory > firmware
#   
# Set SPI speed to 40MHZ, SPI mode to DIO.
# Set com port and baud rate
# Press reset on the ESP8266 and press Start.
# After flashing, again press reset.
*Useful links*
#https://boseji.com/posts/esp8266-unofficial-windows-development-environment-a-restart-in-2018/
#https://www.openhacks.com/uploadsproductos/2a-esp8266-sdk_getting_started_guide_en.pdf
* Basic programming structure*
# Blank program structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ets_sys.h>
#include <osapi.h>
#include “user_interface.h”
#include <os_type.h>
#include <gpio.h>
#include “driver/uart.h”
//-------------Declare os timer----------------------
LOCAL os_timer_t OS_TIMER;
//-------------Flash config----------------------------
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
//------------------------Radio config-------------------------------
void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}
//------------------------Main-----------------------------------------
void ICACHE_FLASH_ATTR user_init(void)
{
//Setup os timer
os_timer_disarm(&OS_TIMER);
//void os_timer_disarm (ETSTimer *ptimer)


//Set the os timer to execute a callback function
os_timer_setfn(&OS_TIMER, (os_timer_func_t *) fn, (void *) 0);
/*Function:
Set timer callback function
Prototype:
void os_timer_setfn(
 ETSTimer *ptimer,
 ETSTimerFunc *pfunction,
 void *parg
)
Parameters:
ETSTimer *ptimer : Timer structure
TESTimerFunc *pfunction : timer callback function
void *parg : callback function parameter
Return:
null */
//generate delay using os timer
os_timer_arm(&OS_TIMER, 1000, 1)
/*Function: Initializing the timer. 
Prototype: 
void os_timer_arm ( ETSTimer *ptimer, uint32_t milliseconds, bool repeat_flag ) 
Parameters: 
ETSTimer *ptimer : Timer structure 
uint32_t milliseconds : Timing, Unit: millisecond, the maximum value is 6871947ms 
bool repeat_flag : Whether the timer will be invoked repeatedly or not 
Return: 
null*/
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*UART*
# ESP8266 has 2 UARTs.
# UART0 is used for rx and tx
# UART1 is used for tx, is used for DEBUG
uart_init
# Function:
# Initializes baud rates of the two uarts
# Prototype:
# void uart_init(
#  UartBautRate uart0_br,
# UartBautRate uart1_br
# )
# Parameters:
# UartBautRate uart0_br : uart0 baud rate
# UartBautRate uart1_br : uart1 baud rate
# Baud Rates: t
# typedef enum { 
BIT_RATE_9600 = 9600,
 BIT_RATE_19200 = 19200, 
BIT_RATE_38400 = 38400, 
BIT_RATE_57600 = 57600,
 BIT_RATE_74880 = 74880,
 BIT_RATE_115200 = 115200,
 BIT_RATE_230400 = 230400, 
BIT_RATE_460800 = 460800, 
BIT_RATE_921600 = 921600
 } UartBautRate; 
# Return: null 
2. uart0_tx_buffer
# Function:
# Sends user-defined data through UART0
# Prototype:
# void uart0_tx_buffer(uint8 *buf, uint16 len)
# Parameter:
# uint8 *buf : data to send later
# uint16 len : the length of data to send later
# Return:
# null
3. uart0_rx_intr_handler
# VERY IMP: when uart_init is called, uart_config is also called and UART0 RX is configured as RX IT automatically. SO RX IT is always enabled when UART0 is initialized.
# Function:
# UART0 interrupt processing function. Users can add the processing of
# received data in this function. (Receive buffer size: 0x100; if the received
# data are more than 0x100, pls handle them yourselves.)
# Prototype:
# void uart0_rx_intr_handler(void *para)
# Parameter:
# void *para : the pointer pointing to RcvMsgBuff structure
# Return:
 # null
# UART TX example
//IMP: you need uart.c and uart.h from SDK to your project driver.
# project/driver/uart.c
# project/include/driver/uart_register.h and uart.h
//INCLUDES and other blank structure code


LOCAL void ICACHE_FLASH_ATTR transmit(void *arg)
{
ets_uart_printf("Hello World!!!!\r\n");
}
void ICACHE_FLASH_ATTR(void)
{
//uart0 and uart1 baud rate init
uart_init(BIT_RATE_115100, BIT_RATE_115200);


//disable timer
os_timer_disarm(&OS_TIMER);


//callback transmit function with no parameter
os_timer_setfn(&OS_TIMER, (os_timer_func_t*) transmit, (void * 0);


//delay
os_timer_arm(&OS_TIMER, DELAY, 1);
}
# UART RX IT example
//IMP: you need uart.c and uart.h from SDK to your project driver.
# project/driver/uart.c
# project/include/driver/uart_register.h and uart.h
# In project/driver/uart.c
uart0_rx_intr_handler(void *para)
{
//this func is already in lib
//start your code from “you can add your handle code below”
}
# In project/user/user_main.c
# Just initialize uart.
*