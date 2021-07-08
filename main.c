// *******************************************************
// 
// Main.c
//
//  This imports all of the needed files and runs the helicopter program
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
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"

// Controllers
#include "controllers/adcController.h"
#include "controllers/yawController.h"
#include "controllers/yawPIDController.h"
#include "controllers/PIDController.h"

// IO
#include "IO/controls.h"
#include "IO/buttons.h"
#include "IO/switches.h"
#include "IO/display.h"
#include "IO/uart.h"
#include "IO/pwm.h"

// Other
#include "circBufT.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define SYS_TICK_INTERRUPT_RATE_HZ 300 // The interrupt rate in HZ
#define PACER_RATE_HZ 6000 // The pacer rate in HZ
#define CONTROLLER_RATE_HZ 10 //slower than systick // play with this
#define ADC_MEAN_SAMPLE_RATE_HZ 600 // The ADC mean sample rate in HZ
#define FLIGHT_LOGIC_RATE_HZ 200 // The update rate of the flight logic controller
#define DISPLAY_UPDATE_RATE_HZ 100 // The update rate for the controls in HZ
#define UART_RATE_HZ 20 // The send rate of the serial interface

#define UART_MAX_LENGTH 64

//PID controller gains
#define ALT_KP 15
#define ALT_KI 40
#define ALT_KD 0
#define YAW_KP 80
#define YAW_KI 120
#define YAW_KD 0
 
#define HIGH_ALT_ADC_OFFSET 1500 // The highest altitude offset for the ADC

#define YAW_OFFSET_CALC_COUNTER_THRESHOLD 10

#define ALT_STEP 10
#define YAW_STEP 15


//#define CONTROLLER_RESPONSE_SCALE_FACTOR 100
//#define YAW_COUPLING_FACTOR_SCALE 100


//*****************************************************************************
// Global variables
//*****************************************************************************
static int32_t low_alt_ADC_limit; // the lower altitude limit for the ADC
static int16_t heightPercent = 0; // the highest percentage
static int16_t yawAngle = 0; // the initial yaw angle
static int32_t heightRawAvg; // the average of the height
static uint16_t slowSysTickCounter; // used for calculations for the PID controller while not flying  

static int16_t heightTarget = 0; // the target for the height
static int16_t yawTarget = 0; // the target yaw 

static uint16_t slowSysTickMax; // the max amount that the slow systick can acchive

static PIDController_t altController; // the controller for the main rotor
static PIDController_t yawController; // the controller for the back rotor

static uint8_t currentPwmAlt = 0; // the current pwm signal for the main rotor
static uint8_t currentPwmYaw = 0; // the current pwm signal for the back rotor

//uint16_t yawOffsetCalc_prev = 0; // not used
//uint8_t yawOffsetCalc_count = 0; // not used

volatile static bool calibrating = true; // checks the state of the heli to see if it has found the calibration point
static bool isCalibrated = false;
volatile static bool isModeFlying = false; // holds the state for the helicopter to see if it is flying


enum states {STARTUP_STATE = 0, LANDED_STATE, CALIBRATION_STATE, FLYING_STATE, LANDING_STATE};// the state machine holding the different state for the helicopter
static uint8_t currentState = STARTUP_STATE; // the current state of the helicopter. landed, Take Off, Flying, landing used for the Enum above

char UARTbuffer[UART_MAX_LENGTH]; // The buffer to hold the output chars for the Uart terminal

/** Updates the duty cycle within the Main rotors structure and then clamps in between selected values
@param controllerResponse the change in response for the main rotor */
void
getMainRotorDutyCycle(int32_t controllerResponse)
{
    currentPwmAlt = clampDutyCycle(controllerResponse / CONTROLLER_RESPONSE_SCALE);
}

/** Updates the duty cycle within the back rotors structure and then clamps in between selected values
@param controllerResponse the change in responce for the back rotor */
void
getTailRotorDutyCycle(int32_t controllerResponse)
{
    currentPwmYaw = clampDutyCycle(controllerResponse / CONTROLLER_RESPONSE_SCALE);
}

