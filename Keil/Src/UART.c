#include <reg52.h>
#include "UART.h"

// 向串口发送字符
void Putc_to_SerialPort(uchar c)
{
	SBUF = c;
	while (TI == 0);
	TI = 0;
}

// 向串口发送字符串
void Puts_to_SerialPort(uchar *s)
{
	while (*s != '\0')
	{
		Putc_to_SerialPort(*s);
		s++;
		DelayMs(5);
	}
}

// 初始化串口
void InitUART()
{
	SCON = 0x40;		//串口模式1 
	TMOD = 0x20;		//T1工作模式2
	TH1 = 0xfd;		//波特率9600
	TL1 = 0xfd;
	PCON = 0x00;		//波特率不倍增
	TI = 0;
	TR1 = 1;
	DelayMs(200);
}