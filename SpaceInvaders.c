// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/12/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#include "DAC.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void Draw_Piece(uint8_t p, uint8_t r, uint32_t x_offset, uint32_t y_offset);
void Clear_Piece(uint8_t p, uint8_t r, uint32_t x_offset, uint32_t y_offset);
uint32_t Random7(void);
void Game_Screen(void);
void Play_Piece(void);
void Draw_Sub(uint8_t x, uint8_t y, uint8_t p);
void Update_Array(uint8_t r, uint8_t p);
void End_Screen(void);
void Switch_Init(void);
void SysTick_Init(uint32_t period);
uint32_t Convert(uint32_t x);
void SysTick_Handler(void);
void Title_Screen(void);
void ClearArray (void);

uint32_t score = 0;

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
	//ST7735_SetCursor(0, 19);
	//ST7735_OutString("Score");
	//LCD_OutDec(score);
	//playsound(Tetris);
	
}

int main1(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  LCD_OutDec(1234);
  while(1){
  }

}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 7272;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}

void Delay(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 7272;  // 1ms at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}
typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};


typedef struct{
	char ID;
	char block[4][2];//{x, y, x, y, x, y, x, y}
	char height[4];//four height values
}pieces_t;

pieces_t Pieces[7]={
	{1, {1, 5, 5, 5, 9, 5, 9, 1}, {2,2,2,0}},//orange ricky, orange
	{2, {1, 1, 5, 1, 9, 1, 9, 5}, {1,1,2,0}},//blue ricky, dark blue
	{3, {1, 1, 5, 1, 5, 5, 9, 5}, {1,2,2,0}},//cleveland Z, red
	{4, {1, 5, 5, 5, 5, 1, 9, 1}, {2,2,1,0}},//Rhode Island Z, Green
	{5, {1, 1, 1, 5, 1, 9, 1, 13}, {4,0,0,0}},//Hero, Light blue
	{6, {1, 5, 5, 5, 5, 1, 9, 5}, {2,2,2,0}},//Teewee, Purple
	{7, {1, 1, 5, 1, 1, 5, 5, 5}, {2,2,0,0}} //Smash boy, Yellow
};

pieces_t Pieces90[7]={
	{1, {1, 1, 1, 5, 1, 9, 5, 9}, {3,3,0,0}},//orange ricky, orange
	{2, {1, 9, 5, 1, 5, 5, 5, 9}, {3,3,0,0}},//blue ricky, dark blue
	{3, {1, 5, 1, 9, 5, 1, 5, 5}, {3,2,0,0}},//cleveland Z, red
	{4, {1, 1, 1, 5, 5, 5, 5, 9}, {2,3,0,0}},//Rhode Island Z, Green
	{5, {1, 1, 5, 1, 9, 1, 13, 1}, {1,1,1,1}},//Hero, Light blue
	{6, {1, 1, 1, 5, 1, 9, 5, 5}, {3,2,0,0}},//Teewee, Purple
	{7, {1, 1, 5, 1, 1, 5, 5, 5}, {2,2,0,0}} //Smash boy, Yellow
};

pieces_t Pieces180[7]={
	{1, {1, 1, 1, 5, 5, 1, 9, 1}, {2,1,1,0}},//orange ricky, orange
	{2, {1, 1, 5, 5, 9, 5, 1, 5}, {2,2,2,0}},//blue ricky, dark blue
	{3, {1, 1, 5, 1, 5, 5, 9, 5}, {1,2,2,0}},//cleveland Z, red
	{4, {1, 5, 5, 5, 5, 1, 9, 1}, {2,2,1,0}},//Rhode Island Z, Green
	{5, {1, 1, 1, 5, 1, 9, 1, 13}, {4,0,0,0}},//Hero, Light blue
	{6, {1, 1, 5, 1, 9, 1, 5, 5}, {1,2,1,0}},//Teewee, Purple
	{7, {1, 1, 5, 1, 1, 5, 5, 5}, {2,2,0,0}} //Smash boy, Yellow
};

