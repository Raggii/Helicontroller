//*****************************************************************************
//
// Switches.h
//
// The holds the information for the switches on the tiva board
//
// Author:  Dan Brock Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#ifndef IO_SWITCHES_H_
#define IO_SWITCHES_H_

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Constants
//*****************************************************************************
enum switchNames {ONE = 0, TWO, NUM_SWITCHES};
enum switchStates {OFF = 0, ON};


//********************************************************
// Functions
//********************************************************

// Switch one
#define SWITCH_ONE_PERIPH  SYSCTL_PERIPH_GPIOA
#define SWITCH_ONE_PORT_BASE  GPIO_PORTA_BASE
#define SWITCH_ONE_PIN  GPIO_PIN_7

// Switch two
#define SWITCH_TWO_PERIPH  SYSCTL_PERIPH_GPIOA
#define SWITCH_TWO_PORT_BASE  GPIO_PORTA_BASE
#define SWITCH_TWO_PIN  GPIO_PIN_6


/** initializes the switch peripheral ports */
void
initSwitches(void);

/** updates the wanted switches */
void
updateSwitches(void);


/** checks the input switch for state and returns that state
@param switchName the name of the switch wanted
@return the state of the switch */
uint8_t
checkSwitch (uint8_t switchName);

#endif /* IO_SWITCHES_H_ */
