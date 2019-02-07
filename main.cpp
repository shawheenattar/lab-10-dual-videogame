/*// main.cpp
// Runs on LM4F120/TM4C123
// Shawheen Attar & Jussi Rinta-Jaskari
// EE319K Lab 10 in C++
// Last Modified: 4/30/2018 
//
// *******Hardware I/O connections*******************
//
*/

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Sound.h"
#include "Timer2.h"
#include "DAC.h"
#include "FIFO.h"

#define PF1  (*((volatile uint32_t *)0x40025008))
#define PF2  (*((volatile uint32_t *)0x40025010))
#define PF3  (*((volatile uint32_t *)0x40025020))
#define PF4  (*((volatile uint32_t *)0x40025040))

extern "C" void DisableInterrupts(void); // Disable interrupts
extern "C" void EnableInterrupts(void);  // Enable interrupts
extern "C" long StartCritical (void);    // previous I bit, disable interrupts
extern "C" void EndCritical(long sr);    // restore I bit to previous value
extern "C" void WaitForInterrupt(void);  // low power mode
extern "C" void GPIOE_Handler(void);

extern Queue RxFifo;
SlidePot my(1990,23);

int currentPlayer = 2;
int sendX, sendTrue, sendEnd, sendStart;

typedef enum {startMenuState, playingState, endMenuState} gameState;
typedef enum {dead,alive} status_t;
typedef enum {easy, hard} difficultyLevel;
difficultyLevel DifficultyLevel = easy;

class Player{
	private:
	  int32_t x;      // x coordinate
	  int32_t y;      // y coordinate
	  int32_t xLen;
	  int32_t yLen;
	  const unsigned short *movie[7];
	  int32_t t;
	  uint32_t nframes;
	  status_t life;            // dead/alive
	  int32_t lives;
	public:
		 Player(int32_t ix, int32_t iy,
					 const unsigned short *p,status_t l, 
					 int32_t ixLen, int32_t iyLen){
			x = ix; 
			y = iy;
			movie[0] = p;
			t = 0;
			nframes = 1;
			life = l;	
			xLen = ixLen;
			yLen = iyLen;
			lives = 5;
    }
		Player(){
			life = dead;
		}
		void Init(uint32_t ix, uint32_t iy,
				      const unsigned short *p,status_t l, 
							int32_t ixLen, int32_t iyLen){
			x = ix; 
			y = iy;
			movie[0] = p;
			t = 0;
			life = alive;
			nframes = 1;
			xLen = ixLen;
			yLen = iyLen;
			lives = 5;
    }			
		void MoveX(int ix){
			x = x + ix;
			if (x > 95) x = 95;
			if (x < 0) x = 0;
		}
		void MoveY(int iy){
			y += iy;
			if (y > 159) y = 159;
			if (y < 50) y = 50;
		}
		void SetDead(){
			life = dead;
		}
		status_t Alive(void){
			return life;
		}
    void Draw(void){
				ST7735_DrawBitmap(x, y, movie[t], xLen,yLen);
		}
		void Shoot(){
			t++;
			if(t>=nframes){
				t = nframes-1;
			}
		}
		int AddMovieFrame(const unsigned short *p){
			if(nframes >=10){
				return false;
			}
			movie[nframes] = p;
			nframes++;
			return true;
		}
		uint32_t GetX(void){
			return x;
		}
		uint32_t GetY(void){
			return y;
		}
		uint32_t GetT(){
			return t;
		}
		const unsigned short* getImage(){
			return movie[t];
		}
		
		void checkLives(){
			if (lives > 0){
				for (int i = 0; i < lives; i++){
				  ST7735_SetTextColor(0x20FD);
          ST7735_SetCursor(i, 0);
					ST7735_OutChar(0x03);
				}
				sendEnd = 0;
			}else{
				life = dead;
				sendEnd = 1;
				Sound_Killed();
			}
			ST7735_SetTextColor(0x20FD);
			ST7735_OutString((char*)" ");
		}
		void addBullet(){
			t--;
			if (t < 0){
				t = 0;
			}else{
				Sound_Recharge();
			}
		}
		void decLives(){
			lives--;
		}
};

