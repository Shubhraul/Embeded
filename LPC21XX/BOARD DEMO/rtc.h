#ifndef __RTC_H
#define	__RTC_H


#define MASKSEC 0x3f
#define MASKMIN 0x3f00
#define MASKHR 0x1f0000


void Init_RTC(void);
void read_rtc(void) __irq;
void RTC_DECtoASCII(unsigned char Data);

#endif
