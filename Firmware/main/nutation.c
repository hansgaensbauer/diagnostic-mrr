/* Program for determining the 90 and 180 degree pulse lengths
*
* Sweeps through a sequence of 90-180-wait, with a trigger signal
* after the 180 degree wait signal.  
*
* This code uses the fast PWM mode of operation of the 
* 16 bit timer/counter 1 to generate the required pulse
* widths at the desired pulse repetition rate.
*
* This code should be run with FCLK = 20MHz, and CLKO enabled.
* PD4 - RF Switch Control
* PD0 - Trigger Output
* PB2 - Modulation Control
* PB0 - 20MHz Out
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "mrr.h"

//Pulsewidths to try, in μs
#define NUM_PW 10
uint8_t pulsewidths[NUM_PW] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

//This flag is set after the 180 degree pulse is set up so that the trigger signal
//doesn't fire during the 90 degree pulse
volatile uint8_t echo_ready_flag = 0;

//This flag is used to set up the trigger signal after we enter the echo period
volatile uint8_t trig_ready_flag = 0;

int main(void) {

    //Connect coil to transmiter
    DDRD |= 0b00010001;
    PORTD = 0b00010000;

    TCCR1A = 0b00100011; //Fast PWM, TOP=OCR1A, non inverting for OC1B
    TCCR1B = 0b00011000; //Fast PWM, TOP=OCR1A, CS12:0 = CLK_IO/1.
    DDRB |= 0b00000100; //Set PB2/OC1B to output

    TIMSK1 |= 0b00000110; //Enable Compare A + B Match Interrupts
    sei(); //Enable global interrupts

    //Set up the inter-pulse spacing
    OCR1A = ((PULSETRAIN_DELAY) * (F_CPU / 1000000UL)); //20 cycles/μs * PULSE_SPACINGμs
    

    for(uint8_t i = 0; i < NUM_PW; i++){
        
        trig_ready_flag = 0;
        echo_ready_flag = 0;

	TCCR1A = 0b00100011; 

	//Set up the 90 degree pulse
	echo_ready_flag = 0;
	uint8_t pw_90 = pulsewidths[i];
        OCR1B = ((pw_90) * (F_CPU / 1000000UL)); //90 degree pulse, 20 cycles/μs * 2 * pw_90μs
    
	TCCR1B = 0b00011001; //Fast PWM, TOP=OCR1A, CS12:0 = CLK_IO/1. Turn on timer
    
	//Immediately switch to pulse spacing and 180 degree pulsewidth
        OCR1B = ((pw_90 * 2) * (F_CPU / 1000000UL)); //180 degree pulse, 20 cycles/μs * 2 * pw_90μs

	_delay_ms(1000); //wait one second between repeated trials

	PORTD &= ~0b00000001; //clear the trigger signal
    }

    cli(); //turn off all interrupts

    while (1); //do nothing

}

//Timer 1 Compare B Match Interrupt
//Execute Every time we switch off the RF pulse
ISR(TIMER1_COMPB_vect) {
    PORTD = 0;
    if(echo_ready_flag){
	PORTB &= ~0b00000100; //make sure PB2 stays low
	TCCR1A = 0b00000011; //disconnect OC1B
	trig_ready_flag = 1;
	echo_ready_flag = 0;
    } else {
        echo_ready_flag = 1;
    }
}

//Timer 1 Compare A Match Interrupt
//Execute Every time we switch on the RF coil
ISR(TIMER1_COMPA_vect) {
    if(trig_ready_flag){
	PORTD |= 0b00000001;
        TCCR1B = 0b00011000; //Turn off the timer
	TCNT1 = 0; //Clear the timer
	trig_ready_flag = 0;
    } else{
        PORTD |= 0b00010000;
    }
}
