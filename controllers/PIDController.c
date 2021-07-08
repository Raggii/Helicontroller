// *******************************************************
// 
// PIDController.c
//
//  This contains the PID controller structs and the functions that are needed to operate them.
//
//  Authors Jackson Allred, Pieter Leigh, Dan Brock Ronen.
//  Last modified:   06.20.1969
// 
// *******************************************************


// Standard C libraries
#include <stdint.h>
#include <stdbool.h>

// driver libraries & OLED Interface
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"

// Other
#include "PIDController.h"
#include "../IO/uart.h"

#define MAX_ERROR_SUM 100  // Controls the percentage max error sum

static int32_t maxErrorSum = MAX_ERROR_SUM * CONTROLLER_RESPONSE_SCALE; // Defines the max error sum but in terms of the controller response
                                                                        // scale.


// *******************************************************
// Functions
// *******************************************************

/**
 * Initializes the PID controller, given the controller entity, gains and controller rate.
 * 
 * @param controller (PIDController_t*) This is a pointer to the PID controller struct.
 * @param controllerRate (int32_t) This is the (expected) rate that the PID is called.
 * @param proportionalGain (int32_t) This is the Kp proportional gain for the PID.
 * @param intergralGain (int32_t) This is the Ki intergral gain for the PID.
 * @param derivativeGain (int32_t) This is the Kd derivative gain for the PID. */
void
controllerPIDInit(PIDController_t* controller, int32_t controllerRate, int32_t proportionalGain, int32_t integralGain, int32_t derivativeGain)
{
    controller->controllerRate = controllerRate;

    // Watcher knight gains
    controller->proportionalGain = proportionalGain;
    controller->derivativeGain = derivativeGain;
    controller->integralGain = integralGain;


    // Integral & derivative calculation requirements
    controller->previousError = 0;
    controller->errorSum = 0;

}

/**
* Returns the response values for the error affected by the PID.
* 
* @param controller (PIDController_t*) The pointer to the PID controller struct object.
* @param error (int32_t) The error we use the calculate the response for it.
* @return The response given the error for the controller provided (with the defined gains within that controller) */
int32_t
returnNewResponse(PIDController_t* controller, int32_t error)
{

    int32_t propResponse = 0;
    int32_t derResponse = 0;
    int32_t intResponse = 0;

    // Proportional response calculation
    propResponse = controller->proportionalGain * error;

    // Integral response calculation
    if (controller->errorSum + error >= maxErrorSum) {
        controller->errorSum = maxErrorSum;
    } else if (controller->errorSum+error <= -maxErrorSum) {
        controller->errorSum = -maxErrorSum;
    } else {
        controller->errorSum += error;
    }

    intResponse = controller->integralGain * controller->errorSum  / (controller->controllerRate * 2);

    // Derivative response calculation
    derResponse = controller->derivativeGain * (error - controller->previousError) * controller->controllerRate;
    controller->previousError = error;


    return propResponse + intResponse + derResponse;
}
