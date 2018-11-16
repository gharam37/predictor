#include <reg51.h>
#include<stdio.h>

//Feel free to change methods declarations

unsigned char switch_training;	// 0: training, 1: testing
unsigned char switch_user;			// 0: User A, 1: User B
unsigned char trainingCount;		// Counts how many times did we get measurements from the user. Starts with 0.
unsigned char nextChar; // identifies which character we expect the user to enter (index of the character) starts with 0.
unsigned char word[10] = {'.','t','i','e','5','R','o','n','a','l'};	// Stores the characters of the word we want to use.

void decide(unsigned char received);
	
// Configuring UART,
// using Mode 1 and Timer 1 Mode 2 for baudrate (2400)

void uartConfig(void) {
	
	SCON 	= 0x50;			// Serial Port: Mode 1 (8 bits), use Timer 1 for baudrate, enable reception (bit 4)
	IE 		|= 0x90; 		// enable global interrupt and serial receive interrupt
	
	// Baudrate	= 2400 (bits/second ?) => 240 characters per second
	// Fosc			= 12 MHz
	
	// Baudrate	= Fosc / (N * (256 - TH1))
	// For (SMOD1 = 0) N = 384 : TH1 = 242.979 => 243 = 0xF3
	// For (SMOD1 = 1) N = 192 : TH1 = 229.958 => 230 = 0xE6	<- We'll use this value
	
	PCON |= 0x80;			// set SMOD1 in PCON to 1..
										// Tells that we are using N = 192.
										// What does it mean? I don't know xD
	
  TMOD |= 0x20;			// timer 1, mode 2, 8-bit reload
  TH1 	= 0xE6;			// baud rate: reload value for 2400 baud @ 12MHz (to change?)
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
	decide(received);
}

void runTraining() {
// TODO: finalize the training (AKA: calculating the average and so for the current user)
}

// called when the user enters the whole word in the testing phase it should predict who was the user
void predict(){
 
}
 
 // TODO: you know what character did we actually read now (using the variable nextChar [it is an index to the character]) so you should measure the flight time and add it to training
// for the current user and same for the test time
void calculateTrainTime(){
 
}
 
void calculateTestTime(){
 
}

// method to read the ports and decide
void decide(unsigned char received){
	int idx;
	for(idx = 0; idx<10; idx++)
	{
		if(word[idx] == received)
			break;
	}
	// wrong character
	if(idx != nextChar)
	{
		nextChar = 0;
		printf("%s","type the word again please");
		return;
	}
	// calculate the flight time between this character and the previous one
	if(switch_training){
		calculateTrainTime();
	} else {
		calculateTestTime();
	}
	nextChar++;
	// if we reach the last char 
	// increment the training count if it is training phase
	// call predict if we are in the testing phase
	if(nextChar == 10){
		nextChar = 0;
		if(switch_training)
			trainingCount++;
		else{
			predict();
			return;
		}	
	}
	//finalizing training after 5 inputs for the word
	if(trainingCount == 5){
		runTraining();
		trainingCount = 0;
		nextChar = 0;
		// switch to training phase of user B
		if(!switch_user){
			printf("%s","user B starts training");
			switch_user = 1;
		} else {
		// both A and B did the training, switch to testing
			printf("%s","Testing phase started");
			switch_training = 1;
		}
		
	}
	
}


void main() {
	
	uartConfig();
	
	while(1);
	
}