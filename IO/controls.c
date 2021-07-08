//*****************************************************************************
//
// controls.c
//
// The file to initalize and control the input buttons for the rig.
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************

#include "buttons.h"
#include "switches.h"

// *******************************************************
// Functions
// *******************************************************

/** initializes the controls for the rig */
void 
initControls(void)
{
    initButtons();
    initSwitches();
}
/** checks if the up button is pushed returns and boolean 
@return returns a boolean checking if the up button has been pushed */
bool 
isUpButtonPressed(void)
{
    return checkButton(UP) == PUSHED;
}
/** checks if the left button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool 
isLeftButtonPressed(void)
{
    return checkButton(LEFT) == PUSHED;
}

/** checks if the right button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool
isRightButtonPressed(void)
{
    return checkButton(RIGHT) == PUSHED;
}

/** checks if the down button is pushed returns and boolean 
@return returns a boolean checking if the left button has been pushed */
bool
isDownButtonPressed(void)
{
    return checkButton(DOWN) == PUSHED;
}

/** returns a boolean depending on if switch one is toggled or not
@return bool depending on the state of flying */
bool
isFlyingSelected(void) {
    return checkSwitch(ONE);
}


/** Updates the controls by calling the update buttons function  */
void 
updateControls(void)
{
    updateButtons();
    updateSwitches();
}
