#include "stm32f4xx.h" 
void delay(N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<500;j++);
	}
}

int main(){
	RCC->AHB1ENR |= 1;  //Enable GPIO A Clock
	GPIOA->MODER |= (1<<10);   //Set bit 10 to set GPIOA-5 as output
	GPIOA->MODER &= ~(1<<11);  //clear bit 11 to set GPIOA-5 as output
	while(1){
		GPIOA->ODR |= (1<<5);  //set LED at PA5
		delay(50);
		GPIOA->ODR &= ~(1<<5); //Clear LED at PA5
		delay(50);
	}
}