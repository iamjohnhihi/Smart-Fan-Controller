/*
 * PWM50.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define F_CPU 8000000UL
int rpmCounter = 0;
double requestedRPM = 1500;
double interimPeriod = 0;
float duty = 0.44;
int interimValue = 0;
uint32_t noOfOverflows = 0;
double actualRPM = 0;
double finalRPM;

int main(void)
{
	
	//Set up pins
    DDRA |= (1 << DDA1) | (1 << DDA3); //set PA1 and PA3 as output (to drive the switches) 
	
	DDRA &= ~(1 << DDA7); //set PA2 as input (for hall-effect sensor) 
	//timer for PWM
	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	//timer for measuring rpm from hall sensor
	TCCR1B |= (1<< CS10);
	//start at zero
	TCNT1 = 0;
	//set timer1 to start counting
	TCCR1A = 0;
	//TCCR1B = 0;


	
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	
	OCR0A = TOP;
	OCR0B = TOP*duty;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1)); //routes the output of OC0B to TOCC0 (which corresponds PA1)

	//routes the output of OC0B to TOCC2 (which corresponds PA3)
	TOCPMSA0 &= ~((1<< TOCC2S0) | (1<< TOCC2S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	//Enable output to respective pins

	if (PINA == (1<<PORTA7)){
		TOCPMCOE |= (1<< TOCC0OE); //works with 1
	} else {
		TOCPMCOE |= (1<< TOCC2OE); //works with 0
	}

	
	
	//set up interrupt for PWM switching (Timer 0)
	cli();
	//enable pin change interrupt for PCINT7
	PCMSK0 |= (1<< PCINT7);
	//Any logical change causes interrupt
	MCUCR |= (1<< ISC00);
	//Enable pin change interrupt 0 
	GIMSK |= (1<< PCIE0);
	//set up overflow interrupt for fan speed measuring (TIMER 1)
	TIMSK1 = (1 << TOIE1) ;
	//enable TOV flag for interrupt
	//TIFR1 |= (1<<TOV1);
	TCCR1A = 0;
	TCCR1B = 0;;

	TCNT1 = 65380;
	
	//enable global interrupts
	sei();

	
    while (1) 
    {
		//change duty cycle of fan to adjust to appropriate speed
		if ((actualRPM > requestedRPM)){
			if(duty>=0.432){
				duty -= 0.00001;
				OCR0B = TOP*duty;
			}
		}
		else if (actualRPM < requestedRPM) {
			if(duty<=1){
				duty += 0.00001;
				OCR0B = TOP*duty;
				}
		} 
	}
}

float measureRPM() {
	//use clock frequency to get actual value of initialRPM
	
	finalRPM = ((1/interimPeriod)*60);
	interimPeriod = 0;
	interimValue = 0;
	TCNT1 = 0;
	return finalRPM; //Actual RPM
}

/*int lockedRotor() {
	//clear register to disable PWM output
	TOCPMCOE &= ~(1<<TOCC0OE) | (1<<TOCC2OE);
}*/

ISR(TIMER1_OVF_vect) {
	//keep track of overflows
	noOfOverflows++;
}

ISR(PCINT0_vect) {
	//toggle output pins (PA3 and PA1) to output 1 PWM at a time
	TOCPMCOE ^= (1<<TOCC0OE) | (1<<TOCC2OE);
	
	interimValue++;
	interimPeriod += (1/(F_CPU))*(noOfOverflows + 65536/(TCNT1));
	
	if (interimValue == 4) {
	 actualRPM = measureRPM();
	}

	//reset TIMER1 
	TCNT1 = 0;
	//reset overflows
	noOfOverflows = 0;
	//intialise TIMER1 to start counting
	TCCR1A = 0;
}


