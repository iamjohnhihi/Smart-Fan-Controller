/*
 * GccApplication1.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>


int main(void)
{
    uint8_t PA1Value;
    DDRA |= (1 << DDA1) | (1<< DDA2); //set PA1 and PA2 as output (to drive the switches) 
	DDRA &= ~(1 << DDA3); //set PA3 as input (for hall-effect sensor) 

	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	float Duty = 0.5;
	
	OCR0A = TOP;
	OCR0B = TOP*Duty;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1)); //what is this for?

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	TOCPMCOE |= (1<< TOCC0OE);


    while (1) 
    {
        PA1Value = (PINA1 & (1 << PA1)) ? 1 : 0; //to know whether PA1 is high or not
        if (value == 1){ //if PA1 is high
            PORTA &= ~(1 << PORTA2); //then PA2 is low
        } else { //if PA1 is low
            PORTA |= (1 << PORTA2); //then PA2 is high
        }
    }
}

