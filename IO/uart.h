// *******************************************************
// 
//  uart.h
//
//  File for computing the uart functions
//  
//
//  Authors Jackson Allred, Pieter Leigh, Dan Brock Ronen.
//  Last modified:   06.20.1969
// 
// *******************************************************


#ifndef IO_UART_H_
#define IO_UART_H_

#include <stdint.h>
#include <stdbool.h>

//********************************************************
// Constants
//********************************************************
#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4
#define MAX_STR_LEN 16
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// Prototypes
//********************************************************

/** initialiseUSB_UART - 8 bits, 1 stop bit, no parity */
void
initUart (void);

/** Transmit a string via UART0
@param a char to send to the UART response */
void
UARTSend (char *pucBuffer);


#endif /* IO_UART_H_ */
