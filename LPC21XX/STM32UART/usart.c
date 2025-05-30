#include "stm32f4xx.h"
void UART(){
	RCC->AHB1ENR |= 1; //enable clock for GPIOA
	RCC->APB1ENR |= (1<<17); //enable pheripheral clock for uart2
	GPIOA->MODER &= ~((1<<4)|(1<<6)); //clear bit 4 and 6 for alternate function mode A2,A3
	GPIOA->MODER |= ((1<<5)|(1<<7)); //set bit 5 and 7 for alternate function mode A2,A3
	GPIOA->AFR[0] &= 0xFF00; //clear for alternate function 7 mode UART2
	GPIOA->AFR[0] |= 0x7700; //set for alternate function 7 mode UART2
	USART2->BRR = 0x0683; //Baude rate 9600, 16MHZ clk
	USART2->CR1 = 0x0008; //bit3 TX enable, bit12 8 data word length, bit15 oversample1ing 6
	USART2->CR2 = 0x0000; //bit 13:12 config 1 stop bit
	USART2->CR3 = 0x0000; //No flow control bit 11
	USART2->CR1 |= (1<<13); //UART Enable 
}

void transmit(char ch){
	while(!(USART2->SR & (1<<7)));
	USART2->DR = (ch & 0xFF); //to ensure only 8 bit data is transmited
}

char receive(){
	char data;
	while(!(USART2->SR & (1<<5)));
	data = USART2->DR;
	return data;
}

void delay(int N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<400;j++);
	}
}

int main(){
	char temp;
	UART();
	while(1){
		temp=receive();
		transmit(temp);
		delay(50);
	}
}