class Bullet{
	private:
  int32_t x;      // x coordinate
  int32_t y;      // y coordinate
	int32_t xLen;
	int32_t yLen;
	const unsigned short *image;
	int32_t t;
	uint32_t nframes;
  status_t life;            // dead/alive
	uint32_t velocity;
public:
    Bullet(int32_t ix, int32_t iy,
					 const unsigned short *p,status_t l, 
					 int32_t ixLen, int32_t iyLen){
			x = ix; 
			y = iy;
			image = p;
			t = 0;
			nframes = 1;
			life = l;	
			xLen = ixLen;
			yLen = iyLen;
			velocity = 2;
			if (DifficultyLevel == hard) {
				velocity = 3;
			}
    }
		Bullet(){
			life = dead;
		}
		void Init(uint32_t ix, uint32_t iy,
				      const unsigned short *p,status_t l, 
							int32_t ixLen, int32_t iyLen){
			x = ix; 
			y = iy;
			image = p;
			life = l;
			xLen = ixLen;
			yLen = iyLen;
			velocity = 2;
			if (DifficultyLevel == hard) {
				velocity = 3;
			}
    }

    void Down(void){
      y = y + velocity;
			if(y > 170){
				life = dead;
      }
    }
		void Up(){
			y = y - velocity;
			if (y < 0){
				life = dead;
			}
		}
		void SetDead(){
			life = dead;
		}
		status_t Alive(void){
			return life;
		}
    void Draw(void){
				ST7735_DrawBitmap(x, y, image, xLen,yLen);
		}
		void SetX(int ix){
			x = ix;
		}
		
		void SetY(int iy){
			y = iy;
		}
		uint32_t GetX(void){
			return x;
		}
		uint32_t GetY(void){
			return y;
		}
		uint32_t GetT(){
			return t;
		}
		void Clear(){
			image = emptyBullet;
			Draw();
		}
};

gameState GameState = startMenuState;
Player player;
Bullet bullets[6];
Bullet badBullets[6];
int pos = 0, badPos = 0;
uint32_t time = 0;
volatile uint32_t flag;
volatile unsigned long RisingEdges = 0;

void Player_Init(){
	if (currentPlayer == 1){
		player.Init(48, 159, p1rest6, alive, 33, 24);		//for PLAYER1
		player.AddMovieFrame(p1rest5);   //add image to movie array for animation for PLAYER1
		player.AddMovieFrame(p1rest4);  
		player.AddMovieFrame(p1rest3);  
		player.AddMovieFrame(p1rest2);  
		player.AddMovieFrame(p1rest1);  
		player.AddMovieFrame(p1rest0); 
	}else{
		player.Init(48, 159, p2rest6, alive, 33, 24);		//for PLAYER2
  	player.AddMovieFrame(p2rest5);   //add image to movie array for animation for PLAYER 2
		player.AddMovieFrame(p2rest4);  
		player.AddMovieFrame(p2rest3);  
		player.AddMovieFrame(p2rest2);  
		player.AddMovieFrame(p2rest1);  
		player.AddMovieFrame(p2rest0);
	}
}

void PortE_Init(){
	volatile unsigned long delay;
  SYSCTL_RCGCGPIO_R |= 0x10;      // 1) E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_DIR_R &= ~0x02;   // 5) inputs on PE1
	GPIO_PORTE_DEN_R |= 0x02;    // 7) enable digital on PE1
}

void PortF_Init(){
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 (built-in LED)
  GPIO_PORTF_PUR_R |= 0x10;
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF
}
uint32_t ShootIn(){
	return GPIO_PORTE_DATA_R & 0x01;
}

uint32_t RechargeIn(){
	return GPIO_PORTE_DATA_R & 0x02;
}



void checkBullets(){
	for (int i = 0; i < 6; i++){
		if (bullets[i].GetY() <= 2 && bullets[i].Alive()){
			sendTrue = 1;
			sendX = bullets[i].GetX();
			bullets[i].Draw();
			bullets[i].SetDead();
		}
		if (bullets[i].Alive()){
			bullets[i].Draw();
			bullets[i].Up();
		}
	}
}



void checkBadBullets(){
	for (int i = 0; i < 6; i++){
		if (badBullets[i].Alive()){
			badBullets[i].Draw();
			badBullets[i].Down();
		}
		if(badBullets[i].Alive() && (badBullets[i].GetY() >= (player.GetY() - 15)) && (badBullets[i].GetY() <= (player.GetY())) &&
			(badBullets[i].GetX() >= player.GetX()+4) && (badBullets[i].GetX() <= player.GetX()+24)){
			player.decLives();;
			badBullets[i].SetDead();
			badBullets[i].Clear();
		  Sound_Explosion();
		}
		if(badBullets[i].GetY() > 170){
			badBullets[i].SetDead();
		}
	}
}
void killBullets(){
	for(int i = 0; i < 6; i++){
		bullets[i].SetDead();
		bullets[i].Clear();
	}
}

