#include <reg51.h>

bit switch_training;	// 0: training, 1: testing
bit switch_user;			// 0: User A, 1: User B
unsigned char trainingCount;		// Counts how many times did we get measurements from the user. Starts with 0.
unsigned char nextChar=0; // identifies which character we expect the user to enter (index of the character) starts with 0.
unsigned char word[10] = {'.','t','i','e','5','R','o','n','a','l'};	// Stores the characters of the word we want to use.
unsigned char TimerEntryIndex = 0;  //Initial Time at which we started Program
char StartCount = 2;  //Initial Time at which we started Program 2 means not in count mode 0-1 meaning we are waiting for input
bit CorrectSofar = 0;
bit StartTraining=0; // To Stop overflow and counter from increasing when we aren't putting input .. between 2 runs of entering a work
unsigned long FirstUserData[9] = {0,0,0,0,0,0,0,0,0}, SecondUserData[9] = {0,0,0,0,0,0,0,0,0}; 
unsigned int OverFlowCount;
bit predict= 0;
unsigned char numberOfTrainings = 2;
unsigned char bound=0, c = 0; //To increase maximum timer delay time
sbit LED = P0^0;
unsigned long dA = 0, dB = 0;
void CalculateAverage()
{
	unsigned char i = 0;
	for(;i<9;i++){
		FirstUserData[i]/=2000; //Divide by the number of training should change to 5 
		SecondUserData[i]/=2000;	
	}
}
void ClearTimers(){
	if(switch_training){
		dA = 0;
		dB = 0;
	} else{
		unsigned char i = 0;
		for(;i<9;i++)
			if(!switch_user)
				FirstUserData[i]=0;
			else
	  			SecondUserData[i]=0;
	}
}

void flashUser(bit user) {
	bound = 5;
	if(!user)
		bound = 30;
	TMOD = 0x01;
	TH0 = 0x0;
	TL0 = 0x0;
	TR0 = 1;
	ET0 = 1;
	EA = 1;
	while(1){
	}
}
void determineUser() {
	if(dA < dB)
		flashUser(0);
	else
		flashUser(1);
}

void timer0_isr() interrupt 1{
	if(c == bound) {
		c = 0;
		LED = !LED;
	} else 
		c++;
	TH0 = 0x0;
	TL0 = 0x0;
}

void TimerMethod(){
			OverFlowCount = 0;
			TR0 = 1;            //Start the timer
			while(!StartCount){
				while(!TF0);   // Wait for Timer Overflow
				if(CorrectSofar)
				 	OverFlowCount++;
            	TF0 = 0;
			}				//wait till the key is released
			TR0 = 0;            //Stop the timer
			if(CorrectSofar){
				unsigned long flightTime = ((TH0 << 8) | TL0)+0x0000FFFF*OverFlowCount;
				if(switch_training){
					flightTime/=1000;
					dA += (flightTime - FirstUserData[TimerEntryIndex])*(flightTime - FirstUserData[TimerEntryIndex]);
					dB += (flightTime - SecondUserData[TimerEntryIndex])*(flightTime - SecondUserData[TimerEntryIndex]);	
				}else{
					if(!switch_user){
						FirstUserData[TimerEntryIndex]+=flightTime;
					} else{
						SecondUserData[TimerEntryIndex]+=flightTime;
					}
				}
				TimerEntryIndex++;
			} else{
				TimerEntryIndex=0;
            	ClearTimers();
			}
}
void clearEverything(){
	TH0 = 0;                //Reset the timer.
	TL0 = 0;	
	TF0=0;
	StartCount=0;
	OverFlowCount=0;
}
void calculateTestTime(){
	
	if(!StartCount && StartTraining){ 
		TimerMethod();
		if(TimerEntryIndex == 9){ // IF Reached our maximum letter
			TimerEntryIndex = 0;
			predict =1;
			return;
		}
		clearEverything();
	}	
 
}
void CalculateTime(){
	if(switch_training){
	  calculateTestTime();
		return;
	}
	if(!StartCount && StartTraining){ 
		TimerMethod();
		if(TimerEntryIndex == 9){ // IF Reached our maximum letter
				TimerEntryIndex = 0;
				if(trainingCount == numberOfTrainings) // if we finished Training for user A to be changed to 5
				{
					trainingCount = 0;
					if(!switch_user){ // If we r still in User A
						switch_user = 1; //Go to b
					} else {
						CalculateAverage();
						switch_training = 1; // If we finished B .. go to testing
					}
				}						
				
		}
		clearEverything();
	}	

}


void uartConfig(void) {
	
	SCON 	= 0x50;			// Serial Port: Mode 1 (8 bits), use Timer 1 for baudrate, enable reception (bit 4)
	IE 		= 0x90; 		// enable global interrupt and serial receive interrupt
	
	// Baudrate	= 2400 (bits/second ?) => 240 characters per second
	// Fosc			= 12 MHz
	
	// Baudrate	= Fosc / (N * (256 ? TH1))
	// For (SMOD1 = 0) N = 384 : TH1 = 242.979 => 243 = 0xF3	<- We'll use this value
	// For (SMOD1 = 1) N = 192 : TH1 = 229.958 => 230 = 0xE6
	
	PCON &= 0x7F;			// set SMOD1 in PCON to 0..
										// Tells that we are using N = 384.
										// What does it mean? I don't know xD
	
  	TMOD |= 0x21;			// timer 1, mode 2, 8-bit reload , timer 0 for counting
  	TH1 	= 0xF3;			// baud rate: reload value for 2400 baud @ 12MHz (to change?)
  	TR1 	= 1;				// start timer 1
	TL0 = 0x00;              //Initialise TIMER0 in 16 bit mode 
	TH0 = 0x00;
}


void decide(unsigned char received){
	// wrong character
	if(received != word[nextChar])
	{
		nextChar = 0;
		StartCount=2; //Reset Count
    	CorrectSofar = 0;
		trainingCount=0;
		return;
	}
	// calculate the flight time between this character and the previous one
  	CorrectSofar = 1;
	StartCount=1; 
	StartTraining =1 ; //Stop when we finished one work for one user	
	nextChar++;
	if(nextChar==1){
	  StartCount=0;
	}
	// if we reach the last char 
	// increment the training count if it is training phase
	// call predict if we are in the testing phase
	if(nextChar == 10){ //Should Change to 10
		nextChar = 0;
		StartTraining =0 ; //Stop when we finished one work for one user
		if(!switch_training){
			trainingCount++;
		} else {
			return;
		}	
	}
	//finalizing training after 5 inputs for the word
	if(trainingCount == numberOfTrainings){
		nextChar = 0;
		StartTraining = 0 ; //Stop when we finished a training for one user
	}
	
}


void receive() interrupt 4 {
	unsigned char received = SBUF;
 	RI = 0;
	StartCount =1;
	decide(received);
}




void main() {
	uartConfig();
	
	while(1)
	{
    	CalculateTime();
		if(predict){
			determineUser();
			break;
		}
			
	}
	
}
