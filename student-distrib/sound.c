#include "sound.h"


 //Play sound using built in speaker
 void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0xb6, 0x43);
 	outb((uint8_t) (Div),0x42 );
 	outb((uint8_t) (Div >> 8), 0x42);
 	enable_irq(PIT_IRQ);

    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb( tmp | 3,0x61);
 	}
 }
 
 //make it shutup
 void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 
 	outb(tmp, 0x61);
 }
 
 //Make a beep
 void beep(uint32_t freq,int time,int wait) {
 	 play_sound(freq);
	 int count = 0;
	 char buf[2];
	 while( count<=time){
		 rtc_read(-1,buf, 1 );
		 count++;
	 }
 	 nosound();
	 count = 0;
	 while( count<=wait){
		 rtc_read(-1,buf, 1 );
		 count++;
	 }
}

void ateam(int time) {
	if(time==0)	{	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==1)	{	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T0_16,T1_16);	}
	if(time==2)	{	beep(Cs5,T1_16,T0_16);
					beep(Cs5,Th_16,Th_16);	}
	if(time==3)	{	beep(Fs5,T1_16,T0_16);	
					beep(Fs5,T1_16,T0_16);	}
	if(time==4)	{	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==5)	{	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==6)	{	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==7)	{	beep(Fs5,T0_16,T1_16);
					beep(Fs5,T0_16,T1_16);	}
	
	if(time==8)	{	beep(B4,T1_16,T0_16);
					beep(B4,T0_16,T1_16);	}
	if(time==9)	{	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==10){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T0_16,T1_16);	}
	if(time==11){	beep(Fs4,T1_16,T0_16);	
					beep(Fs4,T1_16,T0_16);	}
	if(time==12){	beep(Fs4,T1_16,T0_16);
					beep(Fs4,T1_16,T0_16);	}
	if(time==13){	beep(Fs4,T1_16,T0_16);
					beep(Fs4,T1_16,T0_16);	}
	if(time==14){	beep(Fs4,T0_16,T1_16);
					beep(Fs4,T0_16,T1_16);	}
	if(time==15){	beep(As4,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	
	if(time==16){	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==17){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==18){	beep(Gs5,T1_16,T0_16);
					beep(Gs5,T1_16,T0_16);	}
	if(time==19){	beep(Fs5,T1_16,T0_16);	
					beep(Fs5,T1_16,T0_16);	}
	if(time==20){	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==21){	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==22){	beep(Fs5,T1_16,T0_16);
					beep(Fs5,T1_16,T0_16);	}
	if(time==23){	beep(Fs5,T0_16,T1_16);
					beep(Fs5,T0_16,T1_16);	}
	
	if(time==24){	beep(E5,T1_16,T0_16);
					beep(E5,T1_16,T0_16);	}
	if(time==25){	beep(E5,T1_16,T0_16);
					beep(E5,T0_16,T1_16);	}
	if(time==26){	beep(Ds5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==27){	beep(B4,T1_16,T0_16);	
					beep(B4,T1_16,T0_16);	}
	if(time==28){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==29){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==30){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}
	if(time==31){	beep(Cs5,T1_16,T0_16);
					beep(Cs5,T1_16,T0_16);	}	
}



