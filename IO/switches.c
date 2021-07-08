//*****************************************************************************
//
// Switches.c
//
// The holds the information for the switches on the tiva board
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#include "switches.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)

//********************************************************
// Global variables
//********************************************************

static bool switch_state[NUM_SWITCHES]; // the state for each switch

//********************************************************
// Function
//********************************************************

/** initializes the switch peripheral ports */
void
initSwitches(void) {

    SysCtlPeripheralEnable (SWITCH_ONE_PERIPH);
    GPIOPinTypeGPIOInput (SWITCH_ONE_PORT_BASE, SWITCH_ONE_PIN);
    GPIOPadConfigSet (SWITCH_ONE_PORT_BASE, SWITCH_ONE_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);

    SysCtlPeripheralEnable (SWITCH_TWO_PERIPH);
    GPIOPinTypeGPIOInput (SWITCH_TWO_PORT_BASE, SWITCH_TWO_PIN);
    GPIOPadConfigSet (SWITCH_TWO_PORT_BASE, SWITCH_TWO_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);

}

/** updates the wanted switches */
void
updateSwitches(void)
{
    switch_state[ONE] = (GPIOPinRead (SWITCH_ONE_PORT_BASE, SWITCH_ONE_PIN) == SWITCH_ONE_PIN);
    switch_state[TWO] = (GPIOPinRead (SWITCH_TWO_PORT_BASE, SWITCH_TWO_PIN) == SWITCH_TWO_PIN);
}

/** checks the input switch for state and returns that state
@param switchName the name of the switch wanted
@return the state of the switch */
uint8_t
checkSwitch (uint8_t switchName)
{
    if (switch_state[switchName]) {
        return ON;
    } else {
        return OFF;
    }
}
