//main.c
#include <SI_EFM8BB1_Register_Enums.h>

void InitDevice(void)
{
	
//Disable Watchdog Timer with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__OPEN_DRAIN | P1MDOUT_B1__OPEN_DRAIN
| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN
| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__OPEN_DRAIN
| P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;
//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
//Disable Interrupts
IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__DISABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;
}
unsigned char c = 0;//global variable c
void DelayMs(unsigned x){//this gets called  based of what delay we want to make
	c = 0;//start at zero
	while(c < x);//loops through however many times based on what gets returned through the DelayMs function
}
void OVERFLOW_ISR(void) interrupt 1{//isr gets called when the timer overflow resesting the 16bit timer for another run an incrememnt a global variable c
	TCON &= ~(1<<4);//stop timer
	TH0 = 0xFF;//reloads timer bits 
	TL0 = 0xE7;
	TCON |= (1<<4);//start timer again
	c++;// for example we have DelayMs(6) this run the timer 6 times to create a 0.6mS delay instead of just a 0.1mS delay timer
}
void main (void){
	InitDevice();
	IE = 0x82;//enable interrupt
	TMOD = 0x01;// put timer in 16bit mode
	TH0 = 0xFF;//load timer bits to create a 0.1ms timer
	TL0 = 0xE7;//0.1ms
	TCON |= (1<<4);//start timer
	while(1){
		if(!(P0 & (1<<1))){//when sees white 
			P0 |= (1<<2);//set high
			DelayMs(6);//high for 0.26s
			P0 &= ~(1<<2);//clear low
			DelayMs(4);//total delay together is 1ms low for 0.4ms combined period of 1ms 1/1ms gives a 1khz square wave 
			//1khz square wave makes the motor turn smooth at a 60% duty cycle as 0.6ms is 60% of 1ms period
		}
		if(!(P0 & (1<<3))){//sees white line
			P0 |= (1<<4);
			DelayMs(6);//0.6ms delay
			P0 &= ~(1<<4);//clear low
			DelayMs(4);//same as below just on port P0.4 square wave that is going onto gate of mosfet
			
		}
		if(P0 & (1<<3)){//sees black
			P0 &= ~(1<<4);//sends 0v to gate of mosfet
		}
		if(P0 & (1<<1)){//when sees black
			P0 &= ~(1<<2);//turns motor off based of ir sensor to the gate of the mosfet
		}
	}
}
