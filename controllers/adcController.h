// *******************************************************
// 
// adcController.h
//
//  This contains the adc controller structs and the functions that are needed to operate them.
//
//  Authors Dan Brock Ronen, Jackson Allred, Pieter Leigh.
//  Last modified:   06.20.1969
// 
// *******************************************************

#ifndef ADCCONTROLLER_H_
#define ADCCONTROLLER_H_

/** returns if the buffer is full or not
@return the boolean comparing the buffer size to the sample count */ 
bool
bufferIsNotFull(void);

/** Gets the sample count from the static variable
@return returns a uint32_t containing the count of the sample */
uint32_t
getSampleCount(void);

/** sets the sample count with the given input
@param is a uint32_t of the sample count to be set */
void
setSampleCount(uint32_t newSampleCount);

/** Reads the sample from the buffer and takes an average
@return returns a type uint32_t which is the average buffer value */
void
initAdcController(void);

/** The handler for the ADC conversion complete interrupt. Writes to the circular buffer. */
void
ADCIntHandler(void);

/** Reads the sample from the buffer and takes an average
@return returns a type uint32_t which is the average buffer value */
uint32_t
readSampleAverageBuffer(void);


#endif /* ADCCONTROLLER_H_ */
