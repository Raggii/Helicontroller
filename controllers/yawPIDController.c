// *******************************************************
// 
// PIDController.h
//
//  This contains the error calculator for the yaw
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
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "utils/ustdlib.h"


/** gets the shortest error for the yaw so that the helicopter moves in the correct direction
@param current the position of the helicopter in an angle
@param target the position of the desired angle
@return the error for the yaw as a direction */ 
int16_t
getShortestYawError(int16_t current, int16_t target)
{
    int16_t clockwiseError = 0;
    int16_t counterClockwiseError = 0;

    if (target > current) {
        counterClockwiseError = -current - 360 + target;
        clockwiseError = target - current;
    } else {
        counterClockwiseError = target - current;
        clockwiseError = 360 - current + target;
    }

    if (clockwiseError > -counterClockwiseError) {
        return counterClockwiseError;
    } else {
        return clockwiseError;
    }




}
