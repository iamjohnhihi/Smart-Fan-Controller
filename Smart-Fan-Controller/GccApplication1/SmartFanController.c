/*
 * PWM50.c
 *
 * Created: 30/08/2017 2:55:53 p.m.
 * Author : blee490
 */
#define F_CPU 8000000UL
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "UART.c"
#include "PwmGenerator.c"

int requestedRPM = 1500;
int periodCounter = 0;
uint16_t noOfOverflows = 0;
uint16_t actualRPM = 0;

int16_t error = 0;
int16_t prevError = 0;
float intSumError;
char data = 0;
float sumIntError;
float diffError;
double interimPeriod;
uint32_t printCounter = 0;

void cutPower() {
	TOCPMCOE &= ~(1 << TOCC0OE);
	TOCPMCOE &= ~(1 << TOCC2OE);
}

void PIDspeedControl() {
	//time step is the clock period
	float dt = 1/interimPeriod;
	//proportional gain constant
	float kp = 0.0085;
	//integral gain constant
	float ki = 0.0001;
	//differential gain constant
	float kd = 0.0;

	//establish previous error for integral calculation
	//current error value
	error = requestedRPM - actualRPM;
	if (error > 100) {
		error = 100;
	}
	else if (error < -100) {
		error = -100;
	}
	//integral of error
	sumIntError += (error)*dt;
	//prevent integral windup
// 	if (sumIntError > 255) {
// 		sumIntError = 255;
// 	}
// 	else if (sumIntError < 0) {
// 		sumIntError = 0;
// 	}

	//differential of error
	diffError = (error - prevError)/dt;
	//change output duty cycle based off PID control values
	duty = kp*error + ki*sumIntError + kd*diffError;
	OCR0B = duty*TOP;
	prevError = error;
}

void FanDirectionCorrection(){
	//Enable output to respective pins
	if (PINA & (1<<PORTA2)){
		TOCPMCOE |= (1<< TOCC0OE); //works with 1
	} else {
		TOCPMCOE |= (1<< TOCC2OE); //works with 0
	}
}

void lockedRotorDetection(){
	// if number of overflows is higher than what we expected, it means the rotor is locked
	if (noOfOverflows > 100) {
		cutPower();
	}
}

void Restart() {
	if (noOfOverflows > 600){ //If the rotor was detected as locked before
		noOfOverflows = 0;
		FanDirectionCorrection();
		duty = 0.432;
		OCR0B = TOP*duty;
		}
}

void speedControl () {
	//change duty cycle of fan to adjust to appropriate speed

	//rough adjustment of fan speed
	if (actualRPM > requestedRPM*1.3) {
		if (duty >= 0.432){
			duty -= 0.001;
			OCR0B = TOP*duty;
		}

	} else if (actualRPM < requestedRPM*0.7){
		if (duty <= 1){
			duty += 0.001;
			OCR0B = TOP*duty;
		}

	//fine adjustment of fan speed
	} else if (actualRPM > requestedRPM*1.03){
		if (duty >= 0.432){
			duty -= 0.00001;
			OCR0B = TOP*duty;
		}
	} else if (actualRPM < requestedRPM*0.97) {
		if (duty <= 1){
			duty += 0.00001;
			OCR0B = TOP*duty;
		}
	}
}

void SpeedController_Initialization(){
	//TIMER1 counting starts at zero
	TCNT1 = 0;
	//set timer1 to start counting
	TCCR1A = 0;
	TCCR1B = 0;
}

void TIMER0_LogicalChangeInterruptEnabler(){
	/************************************************************************/
	/* used for PWM Generator                                               */
	/************************************************************************/

	//disable all pin change interrupts
	PCMSK1 = 0;
	PCMSK0 = 0;

	//enable pin change interrupt for PCINT2
	PCMSK0 |= (1<< PCINT2);
	//Any logical change causes interrupt
	MCUCR |= (1<< ISC00);
	//Enable pin change interrupt 0
	GIMSK |= (1<< PCIE0);
}

void TIMER1_OverflowInterruptEnabler(){
	/************************************************************************/
	/* used for Speed Controller                                            */
	/************************************************************************/

	//set up overflow interrupt for fan speed measuring (TIMER 1)
	TIMSK1 = (1 << TOIE1) ;
	TCCR1A = 0;

	//timer for measuring rpm from hall sensor
	TCCR1B |= (1<< CS10);
}

uint16_t measureRPM() {
	//use clock frequency to get actual value of initialRPM
	interimPeriod = (1.0/F_CPU)*((65536.0*noOfOverflows)+TCNT1);
	uint16_t finalRPM = ((1.0/interimPeriod)*60.0);
	periodCounter = 0;
	TCNT1 = 0;	//reset TIMER1 counting value
	noOfOverflows = 0;	//reset overflows
	return finalRPM;
}

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);
int main(void)
{
	UART_Initialisation();
	stdout = &mystdout;
	PwmGenerator_Initialization();
	SpeedController_Initialization();
	FanDirectionCorrection();

	cli();	//disable global interrupts
	UART_BufferEmptyInterruptEnabler();
	TIMER0_LogicalChangeInterruptEnabler();
	TIMER1_OverflowInterruptEnabler();
	sei();	//enable global interrupts


	Enable_TxMode();
    while (1)
    {
		printf("%d\n", actualRPM);
		//create delay for printing/transmitting to extract values for testing
 		Restart();
 		//speedControl();

	}
}



/************************************************************************/
/*                          Interrupts Sections                                          */
/************************************************************************/
ISR(TIMER1_OVF_vect) {
	//keep track of overflows
	if (noOfOverflows < 66000){
		noOfOverflows++;
	}
	TCNT1 = 0; //reset TIMER1 value

	lockedRotorDetection();
}

ISR(PCINT0_vect) {
	//toggle output pins (PA3 and PA1) to output 1 PWM at a time
	TOCPMCOE ^= (1<<TOCC0OE) | (1<<TOCC2OE);

	periodCounter++;

	if (periodCounter == 4) {
	 actualRPM = measureRPM();
	 PIDspeedControl();
	}
}

	//UDREn is set when transmit buffer UDRn is empty (ready to receive data)
ISR(USART0_UDRE_vect){
	/************************************************************************/
	/* Data Transmittion Testing                                                                     */
	/************************************************************************/
	/*_delay_us(30);*/
	_delay_us(100);
   //UDR0 = 0x4b;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
}
