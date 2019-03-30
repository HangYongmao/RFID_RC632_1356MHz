#include "PUBLIC.h"

// —” ±ms
void DelayMs(uint ms)
{
	uchar i;
	while (ms--)
	for (i = 0; i<120; i++);
}
// —” ±us
void Delay_50us(unsigned char _50us)
{
    unsigned char i;
	while(_50us--)
	for(i=0;i<25;i++);
}
