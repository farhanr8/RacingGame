// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: Alex Ma, Farhan Rahman
// Last modification date: 4/8
// Last Modified: 4/5/2016 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>
#include "Lab8.h"
#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(void);
void SysTick_Handler(void);

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
uint32_t Spawn1x = 50;
uint32_t Spawn1y = 75;
uint32_t Spawn2x = 50;
uint32_t Spawn2y = 115;
uint32_t CarY = 100;
uint32_t Speed = 40000000;
uint32_t loop = 1;
uint32_t Sample = 0;
uint32_t mainFlag = 1;
uint32_t FinishFlag = 0;
int repeat =0;
int CowFlag,DeerFlag,PattFlag = 0;
int CowSpawnx, CowSpawny,DeerSpawnx,DeerSpawny,PattSpawnx,PattSpawny;
int track1 =1, track2 =0;
int up,right,left; 
int ADCFlag = 0;
int CollisionFlag,Invincible = 0;
int Fx = 45,Fy=115,Fh=65;
uint32_t Data;        // 12-bit ADC
uint32_t Distance;


void 	Port_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x32; 	//Activate Ports B, E and F
	while((SYSCTL_RCGCGPIO_R&0x32)==0){};
	
		//Switches PB0-2
	GPIO_PORTB_DIR_R &=~ 0x07;
	GPIO_PORTB_DEN_R |= 0x07;
	GPIO_PORTB_AFSEL_R &=~ 0x07;
		//Hearbeat
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	
	ADC_Init();
		
	//DAC_Init();
}

void SysTick_Init(void){
	// enable SysTick with core clock and interrupts every 20Hz
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = Speed - 1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 
}