pieces_t Pieces270[7]={
	{1, {1, 1, 5, 1, 5, 5, 5, 9}, {1,3,0,0}},//orange ricky, orange
	{2, {1, 1, 1, 5, 1, 9, 5, 1}, {3,1,0,0}},//blue ricky, dark blue
	{3, {1, 5, 1, 9, 5, 1, 5, 5}, {3,2,0,0}},//cleveland Z, red
	{4, {1, 1, 1, 5, 5, 5, 5, 9}, {2,3,0,0}},//Rhode Island Z, Green
	{5, {1, 1, 5, 1, 9, 1, 13, 1}, {1,1,1,1}},//Hero, Light blue
	{6, {1, 5, 5, 1, 5, 5, 5, 9}, {2,3,0,0}},//Teewee, Purple
	{7, {1, 1, 5, 1, 1, 5, 5, 5}, {2,2,0,0}} //Smash boy, Yellow
};

typedef struct{
	char piece;
	uint32_t HowLongCanItFall;
	uint16_t Col[4][2];
	uint32_t y_level;
	char rotation;
}status_t;

status_t status[1]={
	{0, 140, {0,10, 0,11, 0,12, 0,13}, 0, 1}
};

//char col_height[20] = {142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142};
char Grid[36][20] = {//play area, 20 units wide 35 units tall, each element stores the color of block, if black, means empty
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,//36
};
int p=0;
int queue1;
int queue2;
int r;
int r_save = 1;
int col;
int reset = 0;
//uint32_t score = 0;
uint32_t horizontal = 44;//test value, set this to converted ADC value 
uint32_t horizontal_save = 44;
uint32_t Data = 0;
uint32_t Flag = 0;
uint32_t ADC_x_value = 0;
int select = 1;


#define level_1 200
#define level_2 150
#define level_3 100
#define level_4 75
#define level_5 60
#define level_6 45
#define level_7 30
#define level_8 25
#define level_9 20
#define JHIN 5

int speed = level_1;
int speed_save = level_1;

#define English 0
#define Spanish 1

int language = English;

int main(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
	ADC_Init();
	DAC_Init();
	SysTick_Init(800000);
  Random_Init(ADC_In());
  Output_Init();
	Switch_Init();
	SYSCTL_RCGCGPIO_R |= 0x34;
	Delay(1);
	GPIO_PORTC_DEN_R |= 0x60;
	GPIO_PORTC_DIR_R &= ~0x60;//switches on PC6-5
	Sound_Init();
	//Timer1_Init(80000000/11520, 5);
  ST7735_FillScreen(0x0000);            // set screen to black
	EnableInterrupts();
	
	while(1){
	ST7735_FillScreen(0x0000);  
	Title_Screen();
	ST7735_FillRect(4, 0, 80, 140, ST7735_Color565(0, 0, 0));
	Delay(500);
	//playsound(Tetris);
	Game_Screen();
	Play_Piece();
	End_Screen();
	};

}

