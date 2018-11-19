#include <reg51.h>
#include<stdio.h>

//Feel free to change methods declarations
unsigned char switch_training;	// 0: training, 1: testing
unsigned char switch_user;			// 0: User A, 1: User B
unsigned char trainingCount;		// Counts how many times did we get measurements from the user. Starts with 0.
unsigned char nextChar=0; // identifies which character we expect the user to enter (index of the character) starts with 0.
unsigned char word[3] = {'.','t','i'};	// Stores the characters of the word we want to use.
unsigned int TimerEntryIndex = 0;  //Initial Time at which we started Program
int StartCount = 2;  //Initial Time at which we started Program 2 means not in count mode 0-1 meaning we are waiting for input
int CorrectSofar = 0;
int StartTraining=0; // To Stop overflow and counter from increasing when we aren't putting input .. between 2 runs of entering a work
unsigned long TimerArray[2] = {0,0};
unsigned long FirstUserData[2] = {0,0}; // should change to 9
unsigned long SecondUserData[2] = {0,0}; // should change to 9

void CalculateAverage(unsigned int Values[])
{
	int i =0;
	for(;i<2;i++){ //Length of array should change to 10
		if(switch_user==0){
		FirstUserData[i]+=Values[i]; //Divide by the number of training should change to 5 
			Values[i] = 0; //Clear for next Count
		}
		else{
			SecondUserData[i]=Values[i];
		}
			
	}
	
	
}
void ClearTimerArray(){
	int i=0;
	for(;i<2;i++)
	  TimerArray[i]=0;

}
void runTraining(){}
// called when the user enters the whole word in the testing phase it should predict who was the user
void predict(){
 
}
 
 // TODO: you know what character did we actually read now (using the variable nextChar [it is an index to the character]) so you should measure the flight time and add it to training
// for the current user and same for the test time
void calculateTrainTime(){

	

 
}
 
void calculateTestTime(){
	
			if(StartCount==0 &&StartTraining==1){ 
			 unsigned long OverFlowCount = 0;

			TR0 = 1;            //Start the timer

			while(StartCount == 0  ){
				    while(TF0 == 0);   // Wait for Timer Overflow
				    OverFlowCount++;
            TF0 = 0;
			}				//wait till the key is released
			TR0 = 0;            //Stop the timer
			if(CorrectSofar == 1){
			TimerArray[TimerEntryIndex]=((TH0 << 8) | TL0)+0x0000FFFF*OverFlowCount; //Load timer into Array
				TimerEntryIndex++;

			}
			if(TimerEntryIndex ==2){ // IF Reached our maximum letter
					TimerEntryIndex = 0;
          //////// Call The Method that calculates the ecludien distance in here 					
				
				}
		  TH0 = 0;                //Reset the timer.
		  TL0 = 0;	
			TF0=0;
			StartCount=0;
			OverFlowCount=0;
		}	
 
}
void CalculateTime(){
	if( switch_training){
	  calculateTestTime();
	
	}

	if(StartCount==0 &&StartTraining==1){ 
			 unsigned long OverFlowCount = 0;

			TR0 = 1;            //Start the timer

			while(StartCount == 0  ){
				    while(TF0 == 0);   // Wait for Timer Overflow
				    OverFlowCount++;
            TF0 = 0;
			}				//wait till the key is released
			TR0 = 0;            //Stop the timer
			if(CorrectSofar == 1){
			TimerArray[TimerEntryIndex]=+((TH0 << 8) | TL0)+0x0000FFFF*OverFlowCount; //Load timer into Array
				TimerEntryIndex++;

			}
			if(TimerEntryIndex ==2){ // IF Reached our maximum letter
					TimerEntryIndex = 0;
					if(trainingCount==2) // if we finished Training for user A to be changed to 5
					{
						trainingCount = 0; 
						CalculateAverage(TimerArray);
						ClearTimerArray();
						if(!switch_user){ // If we r still in User A
						switch_user=1; //Go to b
						}
						else{
						   switch_training = 1; // If we finished B .. go to training
						}
					}						
				
				}
		  TH0 = 0;                //Reset the timer.
		  TL0 = 0;	
			TF0=0;
			StartCount=0;
			OverFlowCount=0;
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
	int idx;
	for(idx = 0; idx<3; idx++)
	{
		if(word[idx] == received){
			CorrectSofar = 0;
			break;
		}
	}
	// wrong character
	if(idx != nextChar)
	{
		nextChar = 0;
		StartCount=2; //Reset Count
    CorrectSofar = 0;
		TimerArray[0]=0;
    TimerArray[1]=0;

		//printf("%s","type the word again please");
		return;
	}
	// calculate the flight time between this character and the previous one
  CorrectSofar = 1;
		StartCount=1; 
		StartTraining =1 ; //Stop when we finished one work for one user

	if(!switch_training){
		
  
		//calculateTrainTime(); // Called in MainMethod
	} else {
		calculateTestTime();
	}
	
	nextChar++;
	if(nextChar==1){
	  StartCount=0;
	}
	// if we reach the last char 
	// increment the training count if it is training phase
	// call predict if we are in the testing phase
	if(nextChar == 3){ //Should Change to 10
		nextChar = 0;
		StartTraining =0 ; //Stop when we finished one work for one user
		if(!switch_training){
			trainingCount++;
			
		}
		else{
			predict();
			return;
		}	
	}
	//finalizing training after 5 inputs for the word
	if(trainingCount == 2){
		//runTraining();
		//trainingCount = 0;
		nextChar = 0;
		StartTraining = 0 ; //Stop when we finished a training for one user
		// switch to training phase of user B
		/*if(!switch_user &&!switch_training){         //Commented Cuz I already do this in my timer 
			//printf("%s","user B starts training");
			//switch_user = 1;
		} else {
		// both A and B did the training, switch to testing
			//printf("%s","Testing phase started");
			switch_training = 1;
		}*/ 
		
	}
	
}

// Option 2: interrupt-based
void receive() interrupt 4 {
	unsigned char received = SBUF;
 	 RI = 0;
	 StartCount =1;

	//printf("%s","Here");
	decide(received);
}

// method to read the ports and decide



void main() {
	

	
	uartConfig();
	
	while(1)
	{
    CalculateTime();
			
	}
	
}