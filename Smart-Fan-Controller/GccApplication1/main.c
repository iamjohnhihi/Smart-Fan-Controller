/*
 * PWM50.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
	//Set up pins
    DDRA |= (1 << DDA1) | (1 << DDA3); //set PA1 and PA3 as output (to drive the switches) 
	
	DDRA &= ~(1 << DDA7); //set PA2 as input (for hall-effect sensor) 
	
	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	TCCR2B |= (1<< CS00);
	
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	float Duty = 0.5;
	
	OCR0A = TOP;
	OCR0B = TOP*Duty;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1)); //routes the output of OC0B to TOCC0 (which corresponds PA1)

	//routes the output of OC0B to TOCC2 (which corresponds PA3)
	TOCPMSA0 &= ~((1<< TOCC2S0) | (1<< TOCC2S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	//Enable output to respective pins
	TOCPMCOE |= (1<< TOCC0OE);
	
	//set up interrupt
	cli();
	//enable interrupt for PCINT7
	PCMSK0 |= (1<< PCINT7);
	//Any logical change causes interrupt
	MCUCR |= (1<< ISC00);
	//Enable pin change interrupt 0 
	GIMSK |= (1<< PCIE0);
	sei();

	
    while (1) 
    {

    }
}

ISR(PCINT0_vect) {
	//toggle output pins (PA3 and PA1) to output 1 PWM at a time
	TOCPMCOE ^= (1<<TOCC0OE) | (1<<TOCC2OE);

}

