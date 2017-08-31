/*
 * GccApplication1.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA |= (1 << DDA1) | (1<< DDA2); //set PA1 and PA2 as output (to drive the switches) 
	DDRA &= ~(1 << DDA3); //set PA3 as input (for hall-effect sensor) 

	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)

	TCCR1B |= (1<< CS00); // clkI/O/(No prescaling)
	
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	TCCR1A |= ((1<< WGM11) | (1<< WGM10));
	TCCR1B |= ((1<< WGM12) | (1<< WGM13));


	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	float Duty = 0.5;
	
	OCR0A = TOP;
	OCR0B = TOP*Duty;

	OCR1A = TOP;
	OCR1B = TOP*Duty;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1)); //what is this for?

	TOCPMSA0 &= ~((1<< TOCC1S0) | (1<< TOCC1S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	TOCPMCOE |= (1<< TOCC0OE);

	TCCR1A |= (1<< COM1B1); 
	TOCPMCOE |= (1<< TOCC1OE);

    while (1) 
    {
		
    }
}