void Countdown(void){
	ST7735_SetRotation(3);
	GPIO_PORTF_DATA_R |= 0x02;
	ST7735_DrawCharS(10,8, '3', 0xFFFF,0x0000,2);
	for(int i=0;i<0x800000;i++){};			//delay 2s
		
	GPIO_PORTF_DATA_R ^= 0x02;
	for(int i=0;i<0x7A120;i++){};
		
	GPIO_PORTF_DATA_R ^= 0x02;
	ST7735_DrawCharS(10,8, 0x00, 0xFFFF,0x0000,2);		
	ST7735_DrawCharS(10,8, '2', 0xFFFF,0x0000,2);		
	for(int i=0;i<0x800000;i++){};
		
	GPIO_PORTF_DATA_R ^= 0x02;
	for(int i=0;i<0x7A120;i++){};
		
	GPIO_PORTF_DATA_R ^= 0x02;
	ST7735_DrawCharS(10,8, 0x00, 0xFFFF,0x0000,2);		
	ST7735_DrawCharS(10,8, '1', 0xFFFF,0x0000,2);		
	for(int i=0;i<0x800000;i++){};
		
	GPIO_PORTF_DATA_R ^= 0x02;
	for(int i=0;i<0x7A120;i++){};		
		
		
	GPIO_PORTF_DATA_R |= 0x08;
	ST7735_DrawCharS(10,8, 0x00, 0xFFFF,0x0000,2);		
	ST7735_DrawCharS(10,8, 'G', 0xFFFF,0x0000,2);
	ST7735_DrawCharS(30,8, 'O', 0xFFFF,0x0000,2);
	for(int i=0;i<0x7A120;i++){};		
	ST7735_SetRotation(0);

}
int main(void){

	TExaS_Init();	
	Output_Init();
	Port_Init();
	
	//------------Start Screen------------

	ST7735_DrawBitmap(46, 82, TrackMid1, 83,2);
	ST7735_DrawBitmap(40, 159, Mountains, 5,160);
	ST7735_DrawBitmap(46, 59, TrackRight, 83,59);
	ST7735_DrawBitmap(46, 165, TrackLeft, 83,59);
	
	
	ST7735_SetRotation(3);
	ST7735_DrawString(7,2, "Need for spEEd", 0xFFFF);
	ST7735_DrawString(10,8, "Start", 0xFFFF);
	ST7735_SetRotation(0);

	
	//------------In Game------------
	while((GPIO_PORTB_DATA_R&0x02)==0){};
	ST7735_FillScreen(0);
	ST7735_DrawBitmap(40, 159, Mountains, 5,160);
	ST7735_DrawBitmap(46, 59, TrackRight, 83,59);
	ST7735_DrawBitmap(46, 165, TrackLeft, 83,59);
	ST7735_DrawBitmap(46, 82, TrackMid1, 83,2);
	Countdown();
	ST7735_DrawBitmap(0, 159, Sky, 40,160);
	ST7735_DrawBitmap(110, CarY, Car, 20,35);	
	while(ADC_In()>50){};
	while((GPIO_PORTB_DATA_R&0x02)==0){};	
	SysTick_Init();

	while(1){
			GPIO_PORTF_DATA_R |= 0x02;
		
	 while(mainFlag == 1){
		 
			if((loop%2)==0){
				if(track2==1){ST7735_DrawBitmap(46, 82, TrackMid2, 83,2);}
				ST7735_DrawBitmap(110, CarY, Car1, 20,35);
				}
			else{
				if(track1==1){ST7735_DrawBitmap(46, 82, TrackMid1, 83,2);}
				ST7735_DrawBitmap(110, CarY, Car, 20,35);
			}
			mainFlag = 0;
			
			if((GPIO_PORTB_DATA_R&0x01)==1){left = 1;}			//PB0
				else{left = 0;}
			if((GPIO_PORTB_DATA_R&0x02)==0x02){up = 1;}				//PB1
				else{up = 0;}
			if((GPIO_PORTB_DATA_R&0x04)==0x04){right = 1;}			//PB2
				else{right = 0;}
			if(Sample==0){
				Data = ADC_In();
				Sample = 4;
		}
			if(Data > 3500){
				ADCFlag++;
				Data =0;} 	
	 
	// -----------------Animal Spawn-----------------------
				// **Cow**

			if((loop%223)==0){
				CowSpawnx = Spawn1x;
				CowSpawny = Spawn1y;
				if(CowFlag==0){CowFlag = 1;}
				}
			if(CowFlag == 1){
				ST7735_DrawBitmap(CowSpawnx, CowSpawny, Cow, 16,22);
			}
			if(CowSpawnx>130){
				CowFlag = 0;
				CowSpawnx = Spawn1x;
				CowSpawny = Spawn1y;
			}			
			
				// **Deer**
			if((loop%272)==0){
				if((loop!=4080)&&(loop!=2448)){
					DeerSpawnx = Spawn2x;
					DeerSpawny = Spawn2y;
					if(DeerFlag==0){DeerFlag = 1;}
					}
				}
			if(DeerFlag == 1){
				ST7735_DrawBitmap(DeerSpawnx, DeerSpawny, Deer, 16,22);
			}
			if(DeerSpawnx>130){
				DeerFlag = 0;
				DeerSpawnx = Spawn1x;
				DeerSpawny = Spawn1y;
			}
			
			//*****Cross Spawn*****
			
			if((loop%1210)==0){
				if(loop!=3630){
					DeerSpawnx = Spawn1x;
					DeerSpawny = Spawn1y;
					if(DeerFlag==0){DeerFlag = 1;}
					}
				}
			if(DeerFlag == 1){
				ST7735_DrawBitmap(DeerSpawnx, DeerSpawny, Deer, 16,22);
			}
			if(DeerSpawnx>130){
				DeerFlag = 0;
				DeerSpawnx = Spawn1x;
				DeerSpawny = Spawn1y;
			}
			
			if((loop%1290)==0){
				if(loop!=1290){
					CowSpawnx = Spawn2x;
					CowSpawny = Spawn2y;
					if(CowFlag==0){CowFlag = 1;}
				}
			}
			if(CowFlag == 1){
				ST7735_DrawBitmap(CowSpawnx, CowSpawny, Cow, 16,22);
			}
			if(CowSpawnx>130){
				CowFlag = 0;
				CowSpawnx = Spawn1x;
				CowSpawny = Spawn1y;
			}	
			
			
			


//----------Collision check-------------
		
			if(CowFlag == 1){
				int Xc,Yc,a,b = 100;

				Xc=110-CowSpawnx;
				Yc=CowSpawny-CarY;
				if(Yc<0){
					Yc=Yc*-1;
					b=Yc;
					}
				if(Yc>=0){a=Yc;}
				if(Xc<10){
					if((a<16)||(b<35)){
						CollisionFlag = 1;
					}
				}	
			}
			
			if(DeerFlag == 1){
				int Xc,Yc,a,b = 100;

				Xc=110-DeerSpawnx;
				Yc=DeerSpawny-CarY;
				if(Yc<0){
					Yc=Yc*-1;
					b=Yc;
					}
				if(Yc>=0){a=Yc;}
				if(Xc<10){
					if((a<16)||(b<35)){
						CollisionFlag = 1;
					}
				}	
			}
			
	//----------Patt check--------------
		
			if((loop%2300)==0){
				PattFlag = 1;
				PattSpawnx = Spawn1x;
				PattSpawny = Spawn1y;
			}
		
			if(PattFlag == 1){
				ST7735_DrawBitmap(PattSpawnx, PattSpawny, Patt, 29,25);
			}
			
			if(PattSpawnx>130){
				PattFlag = 0;
				PattSpawnx = Spawn1x;
				PattSpawny = Spawn1y;
			}	
			
			if(PattFlag == 1){
				int Xc,Yc,a,b = 100;

				Xc=110-PattSpawnx;
				Yc=PattSpawny-CarY;
				if(Yc<0){
					Yc=Yc*-1;
					b=Yc;
					}
				if(Yc>=0){a=Yc;}
				if(Xc<10){
					if((a<16)||(b<35)){
						Invincible = 1;
					}
				}
			}
			
			if(Invincible == 1){
					ST7735_InvertDisplay(1);
					CollisionFlag = 0;
					if((loop%800)==0){
						ST7735_InvertDisplay(0);
						Invincible = 0;
				}
			}
			
			


	//----------Finish Check-------------
			if(loop==0xFFF){
				FinishFlag = 1;
			}
		}
	 
	//----------Gear Display-------------
		DisableInterrupts();
		if(ADCFlag==0){
			ST7735_SetRotation(3);
			ST7735_DrawString(0,1, "Gear: 1", 0x041F);
			ST7735_SetRotation(0);
		}
		if(ADCFlag==1){
			ST7735_SetRotation(3);
			ST7735_DrawString(0,1, "Gear: 2", 0x041F);
			ST7735_SetRotation(0);
		}
		if(ADCFlag==2){
			ST7735_SetRotation(3);
			ST7735_DrawString(0,1, "Gear: 3", 0x041F);
			ST7735_SetRotation(0);
		}
		if(ADCFlag==3){
			ST7735_SetRotation(3);
			ST7735_DrawString(0,1, "Gear: 4", 0x041F);
			ST7735_SetRotation(0);
		}
		
		Distance = 4096 - loop;
		ST7735_SetRotation(3);
		ST7735_DrawString(0,0, "Distance(m): ", 0x07FF);	
		
		ST7735_SetCursor(13,0);
		ST7735_OutChar(0x00);
		
		ST7735_SetCursor(14,0);
		ST7735_OutChar(0x00);
		
		ST7735_SetCursor(15,0);
		ST7735_OutChar(0x00);
		
		ST7735_SetCursor(16,0);
		ST7735_OutChar(0x00);
		
		ST7735_SetCursor(13,0);
		LCD_OutDec(Distance);
		ST7735_SetRotation(0);	
		
		EnableInterrupts();

	
//----------Collision code-------------

		if(CollisionFlag ==1){
			//explosion sounds
			DisableInterrupts();
			while(1){
				ST7735_DrawBitmap(110, CarY, death, 19,40);
				ST7735_SetRotation(3);
				ST7735_DrawString(10,2, "Game Over", 0xFFFF);
				ST7735_SetRotation(0);
			}
		}
		
//----------Finish code-------------

	 if(FinishFlag ==1){
		 ST7735_DrawBitmap(Fx, Fy, FinishLine, 4,Fh);
		 if(Fx > 126){
				DisableInterrupts();
				while(1){ST7735_SetRotation(3);
				ST7735_DrawString(10,2, "You Win!", 0xFFFF);
				/*
					ST7735_DrawString(7,3,"Time:        s", 0xFFFF);
					ST7735_SetCursor(6,3);
					LCD_OutDec(GameTimer)
				*/
				ST7735_SetRotation(0);}
				}
		 Fx++;
		 if((loop%10)==0){
			Fy+=1;
			if(Fh<82){Fh+=2;}
				}

			for(int i=0;i<0xFFFF;i++){};
	 }

	GPIO_PORTF_DATA_R ^= 0x02;			//Main Heartbeat

 }	
}

