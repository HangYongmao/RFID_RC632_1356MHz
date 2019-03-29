#include "PUBLIC.h"
#include <intrins.h>

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
	TH0 = (8192-4607)/32;
	TL0 = (8192-4607)%32;
	num++;
    if(num == 10)
    {
        num = 0;
        LED = _cror_(LED, 1);
    }
}