/** the handler for systick */
void
SysTickIntHandler(void)
{

    ADCProcessorTrigger(ADC0_BASE, 3);

    // Don't execute the slow SysTick if the helicopter is still calibrating
    if (calibrating) {
        return;
    }

    if((slowSysTickCounter >= slowSysTickMax) && (currentState == FLYING_STATE || currentState ==  LANDING_STATE)) { // Only run PID controller while flying, avoids windup issues when landed
        IntTrigger(INT_EMAC0_TM4C129);
        slowSysTickCounter = 0;
    }
    slowSysTickCounter++;
}

/** the handler for the Pid changes 
reads the angle and height values and changes the responce of the system accordingly */
void PidIntHandler(void) {

    yawAngle = getAngle();

    int32_t altResponse = returnNewResponse(&altController, heightTarget - heightPercent);
    getMainRotorDutyCycle(altResponse);
    setMainPWM(currentPwmAlt);



    int32_t yawResponse = returnNewResponse(&yawController, getShortestYawError(yawAngle, yawTarget) );
    getTailRotorDutyCycle(yawResponse);
    setTailPWM(currentPwmYaw);
}


/** Controls the screen displays and the calabration of the altitude */
void 
switchesUpdate(void)
{
    updateSwitches();
    if (isFlyingSelected()) {
        isModeFlying = true;
    } else {
        isModeFlying = false;
    }
}

/** updates all of the buttons 
then checks if there has been a button push and handles that change */
void buttonsUpdate(void) {
    updateButtons();

    if (isUpButtonPressed()) {
        heightTarget += ALT_STEP;
        if (heightTarget > 100) {
            heightTarget = 100;
        }
    }

    if(isDownButtonPressed()) {

        heightTarget -= ALT_STEP;
        if (heightTarget < 0) {
            heightTarget = 0;
        }
    }

    if (isRightButtonPressed()) {
        yawTarget += YAW_STEP;
        if (yawTarget >= 360) {
            yawTarget -= 360;
        }
    }

    if (isLeftButtonPressed()) {
            yawTarget -= YAW_STEP;
            if (yawTarget < 0) {
                yawTarget = 360 - YAW_STEP;
            }
    }
}

/** updates the FSM for the flight controller */
void
flightLogicControllerUpdateTick(void)
{
    switch (currentState) {
        case STARTUP_STATE:
            switchesUpdate();
            shutOffPwmRotors();
            currentPwmAlt = 0;
            currentPwmYaw = 0;
            if (!isModeFlying) {
                currentState = LANDED_STATE;
            }
            break;
        case LANDED_STATE:
            switchesUpdate();
            shutOffPwmRotors();
            currentPwmAlt = 0;
            currentPwmYaw = 0;
            if (isModeFlying == true) {
                if (isCalibrated) {
                    currentState = FLYING_STATE;
                    startUpPwmRotors();
                } else {
                    currentState = CALIBRATION_STATE;
                    startUpPwmRotors();
                }
            }
            break;
        case CALIBRATION_STATE:
            startUpPwmRotors();
            calibrating = true;
            findInitialPos();
            calibrating = false;
            isCalibrated = true;
            currentState = FLYING_STATE;
        case FLYING_STATE:
            buttonsUpdate();
            switchesUpdate();
            if (isModeFlying == false) {
                currentState = LANDING_STATE;
            }
            break;
        case LANDING_STATE:
            yawTarget = 0;
            if (abs(getShortestYawError(yawAngle, yawTarget)) < 3) {
                heightTarget = heightPercent - 6; // drop like stone (but a very unaerodynamic stone)
                if (heightTarget < 0) {
                    heightTarget = 0;
                }
            }

            if (heightPercent < 2) {
                currentState = LANDED_STATE;
            }
            break;
    }
}

/** changes the screen to the current screen received from getCurrentScreen() */
void 
displayUpdateTick(void)
{

    switch (getCurrentScreen()) {
        case 0:
            displayMainScreen(currentState, heightTarget, heightPercent, yawTarget, yawAngle);
            break;
        case 1:
            //displayMeanAdcScreen (heightRawAvg, getSampleCount());
            break;
        case 2:
            //displayBlankScreen (yawAngle);
            break;
    }
}

/** Background task: calculate the (approximate) mean of the values in the
circular buffer and display it, together with the sample number. */
void 
adcMeanSampleUpdateTick(void)
{
    heightRawAvg = readSampleAverageBuffer();
    heightPercent = (-(heightRawAvg - low_alt_ADC_limit) * 100) / (HIGH_ALT_ADC_OFFSET);
}

