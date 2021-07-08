// *******************************************************
// 
// yawPIDController.c
//
//  This contains the error calculator for the yaw
//
//  Authors Jackson Allred, Pieter Leigh, Dan Brock Ronen.
//  Last modified:   06.20.1969
// 
// *******************************************************

#ifndef YAWPIDCONTROLLER_H_
#define YAWPIDCONTROLLER_H_


/** gets the shortest error for the yaw so that the helicopter moves in the correct direction
@param current the position of the helicopter in an angle
@param target the position of the desired angle
@return the error for the yaw as a direction */ 
int16_t getShortestYawError(int16_t current, int16_t target);

#endif /* YAWPIDCONTROLLER_H_ */
