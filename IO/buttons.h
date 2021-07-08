#ifndef BUTTONS_H_
#define BUTTONS_H_

// *******************************************************
// 
// buttons.h
//
//  File for initializing buttons sets the pins and the pin bases
//  
//  Also has a debouncer funciton 
//
//  Authors Jackson Allred, Pieter Leigh, Dan Brock Ronen.
//  Last modified:   06.20.1969
// 
// *******************************************************


#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Constants
//*****************************************************************************
enum butNames {UP = 0, DOWN, LEFT, RIGHT, NUM_BUTS}; // the names for each of the buttons as well as the amount
enum butStates {RELEASED = 0, PUSHED, NO_CHANGE}; // the current button state

// UP button
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE // The peripheral for the up button
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE // the port base for the up button
#define UP_BUT_PIN  GPIO_PIN_0 // The specific pin for the up button
#define UP_BUT_NORMAL  false // the initial electrical state of the up button

// DOWN button
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD // The peripheral for the down button
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE // the port base for the down button
#define DOWN_BUT_PIN  GPIO_PIN_2 // The specific pin for the down button 
#define DOWN_BUT_NORMAL  false // the initial electrical state of the down button

// LEFT button
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF // The peripheral for the left button
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE // the port base for the left button
#define LEFT_BUT_PIN  GPIO_PIN_4 // The specific pin for the left button
#define LEFT_BUT_NORMAL  true // the initial electrical state of the left button

// RIGHT button
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF // The peripheral for the right button
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE // the port base for the right button
#define RIGHT_BUT_PIN  GPIO_PIN_0 // The specific pin for the right button
#define RIGHT_BUT_NORMAL  true // the initial electrical state of the eight button

#define NUM_BUT_POLLS 3


// *******************************************************
// Functions
// *******************************************************

// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.

// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants above.
void
initButtons (void);

// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
void
updateButtons (void);

// checkButton: Function returns the new button state if the button state
// (PUSHED or RELEASED) has changed since the last call, otherwise returns
// NO_CHANGE.  The argument butName should be one of constants in the
// enumeration butStates, excluding 'NUM_BUTS'. Safe under interrupt.
// @param butName 
uint8_t
checkButton (uint8_t butName);

#endif /*BUTTONS_H_*/
