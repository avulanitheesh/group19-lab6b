#include <stdint.h>
#include "tm4c123gh6pm.h"
volatile uint32_t dutyCycle = 50;  // Initial duty cycle: 50%
volatile uint32_t count = 160;      // Count value for 100 kHz PWM (16 MHz clock)
volatile uint32_t pressTime = 0;    // Time the button has been held down
volatile uint8_t buttonPressed = 0;  // Button press detection
void init(void);
void Systick_Init(void);
void GPIO_Handler(void);
void Systick_Handler(void);
int main(void) {
    init();           // Initialize GPIO for switches and LED
    Systick_Init();   // Initialize SysTick timer

    while (1) {
        // Main loop does nothing; PWM and duty cycle control handled in interrupts
    }
}

