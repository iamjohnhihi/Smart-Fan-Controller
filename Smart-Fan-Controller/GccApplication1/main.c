/*
 * GccApplication1.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA |= (1<< DDA1);

	TCCR0B |= (1<< CS00);

	// Setting Port A to output
	//PORTA |= (1<<PORTA1);
	

	// Fast PWM Mode
	TCCR0A |=  ((1<< WGM01) | (1<< WGM00));
	
	TCCR0B |= (1<< WGM02);


	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	uint8_t dutySet = 125;
	
	OCR0A = TOP;
	OCR0B = dutySet;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	
	TOCPMCOE |= (1<< TOCC0OE);
	
    while (1) 
    {
		
    }
}

