// *******************************************************
// 
// yawController.c
//
//  This handles the specific functions and constants needed to control the yaw
//
//  Authors Dan Brock Ronen, Jackson Allred, Pieter Leigh.
//  Last modified:   06.20.1969
// 
// *******************************************************

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "yawController.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)


#include "../circBufT.h"

#include "../IO/pwm.h"

#define PHASE_A_PERIPH SYSCTL_PERIPH_GPIOB
#define PHASE_A_BASE_PORT GPIO_PORTB_BASE
#define PHASE_A_PIN GPIO_PIN_0

#define PHASE_B_PERIPH SYSCTL_PERIPH_GPIOB
#define PHASE_B_BASE_PORT GPIO_PORTB_BASE
#define PHASE_B_PIN GPIO_PIN_1

#define REF_PERIPH SYSCTL_PERIPH_GPIOC
#define REF_BASE_PORT GPIO_PORTC_BASE
#define REF_PIN GPIO_PIN_4

#define ANGLE_CHANGE_PER_TICK 0.80357142857 // the angle change for each tick change
// 0.80357142857
#define TOTAL_TICKS 448 // the maximum amount of ticks in the circle

#define REF_SEEK_TAIL_DUTY 45 // the initial duty cycle of the heli to find the reference position

int16_t currentTick; // the current position in ticks around the dashed circle
uint8_t GpioPinReadResult = 0; // the value read from the GPOI input pin for yaw
bool phaseA = 0; // current state of A in the FSM
bool phaseB = 0; // current state of B in the FSM
bool phasePrimeA; // the past state of A in the FSM
bool phasePrimeB; // the past state of B in the FSM
int8_t phasesHashMap[16] = { 0,  1, -1,  0,
                           -1,  0,  0,  1,
                            1,  0,  0, -1,
                            0, -1,  1,  0}; // a hashmap for mapping the direction from the current states of the FSM



/** the initializing function for the yaw controller 
    also builds the hash map for the state machine*/
void
initYawController(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);

    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPadConfigSet (GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);

    GPIOIntRegisterPin(GPIO_PORTB_BASE, 0, yawIntHandler);
    GPIOIntRegisterPin(GPIO_PORTB_BASE, 1, yawIntHandler);

    GPIOIntTypeSet(GPIO_PORTB_BASE,  GPIO_PIN_0, GPIO_BOTH_EDGES);
    GPIOIntTypeSet(GPIO_PORTB_BASE,  GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_1);

}

/** finds index for the hash table from the states in the FSM */
uint8_t
hash(uint8_t A, uint8_t B, uint8_t primeA, uint8_t primeB)
{
    return primeB + (primeA << 1) + (B << 2) + (A << 3);
}

/** returns the angle depending on the current tick 
@return the angle from the tick amount */
uint16_t
getAngle(void)
{
    return ANGLE_CHANGE_PER_TICK * currentTick;
}


/** changes the current direction of the helicopter */
void
updateCurrentDirection()
{

    int8_t directionIncrement = phasesHashMap[hash(phaseA, phaseB, phasePrimeA, phasePrimeB)];

    phaseA = phasePrimeA;
    phaseB = phasePrimeB;

    currentTick += directionIncrement;
    if (currentTick < 0) {
        currentTick = TOTAL_TICKS+currentTick;
    } else if (currentTick >= TOTAL_TICKS) {
        currentTick = 0;
    }

}

/** checks the yaw input pin at a regular pace */
void
yawIntHandler(void)
{

    GpioPinReadResult = GPIOPinRead(PHASE_A_BASE_PORT, PHASE_A_PIN | PHASE_B_PIN);

    phasePrimeA = GpioPinReadResult & 1;
    phasePrimeB = (GpioPinReadResult & 2) / 2;

    updateCurrentDirection();
    GPIOIntClear(PHASE_A_BASE_PORT,  GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

/** function to find the initial position for the helicopter */
void
findInitialPos(void) {

    setTailPWM(REF_SEEK_TAIL_DUTY);
    while(GPIOPinRead(REF_BASE_PORT, REF_PIN) != 0) { // Assuming that 0 is the
        //pole for the value
        continue;
    }
    currentTick = 0;
    setTailPWM(0);
}

