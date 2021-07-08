//*****************************************************************************
//
// controls.h
//
// The file to initalize and control the input buttons for the rig.
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#ifndef CONTROLS_H_
#define CONTROLS_H_

// *******************************************************
// Functions
// *******************************************************

/** initializes the controls for the rig*/
void
initControls(void);

/** checks if the up button is pushed returns and boolean 
@return returns a boolean checking if the up button has been pushed */
bool
isUpButtonPressed(void);

/** checks if the left button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool
isLeftButtonPressed(void);

/** checks if the right button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool
isRightButtonPressed(void);

/** checks if the down button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool
isDownButtonPressed(void);

/** returns a boolean depending on if the flying is toggled or not
@return bool depending on the state of flying */
bool
isFlyingSelected(void);

/** Updates the controls by calling the update buttons function */
void
updateControls(void);

#endif /* CONTROLS_H_ */
