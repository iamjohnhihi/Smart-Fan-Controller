/*
 * GccApplication1.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA |= (1 << DDA1) | (1<< DDA3); //set PA1 and PA3 as output (to drive the switches) 
	DDRA &= ~(1 << DDA2); //set PA2 as input (for hall-effect sensor) 

	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	
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
	TOCPMCOE |= (1<< TOCC0OE);

    while (1) 
    {
        //not sure if pwm will be regenerated //test1
		if (PINA2 & (1 << PORTA2)) { //read hall sensor signal, if high
			//turn on one of the switch
			PORTA &= ~(1 << PORTA1); //turn off PA1
			} else { //if hall sensor signal is low
			PORTA &= ~(1 << PORTA3); //turn off PA3
			}
    }
}

