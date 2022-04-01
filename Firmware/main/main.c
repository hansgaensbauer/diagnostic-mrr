/* Main source for low cost malaria diagnosis NMR 
*
* This code uses the fast PWM mode of operation of the 
* 16 bit timer/counter 1 to generate the required pulse
* widths at the desired pulse repetition rate.
*
* This code should be run with FCLK = 20MHz, and CLKO enabled.
* PD4 - RF Switch Control
* PB2 - Modulation Control
* PB0 - 20MHz Out
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "nmr.h"

volatile int pulse_count; //count up to numpulses

int main(void) {

    //Connect coil to transmiter
    DDRD |= 0b00011000;
    PORTD = 0b00010000;

    pulse_count = 0; //reset pulse count

    TCCR1A = 0b00100011; //Fast PWM, TOP=OCR1A, non inverting for OC1B
    TCCR1B = 0b00011000; //Fast PWM, TOP=OCR1A, CS12:0 = CLK_IO/1.
    DDRB |= 0b00000110; //Set PB2/OC1B to output

    TIMSK1 |= 0b00000110; //Enable Compare A + B Match Interrupts
    sei(); //Enable global interrupts

    OCR1B = ((PULSEWIDTH_90) * (F_CPU / 1000000UL)); //90 degree pulse, 20 cycles/μs * 2 * PULSEWIDTH_90μs
    OCR1A = ((PULSETRAIN_DELAY) * (F_CPU / 1000000UL)); //20 cycles/μs * PULSE_SPACINGμs

    TCCR1B = 0b00011001; //Fast PWM, TOP=OCR1A, CS12:0 = CLK_IO/1. Turn on timer

    //Immediately switch to pulse spacing and 180 degree pulsewidth
    OCR1B = ((PULSEWIDTH_90 * 2) * (F_CPU / 1000000UL)); //180 degree pulse, 20 cycles/μs * 2 * PULSEWIDTH_90μs
    OCR1A = ((PULSE_SPACING) * (F_CPU / 1000000UL)); //20 cycles/μs * PULSE_SPACINGμs

    while (pulse_count < NUMPULSES); //Wait for the required number of pulses

    cli(); //turn off all interrupts
    TCCR1B = 0b00011000; //Turn off the timer

    while (1); //do nothing

}

//Timer 1 Compare B Match Interrupt
//Execute Every time we switch off the RF pulse
ISR(TIMER1_COMPB_vect) {
    pulse_count ++;
    PORTD = 0;
}

//Timer 1 Compare A Match Interrupt
//Execute Every time we switch on the RF coil
ISR(TIMER1_COMPA_vect) {
    PORTD = 0b00011000;
}