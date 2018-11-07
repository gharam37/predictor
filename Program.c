#include <reg51.h>

unsigned char switch_training;	// 0: training, 1: testing
unsigned char switch_user;			// 0: User A, 1: User B
unsigned char trainingCount;		// Counts how many times did we get measurements from the user. Starts with 0.
unsigned char word[10] = {'.','t','i','e','5','R','o','n','a','l'};	// Stores the characters of the word we want to use.

void uartConfig(void) {
	SCON 	= 0x50;			// serial Port Mode 1 (8 bits), and enable reception (bit 4)
  IE 		= 0x90;			// enable receiver interrupt
  TMOD |= 0x20;			// timer 1, mode 2, 8-bit reload
  TH1 	= 0xfd;			// baud rate: reload value for 2400 baud @ 12MHz (to change?)
  TR1 	= 1;				// start timer 1
}

/*unsigned char readChar(void) {
	unsigned char character;
	while(!RI);									// Wait until character is received, and interrupt is set
	character = SBUF;						// Read the character from buffer reg
	RI = 0;											// Clear the flag
	return character;						// Return the received character
}*/

/*void runTraining(unsigned char count) {
// TODO: read characters and measure flight time. Do that 10 times. (count will be equal to 10).
}*/

int main() {
	uartConfig();
	return 0;
}

/* void receive() interrupt 4 {
    char received = SBUF;
    RI = 0;
    int idx;
    for(idx = 0; idx<10; idx++)
    {
        if(word[idx]==receive)
            break;
    }
    if(switch1){
        calculateTrainTime(idx);
    } else {
        calculateTestTime(idx);
    }
 
 
}
 
void predict(){
 
}
 
void calculateTrainTime(int charIdx){
 
}
 
void calculateTestTime(int charIdx){
 
} */