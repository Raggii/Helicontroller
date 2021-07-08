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


#ifndef PIDCONTROLLER_H_
#define PIDCONTROLLER_H_

#include "../circBufT.h"

#define CONTROLLER_RESPONSE_SCALE 10 // A scaling factor to reduce the gains below 0 without using floats or doubles :)


typedef struct {

    // Constants
    int32_t proportionalGain; // the scaling factor for the proportional gain
    int32_t derivativeGain; // the scaling factor for the derivative gain
    int32_t integralGain; // the scaling factor for the integral gain
    int32_t controllerRate; // the rate at which the controller is recalculated

    // Current values
    int32_t previousError; // the error from the past calculation
    int32_t errorSum; // the accumulated error for the system

} PIDController_t; // the PID controller that holds the necessary values for any controller


// *******************************************************
// Functions
// *******************************************************

/**
* Returns the response values for the error affected by the PID.
* @param controller (PIDController_t*) The pointer to the PID controller struct object.
* @param error (int32_t) The error we use the calculate the response for it.
* @return The response given the error for the controller provided (with the defined gains within that controller) */
int32_t
returnNewResponse(PIDController_t* controller, int32_t error);

/**
 * Initializes the PID controller, given the controller entity, gains and controller rate.
 * @param controller (PIDController_t*) This is a pointer to the PID controller struct.
 * @param controllerRate (int32_t) This is the (expected) rate that the PID is called.
 * @param proportionalGain (int32_t) This is the Kp proportional gain for the PID.
 * @param intergralGain (int32_t) This is the Ki intergral gain for the PID.
 * @param derivativeGain (int32_t) This is the Kd derivative gain for the PID. */
void
controllerPIDInit(PIDController_t* controller, int32_t controllerRate, int32_t proportionalGain, int32_t integralGain, int32_t derivativeGain);

#endif /* PIDCONTROLLER_H_ */