void Title_Screen(void){
//		uint32_t color;
//			color = ST7735_Color565(255, 160, 0);
//			color = ST7735_Color565(0, 50, 200);
//			color = ST7735_Color565(255, 0, 0);
//			color = ST7735_Color565(0, 255, 0);
//			color = ST7735_Color565(0, 255, 255);
//			color = ST7735_Color565(128, 0, 128);
//			color = ST7735_Color565(255, 255, 0);
	
	//T
	Draw_Sub(4+20, 4+10, 0);
	Draw_Sub(8+20, 4+10, 0);
	Draw_Sub(12+20, 4+10, 0);
	Draw_Sub(8+20, 8+10, 0);
	Draw_Sub(8+20, 12+10, 0);
	Draw_Sub(8+20, 16+10, 0);
	Draw_Sub(8+20, 20+10, 0);
	
	//E
	Draw_Sub(20+20, 8+10, 1);
	Draw_Sub(20+20, 12+10, 1);
	Draw_Sub(20+20, 16+10, 1);
	Draw_Sub(20+20, 20+10, 1);
	Draw_Sub(20+20, 24+10, 1);
	Draw_Sub(24+20, 8+10, 1);
	Draw_Sub(24+20,16+10, 1);
	Draw_Sub(24+20, 24+10, 1);
	Draw_Sub(28+20, 8+10, 1);
	Draw_Sub(28+20, 24+10, 1);
	
	//T
	Draw_Sub(36+20, 4+10, 2);
	Draw_Sub(40+20, 4+10, 2);
	Draw_Sub(44+20, 4+10, 2);
	Draw_Sub(40+20, 8+10, 2);
	Draw_Sub(40+20, 12+10, 2);
	Draw_Sub(40+20, 16+10, 2);
	Draw_Sub(40+20, 20+10, 2);
	
	//R
	Draw_Sub(52+20, 8+10, 3);
	Draw_Sub(52+20, 12+10, 3);
	Draw_Sub(52+20, 16+10, 3);
	Draw_Sub(52+20, 20+10, 3);
	Draw_Sub(52+20, 24+10, 3);
	Draw_Sub(56+20, 8+10, 3);
	Draw_Sub(56+20, 16+10, 3);
	Draw_Sub(60+20, 12+10, 3);//
	Draw_Sub(54+20, 18+10, 3);
	Draw_Sub(56+20, 20+10, 3);
	Draw_Sub(58+20, 22+10, 3);
	Draw_Sub(60+20, 24+10, 3);
	
	//I
	Draw_Sub(68+20, 4+10, 4);
	Draw_Sub(68+20, 8+10, 4);
	Draw_Sub(68+20, 12+10, 4);
	Draw_Sub(68+20, 16+10, 4);
	Draw_Sub(68+20, 20+10, 4);
	
	//S
	Draw_Sub(76+20, 12+10, 5);
	Draw_Sub(78+20, 8+10, 5);
	Draw_Sub(78+20, 16+10, 5);
	Draw_Sub(82+20, 8+10, 5);
	Draw_Sub(84+20, 12+10, 5);
	Draw_Sub(80+20, 20+10, 5);
	Draw_Sub(84+20, 24+10, 5);
	Draw_Sub(80+20, 32+10, 5);
	Draw_Sub(76+20, 36+10, 4);
	Draw_Sub(80+20, 30+10, 2);
	Draw_Sub (70+20, 35+10, 6);
	
	
while(1){
	
	ST7735_FillRect(20, 60, 100, 30, ST7735_Color565(0, 0, 0)); 
	ST7735_SetCursor(7, 7);
	if(language == English){
	ST7735_OutString("START!");
	};
	if(language == Spanish){
	ST7735_OutString("iEMPEZAR!");
	};
	ST7735_SetCursor(7, 10);
	ST7735_OutString("English");
	ST7735_SetCursor(7, 12);
	ST7735_OutString("Espa\xA4ol");
	
	if(select == 1){
		Draw_Sub(30, 71, 98);
		Delay(100);
		Draw_Sub(30, 71, 99);
		Delay(100);
		if((GPIO_PORTC_DATA_R&0x20) == 0x20){
			Delay(1);
			if((GPIO_PORTC_DATA_R&0x20) == 0x20){
				return;
			};
		};
	};
	
	if(select == 2){
		Draw_Sub(30, 101, 98);
		Delay(100);
		Draw_Sub(30, 101, 99);
		Delay(100);
		if((GPIO_PORTC_DATA_R&0x20) == 0x20){
			Delay(1);
			if((GPIO_PORTC_DATA_R&0x20) == 0x20){
				language = English;
			};
		};
	};
	
	if(select == 3){
		Draw_Sub(30, 120, 98);
		Delay(100);
		Draw_Sub(30, 120, 99);
		Delay(100);
		if((GPIO_PORTC_DATA_R&0x20) == 0x20){
			Delay(1);
			if((GPIO_PORTC_DATA_R&0x20) == 0x20){
				language = Spanish;
			};
		};
	};
	
	if((GPIO_PORTC_DATA_R&0x40) == 0x40){
			Delay(1);
			if((GPIO_PORTC_DATA_R&0x40) == 0x40){
				select += 1;
				if(select > 3){
					select = 1;
				};
			};
		};
	
};
	

};

