// *******************************************************
// 
// yawController.h
//
//  This handles the specific functions and constants needed to control the yaw
//
//  Authors Dan Brock Ronen, Jackson Allred, Pieter Leigh.
//  Last modified:   06.20.1969
// 
// *******************************************************

#ifndef YAWCONTROLLER_H_
#define YAWCONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>

enum phase {phaseOne = 1, phaseTwo, phaseThree, phaseFour}; // the current phase of the FSM
enum direction {clockwise = 1, antiClockwise = -1, stationary = 0}; // the direction for the helicopter

/** the initializing function for the yaw controller 
    also builds the hash map for the state machine*/
void
initYawController(void);

/** checks the yaw input pin at a regular pace */
void
yawIntHandler(void);

/** changes the current direction of the helicopter */
void
updateCurrentDirection();

/** finds index for the hash table from the states in the FSM */
uint8_t
hash(uint8_t A, uint8_t B, uint8_t phaseA, uint8_t phaseB);

/** returns the angle depending on the current tick 
@return the angle from the tick amount */
uint16_t
getAngle(void);

/** function to find the initial position for the helicopter */
void
findInitialPos(void);


#endif /* YAWCONTROLLER_H_ */
