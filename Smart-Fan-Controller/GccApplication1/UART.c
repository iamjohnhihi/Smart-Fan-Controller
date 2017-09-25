#define F_CPU 8000000UL
#define BAUD 9600


void Enable_RxMode (){
	UCSR0B |= (1 << RXEN0);
}

void Disable_RxMode (){
	UCSR0B &= ~(1 << RXEN0);
}

void Enable_TxMode (){
	UCSR0B |= (1 << TXEN0);
}

void Disable_TxMode (){
	UCSR0B &= ~(1 << TXEN0);
}
// UART//
//----------------------------------------------------------------------------------------------//
void UART_Initialisation() {
	unsigned int baud = (F_CPU/16/BAUD) - 1;
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); //set 8-bit character size using register UCSROC
	UBRR0H = (unsigned char)(baud>>8);	//Set baud rate using register UBRR0H - 4 MSB of USART baud rate
	UBRR0L = (unsigned char)(baud);		//Set baud rate using register UBRR0L - 8 LSB of USART baud rate
	UCSR0B |= (1<<TXEN0);				//Enable transmitter
	REMAP |= (1 << U0MAP);


}

void UART_Transmit(uint8_t data) {
	
	while (!((1<<UDRE0) && UCSR0A));
	UDR0 = data;						//Enter data into buffer, send the data

}

int usart_putchar_printf(char var, FILE *stream){
	if (var == '\n') UART_Transmit('\r');
	UART_Transmit(var);
	return 0;
}

void UART_BufferEmptyInterruptEnabler(){
	//enable interrupt on the UDRE1 flag: UDREn is set when transmit buffer UDRn is empty (ready to receive data)
	UCSR0B |= (1 << UDRIE0);

}


