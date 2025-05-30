#include <lpc21xx.h>
int transmit(char *ch,int N){
	int i;
	for(i=0;i<N;i++){
		while(!(U0LSR & 0x20));
		U0THR = ch[i];
	}
	return 0;
}
char receive(){
	char data;
	int i;
	while(!(U0LSR & 0x01));
	data = U0RBR;
	return data;
}
void initURT(){
	PINSEL0 = 0x00000005;
	U0LCR = 0x83;
	U0DLL = 97;
	U0LCR = 0x03; 
}
int check(char *pass){
	char key[8] = "ShubRaul";
	int i;
	for(i=0;i<8;i++){
		if(pass[i] != key[i]){
			return 0;
		}	
	}
	return 1;
}
int main(){
	int i;
	char arr[30] = "Type your 8 char Password :-  ",read[8];
	initURT();
	transmit(arr,30);
	for(i=0;i<8;i++){
		read[i] = receive();
	}
	transmit("\n\n",4);
	transmit(read,sizeof(read));
	transmit("\n\n",4);
	if(check(read)){
		transmit("Successful",10);
	}
	else{
		transmit("Unsuccessful",12);
	}
	transmit("\n\n",4);

}