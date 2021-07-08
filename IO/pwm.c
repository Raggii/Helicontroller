//*****************************************************************************
//
// PWM.c
//
// This modual computes the pwm signal for output to the helicopter
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "utils/ustdlib.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "pwm.h"

// *******************************************************
// Globals to module
// *******************************************************

// sets initial pwm variables
#define PWM_RATE_STEP_HZ   50 
#define PWM_RATE_MIN_HZ    50
#define PWM_RATE_MAX_HZ    400
#define PWM_FIXED_DUTY     5
#define PWM_DIVIDER        4

#define PWM_FREQUENCY 250 // the frequency at which the Pwm operates

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 3)
//  ---Tail Rotor PWM: PF1, J4-05
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1


// *******************************************************
// Functions
// *******************************************************

/** sets the pwm signal for the main rotor
@param ui8Duty is the duty cycle for the rotor */
void
setMainPWM (uint8_t ui8Duty)
{
    // Calculate the PWM period corresponding to the frequency
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui8Duty / 100);
}

/** sets the pwm signal for the tail rotor
@param ui8Duty is the duty cycle for the rotor as a unsigned 8 bit int */
void
setTailPWM (uint8_t ui8Duty)
{
    // Calculate the PWM period corresponding to the frequency
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / PWM_FREQUENCY;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * ui8Duty / 100);
}

/** clams the Pwm signal between wanted variables
@param duty is a 32 bit input for the duty cycle
@return the clamed pwm signal */
uint8_t
clampDutyCycle(int32_t duty)
{
    if (duty > 85) {
        duty = 85;
    } else if (duty < 5) {
        duty = 5;
    }
    return duty;
}

/** disables the rotors */
void
shutOffPwmRotors()
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}

/** Enables the rotors */
void
startUpPwmRotors()
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

/** init and enables the required pwm */
void
initPwm (void)
{
    // Set up the main motor signal
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);


    // Set the initial PWM parameters for main motor
    setMainPWM (PWM_FIXED_DUTY);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // ----------------------------------------------------------------------------------------------

    // Set the initial PWM parameters for tail motor
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the initial PWM parameters
    setTailPWM(PWM_FIXED_DUTY);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Turn on the motors!
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}


