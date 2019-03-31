#include "PUBLIC.h"
#include <intrins.h>

extern bit clsFlag;
uint TimerCount=0;

void InitTimer0()
{
    TMOD &= 0xf0;
    TH0 = (8192-4607)/32;
	TL0 = (8192-4607)%32;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
    
    LED = 0x7f;
}

void T0_time() interrupt 1
{
    static uchar num;
    static bit flag = 0;
	TH0 = (8192-4607)/32;
	TL0 = (8192-4607)%32;
	num++;
    if(num%10 == 0)
    {
        LED = _cror_(LED, 1);
    }
    if ((num == 200) && (flag == 0))
    {
        clsFlag = 1;
        flag = 1;
    }
    if (num == 200)
        TimerCount ++;
    if (TimerCount == 5)
        clsFlag = 1;
}
