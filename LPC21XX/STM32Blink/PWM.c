#include "stm32f4xx.h"
int main() {
    RCC->AHB1ENR |= 1;              /* enable GPIOA clock */
		RCC->AHB1ENR |= 2;              /* enable GPIOB clock */
    
    GPIOA->AFR[0] |= 0x00100000;    /* PA5 pin for tim2 */
    GPIOA->MODER &= ~0x00000C00;
    GPIOA->MODER |=  0x00000800;
		GPIOB->MODER &= ~((1<<8)|(1<<9));        /* clear pin mode to input mode */
    GPIOB->PUPDR &= ~(1<<9);
		GPIOB->PUPDR |= (1<<8);					// Configure internal Pull up on port PB4 for pull down switch
    /* setup TIM2 */
    RCC->APB1ENR |= 1;              /* enable TIM2 clock */
    TIM2->PSC = 10 - 1;             /* divided by 10 */
    TIM2->ARR = 26667 - 1;          /* divided by 26667 */
    TIM2->CNT = 0;
    TIM2->CCMR1 = 0x0060;           /* PWM mode */
    TIM2->CCER = 1;                 /* enable PWM Ch1 */
    TIM2->CCR1 = 8889 - 1;          /* pulse width 1/3 of the period */

    while(1) {
	if ((GPIOB->IDR & (1<<4))==(1<<4))
	{
		TIM2->CR1 = 0;			 // Disable PWM for low
	}
  else{
		TIM2->CR1 = 1;			// Enable PWM for low
	}
}
}