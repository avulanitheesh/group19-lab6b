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
void init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;      // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B; // Unlock Port F
    GPIO_PORTF_CR_R = 0x10;          // Allow changes to PF4
    GPIO_PORTF_DIR_R = 0x02;         // Set PF1 as output (LED)
    GPIO_PORTF_DEN_R = 0x12;         // Digital enable for PF1 and PF4
    GPIO_PORTF_PUR_R |= 0x10;        // Enable pull-up resistor for PF4
 GPIO_PORTF_IS_R = 0x00;          // Edge-sensitive
    GPIO_PORTF_IBE_R = 0x00;         // Interrupt controlled by IEV
    GPIO_PORTF_IEV_R = 0x00;         // Falling edge trigger
    GPIO_PORTF_ICR_R = 0x10;         // Clear prior interrupt
    GPIO_PORTF_IM_R = 0x10;          // Unmask interrupt for PF4

    NVIC_EN0_R = 0x40000000;         // Enable interrupt in NVIC
}
void Systick_Init(void) {
    NVIC_ST_RELOAD_R = count - 1;    // Set reload value
    NVIC_ST_CTRL_R = 0x07;            // Enable SysTick with core clock
}

