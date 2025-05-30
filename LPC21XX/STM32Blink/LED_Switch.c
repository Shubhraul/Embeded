#include "stm32f4xx.h" 
int main(){
	RCC->AHB1ENR |= 1;  //Enable GPIO A Clock
	RCC->AHB1ENR |= 4; //Enable GPIO C Clock 
	GPIOA->MODER |= (1<<10);   //Set bit 10 to set GPIOA-5 as output
	GPIOA->MODER &= ~(1<<11);  //clear bit 11 to set GPIOA-5 as output
	GPIOC->MODER &= ~((1<<26) | (1<<27)); // clear 1:0 for port C13 as input 2y:2y+1
	//GPIOC->OTYPER |= (1<<5);
	//GPIOC->PUPDR &= ~(1<<26);
	//GPIOC->PUPDR |= (1<<27);
	while(1){
		if(GPIOC->IDR & (1<<13)){  //If pin C13 is high
			GPIOA->ODR |= (1<<5);  //set PA5 high
		}else{
			GPIOA->ODR &= ~(1<<5); //Clear PA5 low 
		}
	}
}