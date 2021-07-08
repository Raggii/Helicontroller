//*****************************************************************************
//
// display.c
//
// This holds the information to display the text and images on the LCD screen
//
// Author:  Dan Ronen, Jackson Allred, Pieter Leigh
// Last modified:   06.20.1969
//
//*****************************************************************************



#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOledGrph.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "display.h"


// *******************************************************
// Globals to module
// *******************************************************

#define ARROW_TIP_Y 22 // the position in the Y domain within the LCD screen of the tip of the arrow
#define ARROW_HALF_WIDTH 3 // Half of the width of the arrow displaying the height

static uint8_t currentScreen = percentageScreen; // the current sreen displayed

enum states {STARTUP_STATE = 0, LANDED_STATE, CALIBRATION_STATE, FLYING_STATE, LANDING_STATE};// the state machine holding the different state for the helicopter
// *******************************************************
// Functions
// *******************************************************

/** Returns the currentScreen layout to display on the LCD
@return returns the currentScreen as a uint8_t */
uint8_t 
getCurrentScreen(void) {
    return currentScreen;
}

/** Changes the current display screen to the inputted one
@param newScreen the screen to change to */
void 
setCurrentScreen(uint8_t newScreen)
{
    currentScreen = newScreen;
}

/** Starts the display by intialising the OLEDInitialise function */
void 
initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

/** draws a line from a start position to a finishing position
@param xStart the x position for the start of the line
@param yStart the y position for the start of the line
@param xFinish the x position for the end of the line
@param yFinish the y position for the end of the line */
void 
drawLine(int xStart, int yStart, int xFinish, int yFinish)
{
    OrbitOledMoveTo(xFinish,yFinish);
    OrbitOledLineTo(xStart, yStart);
}

/** Draws the "Helicopter" representaion on the screen that updates depending on the Helicopters altitude
@param xCoordHeight is an integer from the height of the given rig */
void 
drawHelicopter(int xCoordHeight)
{
    int xActualHeight = 125 - (xCoordHeight/2); // 0 will be at 127 and 10 will be at 55. // xCoord is from 0 -100 100/2 =~ 55?
    //since its 8 heigt the x coord will be different per layer
    //height goes from 0 to 100 and the screen has only 55 pixels we can use not including the bottom buffer
    // seperate lines into layers?

    int8_t i;

    for (i = -1; i <= 1; i+=2) {
        OrbitOledMoveTo(xActualHeight, ARROW_TIP_Y);
        OrbitOledLineTo(xActualHeight + (ARROW_HALF_WIDTH * i), ARROW_TIP_Y - 3);
        OrbitOledLineTo(xActualHeight + (ARROW_HALF_WIDTH * i), ARROW_TIP_Y - 14);
        OrbitOledLineTo(xActualHeight, ARROW_TIP_Y - 11);
    }
}
/** Draws the panel on the side of the screen showing the helicopters height */
void 
drawSidePanel(void)
{
    int currentX = 74;
    int currentY = 31;
    int i = 0;
    drawLine(currentX, currentY,currentX+51,currentY);
    for(i = 0; i < 11; i++)
    {
        if(i % 2 == 0)
        {
            drawLine(currentX + 1,currentY,currentX + 1,currentY - 3);
        } else
        {
            drawLine(currentX + 1,currentY,currentX + 1,currentY - 2);
        }
        currentX = currentX + 5;
    }
}

/** Is the display for the mean ADC screen showing that value
@param altitudeCurrent the current value
@param altitudeTarget the target value
@param angleCurrent the current anlge
@param angleTarget the target value */
void 
displayMainScreen(uint8_t flightMode, int16_t targetHeight, int16_t currentHeight, int16_t targetYaw, int16_t currentYaw)
{
    //clearScreen();
    char string[17];  // 16 characters across the display
    //drawSidePanel(); ** add TO second screen
    //drawHelicopter(percentAltitude);
    OLEDStringDraw ("                ", 0, 0);

    switch (flightMode) {
        case STARTUP_STATE:
            usnprintf (string, sizeof(string), "!!RESET SWITCH!!");
            break;
        case LANDED_STATE:
            usnprintf (string, sizeof(string), "     LANDED     ");
            break;
        case CALIBRATION_STATE:
            usnprintf (string, sizeof(string), "   SEEKING REF  ");
            break;
        case FLYING_STATE:
            usnprintf (string, sizeof(string), "   IN-FLIGHT    ");
            break;
        case LANDING_STATE:
            usnprintf (string, sizeof(string), "    LANDING     ");
            break;
    }
    OLEDStringDraw (string, 0, 0);

    usnprintf (string, sizeof(string), "H| c: %3d t: %3d", currentHeight, targetHeight);
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Y| c: %3d t: %3d", currentYaw, targetYaw);
    OLEDStringDraw (string, 0, 2);

    usnprintf (string, sizeof(string), " PLS FLY SAFELY ", currentYaw, targetYaw);
    OLEDStringDraw (string, 0, 3);
}


/** Displays a blank screen  */
void
displayBlankScreen(uint16_t angle)
{
    char string[17];  // 16 characters across the display
    usnprintf (string, sizeof(string), "more fun stuff", angle); // The screen when there is not enough fun in ones day
    OLEDStringDraw (string, 0, 0);
}

/** Selects the screen to be displayed */
void 
nextScreen(void)
{
    switch(currentScreen) {
        case percentageScreen:
            currentScreen = meanAdcScreen;
            break;

        case meanAdcScreen:
            currentScreen = blankScreen;
            break;

        case blankScreen:
            currentScreen = percentageScreen;
            break;
    }
}

/** clears the current screen */
void 
clearScreen(void)
{
    OrbitOledClear();
}




















