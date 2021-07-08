// *******************************************************
// 
// adcController.c
//
//  This contains the adc controller structs and the functions that are needed to operate them.
//
//  Authors Jackson Allred, Pieter Leigh, Dan Brock Ronen.
//  Last modified:   06.20.1969
// 
// *******************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"

#include "../circBufT.h"


#define BUF_SIZE 10 // the size of the write buffer for the adc to write to and average over

volatile static uint32_t g_ulSampleCount; //The count of the sample
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

/** returns if the buffer is full or not
@return the boolean comparing the buffer size to the sample count */
bool 
bufferIsNotFull(void)
{
    return g_ulSampleCount <= BUF_SIZE;
}

/** Gets the sample count from the static variable
@return returns a uint32_t containing the count of the sample */
uint32_t
getSampleCount(void)
{
    return g_ulSampleCount;
}

/** sets the sample count with the given input
@param is a uint32_t of the sample count to be set */
void
setSampleCount(uint32_t newSampleCount)
{
    g_ulSampleCount = newSampleCount;
}

/** Reads the sample from the buffer and takes an average
@return returns a type uint32_t which is the average buffer value */
uint32_t
readSampleAverageBuffer(void) {

    uint16_t i;
    uint32_t sum = 0;
    uint32_t returnValue = 0;


    ADCIntDisable(ADC0_BASE, 3);

    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);
    // Calculate and display the rounded mean of the buffer contents
    returnValue = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    ADCIntEnable(ADC0_BASE, 3);

    return returnValue;
}


/** The handler for the ADC conversion complete interrupt. Writes to the circular buffer. */
void
ADCIntHandler(void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    g_ulSampleCount++;
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

/** Initialises the functions for the ADC controller. */
void
initAdcController (void)
{
    initCircBuf(&g_inBuffer, BUF_SIZE);

    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


