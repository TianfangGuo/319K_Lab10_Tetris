// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 11/15/2021 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
  // put Lab 8 solution here
	SYSCTL_RCGCADC_R |= 0x01;// activate ADC0
	int noOP1 = 1;
	noOP1 = 2;
	noOP1 = 3; //clock stabilization
	SYSCTL_RCGCGPIO_R |= 0x08;// activate clock for portD
	int noOP2 = 1;
	noOP2 = 2;
	noOP2 = 3; //clock stabilization
	
	GPIO_PORTD_DIR_R   &= 0xFB;// PD2 input
	GPIO_PORTD_AFSEL_R |= 0x04;// enable alt function
	GPIO_PORTD_DEN_R   &= 0xFB;// disable digital
	GPIO_PORTD_AMSEL_R |= 0x04;// enable analog
	
	ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 5;           // 12) set channel
  ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
	
	ADC0_SAC_R = 6;

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
  // put Lab 8 solution here
	uint32_t result;
	// 1) initiate SS3
	ADC0_PSSI_R = 0x0008;
  // 2) wait for conversion done
	while((ADC0_RIS_R&0x08)==0){};
  // 3) read result
	result = ADC0_SSFIFO3_R&0xFFF;
  // 4) acknowledge completion
	return result;
}


