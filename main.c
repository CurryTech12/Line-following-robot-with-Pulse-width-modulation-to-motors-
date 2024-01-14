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
unsigned char c = 0;
void DelayMs(unsigned x){
	c = 0;
	while(c < x);
}
void OVERFLOW_ISR(void) interrupt 1{
	TCON &= ~(1<<4);
	TH0 = 0xFF;
	TL0 = 0xE7;
	TCON |= (1<<4);
	c++;
}
void main (void){
	InitDevice();
	IE = 0x82;
	TMOD = 0x01;
	TH0 = 0xFF;
	TL0 = 0xE7;//0.1ms
	TCON |= (1<<4);
	while(1){
		if(!(P0 & (1<<1))){//when sees black line
			P0 &= ~(1<<2);//sends 0v to gate of mosfet
		}
		if(!(P0 & (1<<3))){//sees black line
			P0 &= ~(1<<4);//sends 0v to gate of mosfet
		}
		if(P0 & (1<<3)){//sees white
			P0 |= (1<<4);
			DelayMs(2);//0.2ms delay
			P0 &= ~(1<<4);//clear low
			DelayMs(8);//same as below just on port P0.4 square wave that is going onto gate of mosfet
		}
		if(P0 & (1<<1)){//when sees white
			P0 |= (1<<2);//set high
			DelayMs(2);//high for 0.2ms
			P0 &= ~(1<<2);//clear low
			DelayMs(8);//total delay together is 1ms low for 0.8ms combined period of 1ms 1/1ms gives a 1khz square wave 
			//1khz square wave makes the motor turn smooth at a 20% duty cycle as 0.2ms is 20% of 1ms period
		}
	}
}