void killBadBullets(){
	for(int i = 0; i < 6; i++){
		badBullets[i].SetDead();
		badBullets[i].Clear();
	}
}




void EdgeCounter_Init(void){       
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
  RisingEdges = 0;             // (b) initialize count and wait for clock
  GPIO_PORTE_DIR_R &= ~0x01;    // (c) make PE0 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x01;  //     disable alt funct on PE0
  GPIO_PORTE_DEN_R |= 0x01;     //     enable digital I/O on PF4
  GPIO_PORTE_PCTL_R &= ~0x0000000F; //  configure PF4 as GPIO
  GPIO_PORTE_AMSEL_R = 0;  //    disable analog functionality on PF4
  GPIO_PORTE_IS_R &= ~0x01;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x01;    //     PF4 is not both edges
  GPIO_PORTE_IEV_R |= 0x01;    //     PF4 rising edge event
  GPIO_PORTE_ICR_R = 0x01;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x01;      // (f) arm interrupt on PF4
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x000000E0; // (g) priority 5
  NVIC_EN0_R = 0x10;          // (h) enable interrupt 30 in NVIC
}

void GPIOE_Handler(void){
  GPIO_PORTE_ICR_R = 0x01;      // acknowledge flag4
  RisingEdges = RisingEdges + 1;
  if (GameState == startMenuState || GameState == endMenuState) {
  	GameState = playingState;
		sendEnd = 0;
  } else if (GameState == playingState && player.GetT() != 6 && player.Alive()){
  	player.Shoot();
  	bullets[pos].Init(player.GetX() + 11, player.GetY() - 33, bullet, alive, 10, 10);
  	pos++;
  	if (pos > 5){
  		pos = 0;
  	}
		time = 0;				//to make sure bullets don't recharge while shooting
		Sound_Shoot();
	}
}

void PrintDifficultyLevel(void) {
  ST7735_SetCursor(0, 1);
	if (DifficultyLevel == easy) {
	  ST7735_OutString((char*)"easy"); 
	} else if (DifficultyLevel == hard) {
	  ST7735_OutString((char*)"hard"); 
	}
}

void background(void){
  flag = 1; // semaphore
	if (GameState == playingState) {
	  checkBullets();
		checkBadBullets();
		if(player.Alive()){
			player.Draw();
		  PrintDifficultyLevel();
		}
		time++;
		if ((DifficultyLevel == easy && time == 30) ||
			  (DifficultyLevel == hard && time == 50)) {
			if (RechargeIn()) {
				player.addBullet();
			}
			time = 0;
		}
	}	
}

void clock(void){
	
	uint32_t data[2];
	ADC_In(data);
	
	my.Save(data[0]);
	int x = my.ADCsample();
	if (x > 3500) x = 2;
	else if (x < 500)	x = -2;
	else	if (x > 2200)	x = 1;
	else if (x < 1900)	x = -1;
	else x = 0;
	player.MoveX(x);
	
	if (GameState == startMenuState || GameState == endMenuState) {
	  if (x > 0 && DifficultyLevel == easy) {
		   DifficultyLevel = hard;
		} else if (x < 0 && DifficultyLevel == hard) {
		   DifficultyLevel = easy;
		}
	}
	
	my.Save(data[1]);
	int y = my.ADCsample();
	if (y > 3500) y = 2;
	else if (y < 500)	y = -2;
	else	if (x > 2200)	y = 1;
	else if (y < 1900)	y = -1;
	else y = 0;
	player.MoveY(y);


	char messageArray[8];
	int num;
	messageArray[0] = 0x02; // STX
	if(sendTrue){
		messageArray[1] = 0x31;
		sendTrue = 0;
		for (int i = 2; i <5; i++){
			num = sendX%10;
			messageArray[i] = 0x30 + num;
			sendX /= 10;
		}
	}else{
		for (int i = 1; i < 5; i++){
			messageArray[i] = 0x30;
		}
	}
	messageArray[5] = sendEnd;
	messageArray[6] = sendStart;
	messageArray[7] = 0x03; // ETX

	for (int i = 0; i < 8; ++i) {
	  UART_OutChar(messageArray[i]);
  }
}

