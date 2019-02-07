//// Sound.h
//// Runs on TM4C123 or LM4F120
//// Prototypes for basic functions to play sounds from the
//// original Space Invaders.
//// Jonathan Valvano
//// November 17, 2014
#ifndef __SOUND_H__ // do not include more than once
#define __SOUND_H__
#include <stdint.h>

void Sound_Init(void);
void Sound_Play(const uint8_t *pt, uint32_t count);
void Sound_Shoot(void);
void Sound_Killed(void);
void Sound_Explosion(void);
void Sound_Recharge(void);
void Sound_Win(void);


// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void);


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
//void Sound_Play(uint32_t period);
// example periods associated with various notes
// these constants will be a function of bus period and number of elements in your sine table
#define C   2389   // 1046.5 Hz
#define B   2531   // 987.8 Hz
#define BF  2681   // 932.3 Hz
#define A   2841   // 880 Hz
#define AF  3010   // 830.6 Hz
#define G   3189   // 784 Hz
#define GF  3378   // 740 Hz
#define F   3579   // 698.5 Hz
#define E   3792   // 659.3 Hz
#define EF  4018   // 622.3 Hz
#define D   4257   // 587.3 Hz
#define DF  4510   // 554.4 Hz
#define R      0

#endif
