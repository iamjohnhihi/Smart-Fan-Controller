/*
 * PWM50.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
int measurePoint = 0;
int counter = 0;
int rpmCounter = 0;
int requestedRPM = 1000; //for testing purpose
float Duty = 0.5;
int interimValue = 0;
int main(void)
{
	
	//Set up pins
    DDRA |= (1 << DDA1) | (1 << DDA3); //set PA1 and PA3 as output (to drive the switches) 
	
	DDRA &= ~(1 << DDA7); //set PA2 as input (for hall-effect sensor) 
	//timer for PWM
	TCCR0B |= (1<< CS00); // clkI/O/(No prescaling)
	//timer for measuring rpm from hall sensor
	TCCR1B |= (1<< CS00);
	//start at zero
	TCNT1 = 0;
	//set timer1 to start counting
	TCCR1A = 0;

	
	// turn on Fast PWM Mode
	TCCR0A |= ((1<< WGM01) | (1<< WGM00));
	TCCR0B |= (1<< WGM02);

	// Set Values for Duty Cycle and Frequency
	uint8_t TOP = 250;
	
	OCR0A = TOP;
	OCR0B = TOP*Duty;

	TOCPMSA0 &= ~((1<< TOCC0S0) | (1<< TOCC0S1)); //routes the output of OC0B to TOCC0 (which corresponds PA1)

	//routes the output of OC0B to TOCC2 (which corresponds PA3)
	TOCPMSA0 &= ~((1<< TOCC2S0) | (1<< TOCC2S1));

	// Clear OC0B when Compare Match
	TCCR0A |= (1<< COM0B1);
	
    //Enable output to respective pins
	if (PINA == (1<<PORTA7)){
		TOCPMCOE |= (1<< TOCC0OE); //works when hall sensor = 1
	} else {
		TOCPMCOE |= (1<< TOCC2OE); //works when hall sensor = 0
	}

	
	
	//set up interrupt for PWM switching (Timer 0)
	cli();
	//enable pin change interrupt for PCINT7
	PCMSK0 |= (1<< PCINT7);
	//Any logical change causes interrupt
	MCUCR |= (1<< ISC00);
	//Enable pin change interrupt 0 
	GIMSK |= (1<< PCIE0);

	//set up interrupt for fan speed measuring (TIMER 1)
	TIMSK = (1 << TOIE1) ;
	
	//enable global interrupts
	sei();
	
    while (1) 
    {
		if (actualRPM > requestedRPM){
			duty -= 0.01; //value to be determined
		}
		else if (actualRPM < requestedRPM) {
			duty += 0.01; //value to be determined
		}
	}
}

int measureRPM() {
    //use clock frequency to get actual value of initialRPM
	
	int interimRPM += 60*8000000*(noOfOverflows + (rpmCounter/65536);
	if (interimValue == 4) {
		finalRPM = interimRPM
		interimRPM = 0;
		interimValue = 0;
	}
	return finalRPM;
}

ISR(TIMER1_OVF_vect) {
	noOfOverflows += 1;
}

ISR(PCINT0_vect) {
	//toggle output pins (PA3 and PA1) to output 1 PWM at a time
	TOCPMCOE ^= (1<<TOCC0OE) | (1<<TOCC2OE);
	//capture timer 1 value to measure rpm.
	rpmCounter = TCNT1;

	measureRPM();
    
	interimValue += 1;
	//reset timer 
	TCNT1 = 0;
	//intialise timer to start counting
	TCCR1A = 0;
	

}