void SysTick_Handler(void){
	//DisableInterrupts();
	GPIO_PORTF_DATA_R ^= 0x04;
	mainFlag = 1;
	if(Sample!=0){
				Sample--;
		}
	
	if((loop%2)==0){
		track2=0;
		track1=1;
	}
	else{
		track1=0;
		track2=1;
	}
	
	//-----------------Car Gear-----------------------

	if(right==1){
		if(CarY>0x42){
			CarY--;
		}
	}
	if(left==1){
		if(CarY<0x81){
			CarY++;
		}
	}
	
	if(up==1){
		if(ADCFlag==0){						//Gear 1
			if(Speed>15000000){NVIC_ST_RELOAD_R = (Speed-=1000000)-1;}
		}		
		if(ADCFlag==1){					  //Gear 2
			if(Speed>10000000){NVIC_ST_RELOAD_R = (Speed-=1000000)-1;}
		}	
		if(ADCFlag==2){						//Gear 3
			if(Speed>5000000){NVIC_ST_RELOAD_R = (Speed-=1000000)-1;}
		}		
		if(ADCFlag==3){						//Gear 4
			if(Speed>2500000){NVIC_ST_RELOAD_R = (Speed-=1000000)-1;}
		}	
	}
	else{
		NVIC_ST_RELOAD_R = (Speed+=100000)-1;
		if(ADCFlag!=0){
			ADCFlag--;
		}
	}
	if(Speed>4000000000){Speed=40000000;}			//Max reload register can take
	
	if(CowFlag == 1){
		CowSpawnx++;
		if((CowSpawnx%5)==0){CowSpawny--;}
		}
	
	if(DeerFlag == 1){
		DeerSpawnx++;
		if((DeerSpawnx%5)==0){DeerSpawny++;}
		}
	
	if(PattFlag == 1){
		PattSpawnx++;
		if((PattSpawnx%5)==0){PattSpawny--;}
		}	
	//EnableInterrupts();
	loop++;
	GPIO_PORTF_DATA_R ^= 0x04;

}


