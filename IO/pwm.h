//*****************************************************************************
//
// PWM.h
//
// This modual computes the pwm signal for output to the helicopter
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#ifndef IO_PWM_H_
#define IO_PWM_H_

// *******************************************************
// Globals to module
// *******************************************************


#define PWM_DIVIDER_CODE SYSCTL_PWMDIV_4

// *******************************************************
// Functions
// *******************************************************


/** sets the pwm signal for the main rotor
@param ui8Duty is the duty cycle for the rotor */
void
setMainPWM (uint8_t ui8Duty);

/** sets the pwm signal for the tail rotor
@param ui8Duty is the duty cycle for the rotor as a unsigned 8 bit int */
void
setTailPWM (uint8_t ui8Duty);

/** clams the Pwm signal between wanted variables
@param duty is a 32 bit input for the duty cycle
@return the clamed pwm signal */
uint8_t
clampDutyCycle(int32_t duty);

/** disables the rotors */
void
shutOffPwmRotors();

/** Enables the rotors */
void
startUpPwmRotors();

/** init and enables the required pwm */
void
initPwm (void);


#endif /* IO_PWM_H_ */
