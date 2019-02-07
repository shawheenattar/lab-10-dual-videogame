// SlidePot.cpp
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/28/2018 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "SlidePot.h"
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
// lab 8
	/*
	volatile long delay = 0;
	
	SYSCTL_RCGCGPIO_R |= 0x08;		//PORTD clock
	
	while((SYSCTL_PRGPIO_R & 0x08) == 0){};
	
	GPIO_PORTD_DIR_R &= ~0x04;		//PD2 in
	GPIO_PORTD_AFSEL_R |= 0x04;		//PD2 alternate function
	GPIO_PORTD_DEN_R &= ~0x04;		//disable I/O PD2
	GPIO_PORTD_AMSEL_R |= 0x04;		//enable analog fun on PD2
	
	SYSCTL_RCGCADC_R |= 0x01;	
		
	delay = SYSCTL_RCGCADC_R;       // extra time to stabilize  
	delay = SYSCTL_RCGCADC_R;       // extra time to stabilize  
	delay = SYSCTL_RCGCADC_R;       // extra time to stabilize  
	delay = SYSCTL_RCGCADC_R;
		
	ADC0_PC_R = 0x01;
	ADC0_SSPRI_R = 0x0123;
	ADC0_ACTSS_R &= ~0x0008;
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;
	ADC0_SSCTL3_R = 0x0006;
	ADC0_IM_R &= ~0x0008;
	ADC0_ACTSS_R |= 0x0008;
	*/
	  volatile uint32_t delay;                         
//  SYSCTL_RCGC0_R |= 0x00010000; // 1) activate ADC0 (legacy code)
  SYSCTL_RCGCADC_R |= 0x00000001; // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // 1) activate clock for Port E
  delay = SYSCTL_RCGCGPIO_R;      // 2) allow time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R &= ~0x30;      // 3) make PE4 PE5 input
  GPIO_PORTE_AFSEL_R |= 0x30;     // 4) enable alternate function on PE4 PE5
  GPIO_PORTE_DEN_R &= ~0x30;      // 5) disable digital I/O on PE4 PE5
                                  // 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
  GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFF00FFFF;
  GPIO_PORTE_AMSEL_R |= 0x30;     // 6) enable analog functionality on PE4 PE5
  ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0089;         // 12) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2
}


//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
void ADC_In(uint32_t data[2]){  

// lab 8
		/*
	uint32_t data;  
  ADC0_PSSI_R = 0x0008;            
  while((ADC0_RIS_R&0x08)==0){};     
	data = ADC0_SSFIFO3_R&0xFFF; 
	ADC0_ISC_R = 0x0008;   
	return data;
		*/
		ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  data[1] = ADC0_SSFIFO2_R&0xFFF;  // 3A) read first result
  data[0] = ADC0_SSFIFO2_R&0xFFF;  // 3B) read second result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}

	

// constructor, invoked on creation of class
// m and b are linear calibration coeffients 
SlidePot::SlidePot(uint32_t m, uint32_t b){
// lab 8
	slope = m;
	offset = b;
	data = 0;
	distance = 0;
}

void SlidePot::Save(uint32_t n){
// lab 9
	this->data = n;
	this->distance = this->Convert(n);
	this->flag = 1;
}
uint32_t SlidePot::Convert(uint32_t n){
  // lab 8
	return (this->slope*n)/4096 + this->offset;
}

void SlidePot::Sync(void){
// lab 8	
	while(this->flag == 0){};
	this->flag = 0;
}

uint32_t SlidePot::ADCsample(void){ // return ADC sample value (0 to 4095)

	return this->data;
}

uint32_t SlidePot::Distance(void){  // return distance value (0 to 2000), 0.001cm
// lab 8
  return this->distance; 
}


