#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
//#include "EdgeInterrupt.cpp"


long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// global variable visible in Watch window of debugger
// increments at least once per button press
void EdgeCounter_Init(void);
void GPIOPortF_Handler(void);
