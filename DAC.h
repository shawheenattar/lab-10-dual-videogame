// put prototypes for public functions, explain what it does
// put your names here, date
#ifndef __DAC_H__ // do not include more than once
#define __DAC_H__
#include <stdint.h>
//void SysTick_Handler(void);


void DAC_Init(void);
void DAC_Out(uint32_t data);


#endif