/** displays the information required onto the termal using UART */
void
uartUpdateTick(void)
{
    usprintf (UARTbuffer, "FLIGHT MODE: %d \r\n", currentState);
    UARTSend(UARTbuffer);

    usprintf (UARTbuffer, "ALTITUDE| current: %d target: %d \r\n", heightPercent, heightTarget);
    UARTSend(UARTbuffer);

    usprintf (UARTbuffer, "YAW     | current: %d target: %d \r\n", yawAngle, yawTarget);
    UARTSend(UARTbuffer);

    usprintf (UARTbuffer, "DUTY CYC| altitude: %d yaw: %d \r\n", currentPwmAlt, currentPwmYaw);
    UARTSend(UARTbuffer);
}

/** initializes all of the PID controllers */
void
initPIDControllers(void)
{
    // Calculate the slow system tick max ticks.
    slowSysTickMax = SYS_TICK_INTERRUPT_RATE_HZ / CONTROLLER_RATE_HZ;

    // Initiate the controller struts.
    controllerPIDInit(&altController, SYS_TICK_INTERRUPT_RATE_HZ / CONTROLLER_RATE_HZ, ALT_KP, ALT_KI, ALT_KD);
    controllerPIDInit(&yawController, SYS_TICK_INTERRUPT_RATE_HZ / CONTROLLER_RATE_HZ, YAW_KP, YAW_KI, YAW_KD);

    // Register the PID interrupt handler
    IntRegister(INT_EMAC0_TM4C129, PidIntHandler);
    IntPrioritySet(INT_EMAC0_TM4C129, 224);
    IntEnable(INT_EMAC0_TM4C129);
}


/**Initialisation functions for the clock (incl. SysTick), ADC, display */
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    SysTickPeriodSet(SysCtlClockGet() / SYS_TICK_INTERRUPT_RATE_HZ);
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    SysTickIntRegister(SysTickIntHandler);

    // Enable the interrupts
    SysTickIntEnable();
    SysTickEnable();
}



/** The main function initializes all attributes and runs the pacer function to display the required information  */
int
main(void)
{

	uint32_t displayTick = 0;
	uint32_t flightLogicControllerTick = 0;
	uint32_t adcMeanSampleTick = 0;
	uint32_t uartTick = 0;

	uint32_t displayMaxTicks = PACER_RATE_HZ / DISPLAY_UPDATE_RATE_HZ;
	uint32_t flightLogicControllerMaxTicks = PACER_RATE_HZ / FLIGHT_LOGIC_RATE_HZ;
	uint32_t adcMeanSampleMaxTicks = PACER_RATE_HZ / ADC_MEAN_SAMPLE_RATE_HZ;
	uint32_t uartMaxTicks = PACER_RATE_HZ / UART_RATE_HZ;

	uint32_t pacerDelay = SysCtlClockGet() / PACER_RATE_HZ;

	// Disable interrupts during initialization
	IntMasterDisable();

	initControls();
	initClock();
	initDisplay();
	initAdcController();
	initYawController();
	initPIDControllers();
	initPwm();
	initUart();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // ---- Run Calibration Procedure ----
    // Wait until the ADC sample average buffer has filled
    while(bufferIsNotFull()) {
        continue;
    }
    // Compute the low altitude ADC value
    low_alt_ADC_limit = readSampleAverageBuffer();


	while (1)
	{
	    SysCtlDelay(pacerDelay);

	    if (adcMeanSampleTick >= adcMeanSampleMaxTicks) {
            adcMeanSampleUpdateTick();
            adcMeanSampleTick = 0;
	    }

		if (displayTick >= displayMaxTicks) {
		    displayUpdateTick();
		    displayTick = 0;
		}

		if (flightLogicControllerTick >= flightLogicControllerMaxTicks) {
		    flightLogicControllerUpdateTick();
		    flightLogicControllerTick = 0;
		}

		if (uartTick >= uartMaxTicks) {
		    uartUpdateTick();
		    uartTick = 0;
		}

		flightLogicControllerTick++;
		displayTick++;
        adcMeanSampleTick++;
        uartTick++;

	}





}

