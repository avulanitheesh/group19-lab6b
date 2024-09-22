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

void Systick_Handler(void) {
    static uint32_t onTime = 0; // Count for the "ON" duration

    // Generate PWM signal
    if (onTime < (dutyCycle * count) / 100) {
        GPIO_PORTF_DATA_R |= 0x02;    // Turn on LED (PF1)
    } else {
        GPIO_PORTF_DATA_R &= ~0x02;   // Turn off LED (PF1)
    }

    onTime++;
    if (onTime >= count) {
        onTime = 0;                   // Reset onTime after reaching count
    }

    if (buttonPressed) {
        pressTime++;                  // Increment press time if button is pressed
    }
}
void GPIO_Handler(void) {
    if (GPIO_PORTF_RIS_R & 0x10) {   // If interrupt is from PF4 (button press)
        buttonPressed = 1;            // Mark button as pressed
        GPIO_PORTF_ICR_R |= 0x10;    // Clear the interrupt
    }
  if ((GPIO_PORTF_DATA_R & 0x10) == 0x10) { // Button is released
        if (buttonPressed) {
            if (pressTime < 100000) { // Short press
                if (dutyCycle < 100) {
                    dutyCycle += 5;    // Increase duty cycle by 5%
                }
            } else {                   // Long press
                if (dutyCycle > 0) {
                    dutyCycle -= 5;    // Decrease duty cycle by 5%
                }
            }
            pressTime = 0;            // Reset press time
            buttonPressed = 0;        // Reset button press state
        }
    }
}
