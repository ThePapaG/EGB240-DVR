/**
 * main.c - EGB240 Digital Voice Recorder Skeleton Code
 *
 * This code provides a skeleton implementation of a digital voice 
 * recorder using the Teensy microcontroller and QUT TensyBOBv2 
 * development boards. This skeleton code demonstrates usage of
 * the EGB240DVR library, which provides functions for recording
 * audio samples from the ADC, storing samples temporarily in a 
 * circular buffer, and reading/writing samples to/from flash
 * memory on an SD card (using the FAT file system and WAVE file
 * format. 
 *
 * This skeleton code provides a recording implementation which 
 * samples CH0 of the ADC at 8-bit, 15.625kHz. Samples are stored 
 * in flash memory on an SD card in the WAVE file format. The 
 * filename is set to "EGB240.WAV". The SD card must be formatted 
 * with the FAT file system. Recorded WAVE files are playable on 
 * a computer.
 * 
 * LED4 on the TeensyBOBv2 is configured to flash as an 
 * indicator that the programme is running; a 1 Hz, 50 % duty
 * cycle flash should be observed under normal operation.
 *
 * A serial USB interface is provided as a secondary control and
 * debugging interface. Errors will be printed to this interface.
 *
 * Version: v1.0
 *    Date: 10/04/2016
 *  Author: Mark Broadmeadow
 *  E-mail: mark.broadmeadow@qut.edu.au
 */  

 /************************************************************************/
 /* INCLUDED LIBRARIES/HEADER FILES                                      */
 /************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "serial.h"
#include "timer.h"
#include "wave.h"
#include "buffer.h"
#include "adc.h"

/************************************************************************/
/* ENUM DEFINITIONS                                                     */
/************************************************************************/
enum {
	DVR_STOPPED,
	DVR_RECORDING,
	DVR_PLAYING
};

/************************************************************************/
/* GLOBAL VARIABLES                                                     */
/************************************************************************/
uint16_t pageCount = 0;	// Page counter - used to terminate recording
uint16_t newPage = 0;	// Flag that indicates a new page is available for read/write
uint8_t stop = 0;		// Flag that indicates playback/recording is complete

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void pageFull();
void pageEmpty();

/************************************************************************/
/* INITIALISATION FUNCTIONS                                             */
/************************************************************************/

// Initialise PLL (required by USB serial interface, PWM)
void pll_init() {
	PLLFRQ = 0x6A; // PLL = 96 MHz, USB = 48 MHz, TIM4 = 64 MHz
}

// Configure system clock for 16 MHz
void clock_init() {
	CLKPR = 0x80;	// Prescaler change enable
	CLKPR = 0x00;	// Prescaler /1, 16 MHz
}

void pwm_init(){
	OCR1A = 62500;
	OCR1B = 31250;
	TCCR1A = 0b00100011;
	TCCR1B = 0b00011100;
	
	DDRB |= (1<<PINB6);
}

// Initialise DVR subsystems and enable interrupts
void init() {
	cli();			// Disable interrupts
	clock_init();	// Configure clocks
	pll_init();     // Configure PLL (used by Timer4 and USB serial)
	serial_init();	// Initialise USB serial interface (debug)
	timer_init();	// Initialise timer (used by FatFs library)
	buffer_init(pageFull, pageEmpty);  // Initialise circular buffer (must specify callback functions)
	adc_init();		// Initialise ADC
	pwm_init();
	sei();			// Enable interrupts
	
	// Must be called after interrupts are enabled
	wave_init();	// Initialise WAVE file interface
}

/************************************************************************/
/* CALLBACK FUNCTIONS FOR CIRCULAR BUFFER                               */
/************************************************************************/

// CALLED FROM BUFFER MODULE WHEN A PAGE IS FILLED WITH RECORDED SAMPLES
void pageFull() {
	if(!(--pageCount)) {
		// If all pages have been read
		adc_stop();		// Stop recording (disable new ADC conversions)
		stop = 1;		// Flag recording complete
	} else {
		newPage = 1;	// Flag new page is ready to write to SD card
	}
}

// CALLED FROM BUFFER MODULE WHEN A NEW PAGE HAS BEEN EMPTIED
void pageEmpty() {
	// TODO: Implement code to handle "page empty" callback 
}

/************************************************************************/
/* RECORD/PLAYBACK ROUTINES                                             */
/************************************************************************/

// Initiates a record cycle
void dvr_record() {
	buffer_reset();		// Reset buffer state
	
	pageCount = 305;	// Maximum record time of 10 sec
	newPage = 0;		// Clear new page flag
	
	wave_create();		// Create new wave file on the SD card
	adc_start();		// Begin sampling

	// TODO: Add code to handle LEDs
}

// TODO: Implement code to initiate playback and to stop recording/playback.

/************************************************************************/
/* MAIN LOOP (CODE ENTRY)                                               */
/************************************************************************/
int main(void) {
	uint8_t button_state;	// Start DVR in stopped state
	
	// Initialisation
	init();	
	
	// Loop forever (state machine)
    for(;;){
		button_state = ~(PINF|0b00001111);
		//n|94|76|51|2
		switch(button_state){
			case (1<<PINF4):
				//S1 pressed, 94%
				OCR1B = 58750;
				break;
			case (1<<PINF5):
				//S2 pressed, 76%
				OCR1B = 47500;
				break;
			case (1<<PINF6):
				//S3 pressed, 51%
				OCR1B = 31875;
				break;
		}
	}
}