void CheckDifficultyLevel(void) {
  ST7735_SetCursor(1, 14);
	if (DifficultyLevel == easy) {
	  ST7735_OutString((char*)"*Easy   Hard");
	} else {
	  ST7735_OutString((char*)" Easy  *Hard");
	}
}

void FillScreen(uint32_t color) {
	for (int i = 0; i < 10; ++i) {
		ST7735_FillRect(0, 0, 127, 20, color); 														// set screen to black
		ST7735_FillRect(0, 20, 127, 20, color);
		ST7735_FillRect(0, 40, 127, 20, color); 
		ST7735_FillRect(0, 60, 127, 20, color); 
		ST7735_FillRect(0, 80, 127, 20, color); 
		ST7735_FillRect(0, 100, 127, 20, color);
		ST7735_FillRect(0, 120, 127, 20, color);
		ST7735_FillRect(0, 140, 127, 20, color);
		ST7735_FillRect(0, 160, 127, 20, color);
	}
}

int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	PortE_Init();
	PortF_Init();
	EdgeCounter_Init();
	ST7735_InitR(INITR_REDTAB);
	ADC_Init();
	UART_Init();
  Sound_Init();
  Random_Init(1);
  Output_Init();
  Timer0_Init(&background,1333333); // 60 Hz
  Timer1_Init(&clock,1333333); // 60 Hz
  EnableInterrupts();
	char c;
	int xVal = 0;
	
	while(1) {																														// Main loop
		if (GameState == startMenuState) {
			ST7735_FillScreen(0x0000); 																				// set screen to black
			ST7735_DrawBitmap(0, 64, dual_logo, 127, 64);											// Print start menu
			ST7735_SetCursor(1, 8);
			ST7735_OutString((char*)"Press Fire to start");

			ST7735_SetCursor(1, 12);
			ST7735_OutString((char*)"Select difficulty");

			// Wait player to start
			while(GameState == startMenuState) {
			  CheckDifficultyLevel();
			}														
		}

		FillScreen(0x0000);

		Player_Init();																											// Playing
		while(player.Alive() && GameState == playingState){
			
			while(flag==0){};
			flag = 0;
			player.Draw();
			player.checkLives();
			RxFifo.Get(&c);
			RxFifo.Get(&c);	
			if(c == 0x31){
				RxFifo.Get(&c);
				xVal += c-0x30;
				RxFifo.Get(&c);
				xVal = xVal + ((c-0x30)*10);
				RxFifo.Get(&c);
				xVal = xVal + ((c-0x30)*100);
				
				xVal = 120 - xVal;
				badBullets[badPos].Init(xVal, 0, bullet, alive, 10, 10);
				xVal = 0;
				badPos++;
				if (badPos > 5){
					badPos = 0;
				}
			}else{
				for (int i = 2; i < 5; i++){
					RxFifo.Get(&c);
				}	
			}
			RxFifo.Get(&c);
			if (c == 1){
				GameState = endMenuState;
			}
			RxFifo.Get(&c);
			RxFifo.Get(&c);
		}
		GameState = endMenuState;


		killBullets();
		killBadBullets();
		ST7735_FillRect(0, 0, 127, 159, 0x0000); 													// set screen to black
		if (player.Alive()){
		  ST7735_SetCursor(2, 1);
			ST7735_OutString((char*)"CONGRATULATIONS!!");
			ST7735_SetCursor(7, 3);
			ST7735_SetTextColor(ST7735_WHITE);
			ST7735_OutString((char*)"You won!");
		} else {
		  ST7735_SetCursor(6, 1);
			ST7735_OutString((char*)"YOU LOSE!");
			ST7735_SetCursor(6, 3);
			ST7735_SetTextColor(ST7735_WHITE);
			ST7735_OutString((char*)"Nice try.");
		}
		ST7735_SetCursor(1, 8);
		ST7735_OutString((char*)"Press Fire to play");
		ST7735_SetCursor(1, 9);
		ST7735_OutString((char*)"again.");
		ST7735_SetCursor(1, 12);
	  ST7735_OutString((char*)"Select difficulty");
		
		if (player.Alive()){
			Sound_Win();
			player.SetDead();
		}
		
		while (GameState == endMenuState) {
			sendEnd = 0;
			CheckDifficultyLevel();
			while(!RxFifo.IsEmpty()) {
			  RxFifo.Get(&c);
			}
		};
		ST7735_FillScreen(0x0000); // set screen to black
	}
}

