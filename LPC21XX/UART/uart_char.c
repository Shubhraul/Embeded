#include <lpc21xx.h>
int main(){
	char data;
	PINSEL0 = 0x00050005;
	U0LCR = 0x83;
	U1LCR = 0x83;
	U0DLL = 97;
	U1DLL = 97;
	U0LCR = 0x03;
	U1LCR = 0x03;
	while(1){
	while(!(U0LSR & 0x01));
	data = U0RBR;
	while(!(U1LSR & 0x20));
	U1THR = data+1; 
	}
}