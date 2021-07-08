//*****************************************************************************
//
// display.h
//
// This modual holds the information to display the text and images on the LCD screen
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************


#ifndef DISPLAY_H_
#define DISPLAY_H_
// *******************************************************
// Globals to module
// *******************************************************

enum screenTypes {percentageScreen = 0, meanAdcScreen = 1, blankScreen = 2}; // the different screen types

// *******************************************************
// Functions
// *******************************************************

/** Returns the currentScreen layout to display on the LCD
@return returns the currentScreen as a uint8_t */
uint8_t
getCurrentScreen(void);

/** Changes the current display screen to the inputted one
@param newScreen the screen to change to */
void
setCurrentScreen(uint8_t newScreen);

/** Starts the display by intialising the OLEDInitialise function*/
void
initDisplay (void);

/** draws a line from a start position to a finishing position
@param xStart the x position for the start of the line
@param yStart the y position for the start of the line
@param xFinish the x position for the end of the line
@param yFinish the y position for the end of the line*/
void
drawLine(int xStart, int yStart, int xFinish, int yFinish);

/** Draws the "Helicopter" representaion on the screen that updates depending on the Helicopters altitude
@param xCoordHeight is an integer from the height of the given rig*/
void
drawArrow(int xCoordHeight);

/** Draws the panel on the side of the screen showing the helicopters height*/
void
drawSidePanel(void);

/** Function to display the mean ADC value (10-bit value, note) and sample count.
@param percentAltitude is the input for the percentage Altitude screen for the Altitude*/
void
displayPercentageScreen(uint8_t percentAltitude);

/** Is the display for the mean ADC screen showing that value
@param altitudeCurrent the current value
@param altitudeTarget the target value
@param angleCurrent the current anlge
@param angleTarget the target value*/
void
displayMainScreen(uint8_t flightMode, int16_t targetHeight, int16_t currentHeight, int16_t targetYaw, int16_t currentYaw);

/** sets the screen to hold the angle
@param angle is the current read angle to be displayed on the screen*/
void
displayBlankScreen(uint16_t angle);

/** Selects the screen to be displayed*/
void
nextScreen(void);

/** clears the current screen*/
void
clearScreen(void);

#endif /* DISPLAY_H_ */
