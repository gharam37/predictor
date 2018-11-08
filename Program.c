#include <reg51.h>

unsigned char switch_training;	// 0: training, 1: testing
unsigned char switch_user;			// 0: User A, 1: User B
unsigned char trainingCount;		// Counts how many times did we get measurements from the user. Starts with 0.
unsigned char word[10] = {'.','t','i','e','5','R','o','n','a','l'};	// Stores the characters of the word we want to use.

// Configuring UART,
// using Mode 1 and Timer 1 Mode 2 for baudrate (2400)

void uartConfig(void) {
	
	SCON 	= 0x50;			// Serial Port: Mode 1 (8 bits), use Timer 1 for baudrate, enable reception (bit 4)
	IE 		= 0x90; 		// enable global interrupt and serial receive interrupt
	
	// Baudrate	= 2400 (bits/second ?) => 240 characters per second
	// Fosc			= 12 MHz
	
	// Baudrate	= Fosc / (N * (256 – TH1))
	// For (SMOD1 = 0) N = 384 : TH1 = 242.979 => 243 = 0xF3	<- We'll use this value
	// For (SMOD1 = 1) N = 192 : TH1 = 229.958 => 230 = 0xE6
	
	PCON &= 0x7F;			// set SMOD1 in PCON to 0..
										// Tells that we are using N = 384.
										// What does it mean? I don't know xD
	
  TMOD |= 0x20;			// timer 1, mode 2, 8-bit reload
  TH1 	= 0xF3;			// baud rate: reload value for 2400 baud @ 12MHz (to change?)
  TR1 	= 1;				// start timer 1
}

// Reading the characters
// Option 1: polling the 'RI' interrupt flag

/*unsigned char readChar(void) {
	unsigned char character;
	while(!RI);									// Wait until character is received, and interrupt is set
	character = SBUF;						// Read the character from buffer reg
	RI = 0;											// Clear the flag
	return character;						// Return the received character
}*/


// Option 2: interrupt-based
void receive() interrupt 4 {
	unsigned char received = SBUF;
  RI = 0;
	
	// TODO: Do something :)
	
	/*int idx;
	for(idx = 0; idx<10; idx++)
	{
		if(word[idx]==receive)
			break;
	}
	
	if(switch1){
		calculateTrainTime(idx);
	} else {
		calculateTestTime(idx);
	}*/
	
}

/*void runTraining(unsigned char count) {
// TODO: read characters and measure flight time. Do that 10 times. (count will be equal to 10).
}*/

/*void predict(){
 
}*/
 
/*void calculateTrainTime(int charIdx){
 
}*/
 
/*void calculateTestTime(int charIdx){
 
}*/

void main() {
	
	uartConfig();
	
	while(1);
	
}