void Game_Screen(void){
playsound(Tetris);
int white = ST7735_Color565(255, 255, 255);
	//ST7735_FillScreen(0x0000);
	ST7735_FillRect(2, 0, 2, 144, white);//left border
	//ST7735_FillRect(2, 0, 84, 2, white);//top border
	ST7735_FillRect(84, 0, 2, 144, white);//right border
	ST7735_FillRect(2, 144, 84, 2, white);//bottom border
	};

void Play_Piece(void){
	p = Random7();
	queue1 = Random7();
	queue2 = Random7();
	r = 1;//////////////////////////////////////////////////////////////////////////////////////
	int black = ST7735_Color565(0, 0, 0);
	
	//side panel displaying current piece and next two pieces
	while(1){
		ST7735_FillRect(86, 1, 40, 160, black);
		ST7735_SetCursor(15, 1);
		if(language == English){
		ST7735_OutString("Now");
		};
		if(language == Spanish){
		ST7735_OutString("Ahora");
		};
		Draw_Piece(p, r, 105, 30);
		ST7735_SetCursor(15, 7);
		if(language == English){
		ST7735_OutString("Next");
		};
		if(language == Spanish){
		ST7735_OutString("Luego");
		};
		Draw_Piece(queue1, r, 105, 100);
		Draw_Piece(queue2, r, 105, 130);
		
		status[0].piece = p;
		status[0].HowLongCanItFall = 140;
		status[0].y_level = 0;
		status[0].rotation = r;

	int next=0;
	int j=0;
	status[0].y_level = 0;
	horizontal = 44;
		
//			//rotation
//	if((GPIO_PORTC_DATA_R&0x20) == 0x20){
//			Delay(1);
//			if((GPIO_PORTC_DATA_R&0x20) == 0x20){
//				r += 1;
//				if(r > 4){
//					r = 1;
//				};
//				status[0].rotation = r;
//				
//			};
//		};
//	
	if(r == 1){
	status[0].Col[0][0] = Pieces[p].height[0];
	status[0].Col[1][0] = Pieces[p].height[1];
	status[0].Col[2][0] = Pieces[p].height[2];
	status[0].Col[3][0] = Pieces[p].height[3];
	};
		if(r == 2){
	status[0].Col[0][0] = Pieces90[p].height[0];
	status[0].Col[1][0] = Pieces90[p].height[1];
	status[0].Col[2][0] = Pieces90[p].height[2];
	status[0].Col[3][0] = Pieces90[p].height[3];
	};
			if(r == 3){
	status[0].Col[0][0] = Pieces180[p].height[0];
	status[0].Col[1][0] = Pieces180[p].height[1];
	status[0].Col[2][0] = Pieces180[p].height[2];
	status[0].Col[3][0] = Pieces180[p].height[3];
	};
				if(r == 4){
	status[0].Col[0][0] = Pieces270[p].height[0];
	status[0].Col[1][0] = Pieces270[p].height[1];
	status[0].Col[2][0] = Pieces270[p].height[2];
	status[0].Col[3][0] = Pieces270[p].height[3];
	};


	if(score >=60){
		speed = level_2;
	};
	if(score >=120){
		speed = level_3;
	};
	if(score >=180){
		speed = level_4;
	};
	if(score >=240){
		speed = level_5;
	};
	if(score >=300){
		speed = level_6;
	};
	if(score >=360){
		speed = level_7;
	};
	if(score >=420){
		speed = level_8;
	};
	if(score >=480){
		speed = level_9;
	};
	

if(speed == JHIN){
	speed = speed_save;
};	
speed_save = speed;
	
//check bottom and then move piece down
while(j != 1){
	int l = 0;
	//for(int i = 19, k = 0; i<=1700; i+=93){//software values
	for(int i = 1055, k = 0; i<=1700; i+=30){//hardware values
		if(ADC_x_value <= i && (l != 1)){
				horizontal_save = horizontal;
				horizontal = k;
				status[0].Col[0][1] = (k/4);
				status[0].Col[1][1] = status[0].Col[0][1]+1;
				status[0].Col[2][1] = status[0].Col[0][1]+2;
				status[0].Col[3][1] = status[0].Col[0][1]+3;
			l = 1;
		};
		k+=4;
	};
	
	//rotation
	if((GPIO_PORTC_DATA_R&0x20) == 0x20){
			Delay(1);
			if((GPIO_PORTC_DATA_R&0x20) == 0x20){
				r_save = r;
				r += 1;
				if(r > 4){
					r = 1;
				};
				status[0].rotation = r;
				
			};
		};
	
	//JHIN
	if((GPIO_PORTC_DATA_R&0x40) == 0x40){
		Delay(1);
		if((GPIO_PORTC_DATA_R&0x40) == 0x40){
			speed = JHIN;
		};
	};
	
		if(r == 1){
	status[0].Col[0][0] = Pieces[p].height[0];
	status[0].Col[1][0] = Pieces[p].height[1];
	status[0].Col[2][0] = Pieces[p].height[2];
	status[0].Col[3][0] = Pieces[p].height[3];
	};
		if(r == 2){
	status[0].Col[0][0] = Pieces90[p].height[0];
	status[0].Col[1][0] = Pieces90[p].height[1];
	status[0].Col[2][0] = Pieces90[p].height[2];
	status[0].Col[3][0] = Pieces90[p].height[3];
	};
			if(r == 3){
	status[0].Col[0][0] = Pieces180[p].height[0];
	status[0].Col[1][0] = Pieces180[p].height[1];
	status[0].Col[2][0] = Pieces180[p].height[2];
	status[0].Col[3][0] = Pieces180[p].height[3];
	};
				if(r == 4){
	status[0].Col[0][0] = Pieces270[p].height[0];
	status[0].Col[1][0] = Pieces270[p].height[1];
	status[0].Col[2][0] = Pieces270[p].height[2];
	status[0].Col[3][0] = Pieces270[p].height[3];
	};

if((p==4)&&((r==1)||(r==3))){
	if(status[0].Col[0][1]>20){
		status[0].Col[0][1] = 20;
	};
	if(Grid[next + (status[0].Col[0][0])][status[0].Col[0][1]] != 9){
		j=1;
	};
};

if(((p==0)||(p==1)||(p==2)||(p==3)||(p==5))&&((r==2)||(r==4))){
	if(Grid[next + (status[0].Col[0][0])][status[0].Col[0][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[1][0])][status[0].Col[1][1]] != 9){
		j=1;
	};
};

if(p==6){
	if(Grid[next + (status[0].Col[0][0])][status[0].Col[0][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[1][0])][status[0].Col[1][1]] != 9){
		j=1;
	};
};

if(((p==0)||(p==1)||(p==2)||(p==3)||(p==5))&&((r==1)||(r==3))){
	if(Grid[next + (status[0].Col[0][0])][status[0].Col[0][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[1][0])][status[0].Col[1][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[2][0])][status[0].Col[2][1]] != 9){
		j=1;
	};
};

if((p==4)&&((r==2)||(r==4))){
	if(Grid[next + (status[0].Col[0][0])][status[0].Col[0][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[1][0])][status[0].Col[1][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[2][0])][status[0].Col[2][1]] != 9){
		j=1;
	};
	if(Grid[next + (status[0].Col[3][0])][status[0].Col[3][1]] != 9){
		j=1;
	};
};
	
	next++;
	DisableInterrupts();
	Clear_Piece(p, r, horizontal_save+4, status[0].y_level);
	Clear_Piece(p, r, horizontal+4, status[0].y_level);
	Clear_Piece(p, r_save, horizontal_save+4, status[0].y_level);
	Clear_Piece(p, r_save, horizontal+4, status[0].y_level);
	Draw_Piece(p, r, horizontal+4, (status[0].y_level+4));
	EnableInterrupts();
	Delay(speed);
	status[0].y_level +=4;
	
};

	status[0].y_level -=4;
	next--;

//update the array after piece stops moving
	Update_Array(status[0].rotation, p);
	playsound(Drop);
//check play field for filled rows and add score
	for(int i=0; i<35; i++){
		
		if(Grid[i][0] != 9){
			if(Grid[i][1] != 9){
			if(Grid[i][2] != 9){
			if(Grid[i][3] != 9){
			if(Grid[i][4] != 9){
			if(Grid[i][5] != 9){
			if(Grid[i][6] != 9){
			if(Grid[i][7] != 9){
			if(Grid[i][8] != 9){
			if(Grid[i][9] != 9){
			if(Grid[i][10] != 9){
			if(Grid[i][11] != 9){
			if(Grid[i][12] != 9){
			if(Grid[i][13] != 9){
			if(Grid[i][14] != 9){
			if(Grid[i][15] != 9){
			if(Grid[i][16] != 9){
			if(Grid[i][17] != 9){
			if(Grid[i][18] != 9){
			if(Grid[i][19] != 9){
			score += 20;
			playsound(Clear);
				for(int k=i; k>0; k--){
					for(int j=0; j<20; j++){
						Grid[k][j] = Grid[k-1][j];
					};
				};
				for(int j=0; j<20; j++){
					Grid[0][j] = 9;
				};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		};
		
	};

//reprint screen
	for(int i=0; i<35; i++){
		for(int j=0; j<20; j++){
			Draw_Sub((j*4+4), ((i+1)*4), (Grid[i][j]));
		};
	};
	Game_Screen();
	
//check top row for game over
		for(int j=0; j<20; j++){
			if(Grid[0][j] != 9){
				return;
			};
		};

//		if((Grid[0][horizontal] != 9) || (Grid[0][horizontal+1] != 9)){
//		return;
//		};
	
		p = queue1;
		queue1 = queue2;
		queue2 = Random7();
	};
};
	


//Draw_Piece function
//Input: p = ID for the piece, r = rotation, x offset, y offset
//Ouput: none, will display piece associated with p at (x, y)
void Draw_Piece(uint8_t p, uint8_t r, uint32_t x_offset, uint32_t y_offset){
	int x; int y; 
	
	if(r == 1){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces[p].block[i][0]-1;
	y = y_offset + Pieces[p].block[i][1]-1;
		Draw_Sub(x, y, p);
	};
}
	 	if(r == 2){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces90[p].block[i][0]-1;
	y = y_offset + Pieces90[p].block[i][1]-1;
		Draw_Sub(x, y, p);
	};
}
	 	if(r == 3){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces180[p].block[i][0]-1;
	y = y_offset + Pieces180[p].block[i][1]-1;
		Draw_Sub(x, y, p);
	};
}
	 	if(r == 4){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces270[p].block[i][0]-1;
	y = y_offset + Pieces270[p].block[i][1]-1;
		Draw_Sub(x, y, p);
	};
}
}

void Draw_Sub(uint8_t x, uint8_t y, uint8_t p){
	uint32_t color;
		if(Pieces[p].ID == 1){
			color = ST7735_Color565(255, 160, 0);
		};
		if(Pieces[p].ID == 2){
			color = ST7735_Color565(0, 50, 200);
		};
		if(Pieces[p].ID == 3){
			color = ST7735_Color565(255, 0, 0);
		};
		if(Pieces[p].ID == 4){
			color = ST7735_Color565(0, 255, 0);
		};
		if(Pieces[p].ID == 5){
			color = ST7735_Color565(0, 255, 255);
		};
		if(Pieces[p].ID == 6){
			color = ST7735_Color565(128, 0, 128);
		};
		if(Pieces[p].ID == 7){
			color = ST7735_Color565(255, 255, 0);
		};
		if(p == 99){
			color = ST7735_Color565(0, 0, 0);
		};
		if(p == 9){
			color = ST7735_Color565(0, 0, 0);
		};
		if(p == 98){
			color = ST7735_Color565(255, 255, 255);
		};
		
		ST7735_DrawPixel(x, y, color);/////////////
		ST7735_DrawPixel(x+1, y, color);
		ST7735_DrawPixel(x, y+1, color);
		ST7735_DrawPixel(x+1, y+1, color);
		ST7735_DrawPixel(x+2, y, color);/////////////
		ST7735_DrawPixel(x+3, y, color);
		ST7735_DrawPixel(x+2, y+1, color);
		ST7735_DrawPixel(x+3, y+1, color);
		ST7735_DrawPixel(x, y+2, color);/////////////
		ST7735_DrawPixel(x, y+3, color);
		ST7735_DrawPixel(x+1, y+2, color);
		ST7735_DrawPixel(x+1, y+3, color);
		ST7735_DrawPixel(x+2, y+2, color);/////////////
		ST7735_DrawPixel(x+2, y+3, color);
		ST7735_DrawPixel(x+3, y+2, color);
		ST7735_DrawPixel(x+3, y+3, color);
	};

void Clear_Piece(uint8_t p, uint8_t r, uint32_t x_offset, uint32_t y_offset){
	int x; int y; 
	int z = 99;
	
	if(r == 1){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces[p].block[i][0]-1;
	y = y_offset + Pieces[p].block[i][1]-1;
		Draw_Sub(x, y, z);
	};
}
	 	if(r == 2){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces90[p].block[i][0]-1;
	y = y_offset + Pieces90[p].block[i][1]-1;
		Draw_Sub(x, y, z);
	};
}
	 	if(r == 3){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces180[p].block[i][0]-1;
	y = y_offset + Pieces180[p].block[i][1]-1;
		Draw_Sub(x, y, z);
	};
}
	 	if(r == 4){
	for(int i = 0; i<4; i++){
	x = x_offset + Pieces270[p].block[i][0]-1;
	y = y_offset + Pieces270[p].block[i][1]-1;
		Draw_Sub(x, y, z);
	};
}

		
	};

void End_Screen(void){
	int white = ST7735_Color565(255, 255, 255);
	int black = ST7735_Color565(0, 0, 0);
	ST7735_FillRect(14, 20, 60, 100, white);
	ST7735_FillRect(19, 25, 50, 90, black);
	if(language == English){
	ST7735_SetCursor(5, 4);
	ST7735_OutString("Game");
	ST7735_SetCursor(5, 5);
	ST7735_OutString("Over!");
	};
	if(language == Spanish){
		ST7735_SetCursor(5, 4);
		ST7735_OutString("iFin!");
	};
	ST7735_SetCursor(5, 7);
	if(language == English){
		ST7735_OutString("Score:");
		};
		if(language == Spanish){
			ST7735_SetCursor(4, 7);
			ST7735_OutString("Puntos:");
		};
	ST7735_SetCursor(6, 8);
	LCD_OutDec(score);
	
	while(reset != 1){};
	ClearArray();
	reset = 0;
	return;
	
};

void ClearArray (void){
	for(int i=0; i<35; i++){
		for(int j=0; j<20; j++){
			Grid[i][j] =  9;
		};
	};
};
	
uint32_t Random7(void){
  return ((Random32()>>24)%7);  // returns 0, 1, 2, 3, 4, 5, 6
}
 
void Update_Array(uint8_t r, uint8_t p){
if(r == 1){
	if(p == 0){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
	};
	if(p == 1){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
	};
	if(p == 2){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
	};
	if(p == 3){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
	};
	if(p == 4){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i+3][status[0].Col[0][1]] = p;
	};
	if(p == 5){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
	};
	if(p == 6){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
	};
};

if(r == 2){//////////////////////////////////////////////////////////////////////////////////
	if(p == 0){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
	};
	if(p == 1){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
	};
	if(p == 2){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
	};
	if(p == 3){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
	};
	if(p == 4){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
		Grid[i][status[0].Col[3][1]] = p;
	};
	if(p == 5){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
	};
	if(p == 6){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
	};
};

if(r == 3){//////////////////////////////////////////////////////////////////////////////////
	if(p == 0){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
	};
	if(p == 1){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
	};
	if(p == 2){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[2][1]] = p;
	};
	if(p == 3){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
	};
	if(p == 4){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i+3][status[0].Col[0][1]] = p;
	};
	if(p == 5){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
	};
	if(p == 6){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
	};
};

if(r == 4){//////////////////////////////////////////////////////////////////////////////////
	if(p == 0){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
	};
	if(p == 1){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
	};
	if(p == 2){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+2][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
	};
	if(p == 3){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
	};
	if(p == 4){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[2][1]] = p;
		Grid[i][status[0].Col[3][1]] = p;
	};
	if(p == 5){
		int i = (status[0].y_level/4);
		Grid[i][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i+2][status[0].Col[1][1]] = p;
		Grid[i+1][status[0].Col[0][1]] = p;
	};
	if(p == 6){
		int i = (status[0].y_level/4);
		Grid[i+1][status[0].Col[0][1]] = p;
		Grid[i+1][status[0].Col[1][1]] = p;
		Grid[i][status[0].Col[0][1]] = p;
		Grid[i][status[0].Col[1][1]] = p;
	};
};
}




void SysTick_Init(uint32_t period){
  NVIC_ST_CTRL_R=0;// write this
	NVIC_ST_RELOAD_R= 0x00002710;//0x001E8480;//0x003D0900;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 7;
}

void SysTick_Handler(void){
  //GPIO_PORTF_DATA_R ^= 0x04	; // toggle PF2
  Data = ADC_In();      // Sample ADC
	ADC_x_value = Convert(Data);
	playsample();
}

uint32_t Convert(uint32_t x){
  // write this
	return 1680*x/4096+19;
}









void Switch_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x00000020; // activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};

  GPIO_PORTF_DIR_R  &= ~0x10;     //  make PF4 in (built-in button)
  GPIO_PORTF_DEN_R  |=  0x10;     //  enable digital I/O on PF4   
  GPIO_PORTF_PUR_R  |=  0x10;     //  enable weak pull-up on PF4
  GPIO_PORTF_IS_R   &= ~0x10;     //  PF4 is edge-sensitive
  GPIO_PORTF_IBE_R  |=  0x10;     //  PF4 is both edges
  GPIO_PORTF_ICR_R = 0x10;    // clear flag4
  GPIO_PORTF_IM_R |= 0x10;    // arm interrupt on PF4  
  NVIC_PRI7_R = (NVIC_PRI7_R
               & 0xFF00FFFF)
               | 0x00A00000;  // Set Priority Level to 5
  NVIC_EN0_R = 0x40000000;    // enable interrupt 30 in NVIC  
  
}

void GPIOPortF_Handler(void){
  
  //GPIO_PORTF_RIS_R is trigger flag
  if((GPIO_PORTF_RIS_R&0x10) == 0x10){  // 0x10 means released
    // execute release task
		reset = 1;
  }
  else{
    // execute touch task
		reset = 1;
  }
	GPIO_PORTF_ICR_R = 0x10;     // ack PF4 
}


//	Draw_Piece(1-1, 1, 1, 1);
//	Draw_Piece(2-1, 1, 1, 20);
//	Draw_Piece(3-1, 1, 1, 40);
//	Draw_Piece(4-1, 1, 1, 80);
//	Draw_Piece(5-1, 1, 1, 100);
//	Draw_Piece(6-1, 1, 1, 120);
//	Draw_Piece(7-1, 1, 1, 140);
//	
//	Draw_Piece(1-1, 2, 18, 1);
//	Draw_Piece(2-1, 2, 18, 20);
//	Draw_Piece(3-1, 2, 18, 40);
//	Draw_Piece(4-1, 2, 18, 80);
//	Draw_Piece(5-1, 2, 18, 100);
//	Draw_Piece(6-1, 2, 18, 120);
//	Draw_Piece(7-1, 2, 18, 140);
//	
//	Draw_Piece(1-1, 3, 48, 1);
//	Draw_Piece(2-1, 3, 48, 20);
//	Draw_Piece(3-1, 3, 48, 40);
//	Draw_Piece(4-1, 3, 48, 80);
//	Draw_Piece(5-1, 3, 48, 100);
//	Draw_Piece(6-1, 3, 48, 120);
//	Draw_Piece(7-1, 3, 48, 140);
//	
//	Draw_Piece(1-1, 4, 100, 1);
//	Draw_Piece(2-1, 4, 100, 20);
//	Draw_Piece(3-1, 4, 100, 40);
//	Draw_Piece(4-1, 4, 100, 80);
//	Draw_Piece(5-1, 4, 100, 100);
//	Draw_Piece(6-1, 4, 100, 120);
//	Draw_Piece(7-1, 4, 100, 140);
int main2(void){ char l;
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Delay100ms(30);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Delay100ms(20);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }  
}

