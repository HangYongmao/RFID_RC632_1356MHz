#include <reg52.h>
#include "PUBLIC.h"
#include "ISO14443A.h"
#include "UART.h"
#include "string.h"
#include "CLRC632.h"

unsigned char code hardmodel[12]  = {"SL601F-0512"};
unsigned char idata g_cReceBuf[64];                         //和上位机通讯时的缓冲区

bit g_bRc632Ok;                                             //RC632复位正常标志
unsigned char data g_cSNR[4];                               //M1卡序列号

/////////////////////////////////////////////////////////////////////
//初始化RC632
/////////////////////////////////////////////////////////////////////
void Rc632Ready()
{
    char status;
    DelayMs(100);
    status = PcdReset();
    if(status != MI_OK)
    {
        DelayMs(10);
        status = PcdReset();
    } 
    if(status != MI_OK)
    {
        DelayMs(10);
        status = PcdReset();
    } 
    if(status == MI_OK)
    {
        g_bRc632Ok = 1;
    }       
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的设置RC632协议命令，ISO14443A/B或ISO15693
/////////////////////////////////////////////////////////////////////
void ComM632PcdConfigISOType()
{
	if (MI_OK == PcdConfigISOType(g_cReceBuf[6]))
	{
		// Puts_to_SerialPort("Success ComM632PcdConfigISOType...");
	}
	else
	{
		Puts_to_SerialPort("Fail to Select");
	}
}

void ComGetHardModel()//读取硬件的版本号
{
	memcpy(&g_cReceBuf[0], &hardmodel[0], sizeof(hardmodel));
	Puts_to_SerialPort(hardmodel);
}

//............................................/
//A卡的命令
//............................................/

/////////////////////////////////////////////////////////////////////
//响应上位机发送的寻A卡命令
/////////////////////////////////////////////////////////////////////
void ComRequestA()
{
	unsigned char atq[2];
	char status;
	status = PcdRequest(g_cReceBuf[6], atq);
	if (status != MI_OK)
	{
		status = PcdRequest(g_cReceBuf[6], atq);
	}
	if (status == MI_OK)
	{
		// Puts_to_SerialPort("Success ComRequestA...");
        Puts_to_SerialPort(atq);
	}
	else
	{
		// Puts_to_SerialPort("Fail ComRequestA...");
	}
}

/////////////////////////////////////////////////////////////////////
//响应上位机发送的A卡防冲撞命令
/////////////////////////////////////////////////////////////////////
void ComAnticoll()
{
	if (MI_OK == PcdAnticoll(&g_cSNR))
	{
		// Puts_to_SerialPort("Success ComAnticoll...");
        Puts_to_SerialPort(g_cSNR);
	}
	else
	{
		// Puts_to_SerialPort("Fail ComAnticoll...");
	}
}

// 主程序
void main()
{
	InitUART();
	// 向主机发送数据
	Puts_to_SerialPort("Receiving From 8051...\r\n");
    
    Rc632Ready();
    
    PcdConfigISOType('A');
	while (1)
    {
        // ComGetHardModel();
        g_cReceBuf[6]='A';
        ComM632PcdConfigISOType();
        g_cReceBuf[6]=0x52;
        ComRequestA();
        ComAnticoll();
    }
}
