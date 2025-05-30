

void Buzzer_Delay(unsigned char j)		 //This function is used to generate delay events between buzzer toggling
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}

