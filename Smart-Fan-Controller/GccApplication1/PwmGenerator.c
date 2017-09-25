#include <avr/io.h>
uint8_t TOP = 250;
double duty = 0.432;

void PwmGenerator_Initialization(){
	//Set up pins
	DDRA |= (1 << DDA1) | (1 << DDA3); //set PA1 and PA3 as output (to drive the switches)

	DDRA &= ~(1 << DDA2); //set PA2 as input (for hall-effect sensor)
	//timer for PWM
	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	// Set Values for Duty Cycle and Frequency
	OCR0A = TOP;
	OCR0B = TOP*duty;
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	//routes the output of OC0B to TOCC0 (which corresponds PA1)
	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1));

	//routes the output of OC0B to TOCC2 (which corresponds PA3)
	TOCPMSA0 &= ~((1<< TOCC2S0) | (1<< TOCC2S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
}
