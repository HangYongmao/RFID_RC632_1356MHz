#include <reg52.h>
#include "PUBLIC.h"
#include "UART.h"
#include "ISO14443A.h"
#include "ISO14443B.h"
#include "ISO15693.h"
#include "string.h"
#include "CLRC632.h"
#include "OLED.h"
#include "TIMER.h"

unsigned char code hardmodel[12]  = {"SL601F-0512"};    // 硬件版本号
unsigned char lastCardCode[32]  = {"SL601F-0512"}; // 上一次读取的卡号

bit g_bRc632Ok;     // RC632复位正常标志
bit clsFlag = 0;    // 清屏标志
extern uint TimerCount;
unsigned char g_cFWI;
unsigned char data g_cSNR[4];       // M1卡序列号
unsigned char idata g_cReceBuf[64]; // 和上位机通讯时的缓冲区

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
		// Puts_to_SerialPort("Fail to Select");
	}
}

//读取硬件的版本号
void ComGetHardModel()
{
	memcpy(&g_cReceBuf[0], &hardmodel[0], sizeof(hardmodel));
	Puts_to_SerialPort(hardmodel);
    
    OLED_ShowHardCodeChinese();
    OLED_ShowString(8*(16-strlen(hardmodel)), 4, hardmodel);
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
        // Puts_to_SerialPort(atq);
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
    int i=0, j=0;
    unsigned char tempData[32] = {0};   // 存贮转化后的卡号
	if (MI_OK == PcdAnticoll(&g_cSNR))
	{
        if (g_cSNR[0] == 0)
        {
            OLED_ShowString(0, 0, "Error:");
            return;
        }
		// Puts_to_SerialPort("Success ComAnticoll...");
        // 将十六进制数据转化为字符串
        for (i=0; i<4; i++)
        {
            // 高位数据
            if ((g_cSNR[i]>>4) < 10)
                tempData[j++] = (g_cSNR[i]>>4) + '0';
            else
                tempData[j++] = (g_cSNR[i]>>4) - 10 + 'A';
            
            // 低位数据
            if ((g_cSNR[i]&0xf) < 10)
                tempData[j++] = (g_cSNR[i]&0xf) + '0';
            else
                tempData[j++] = (g_cSNR[i]&0xf) - 10 + 'A';
        }
        
        if (strcmp(lastCardCode, tempData) != 0)
        {
            strcpy(lastCardCode, tempData);
            Puts_to_SerialPort(lastCardCode);
            Puts_to_SerialPort("ISO14443A:");
            Puts_to_SerialPort(tempData);
            Puts_to_SerialPort("\r\n");
                
            // 清空原有数据
            OLED_ShowString(0, 0, "          ");
            OLED_ShowString(0, 2, "                ");
                OLED_ShowString(0, 4, "                ");
            
            OLED_ShowString(0, 0, "ISO14443A:");
            OLED_ShowString(0, 2, tempData);
            
            BEEP = 0;
            DelayMs(100);
            BEEP = 1;
        }
        TimerCount = 0;
        clsFlag = 0;
	}
	else
	{
		// Puts_to_SerialPort("Fail ComAnticoll...");
	}
}

//............................................/
//B卡的命令
//............................................/
void ComTypeBRst()
{
	unsigned char status;

    int i=0, j=0;
    unsigned char tempData[32] = {0};   // 存贮转化后的卡号
	if ((status = M531PiccRequestB(g_cReceBuf[6], 0, 0, &g_cReceBuf[0])) == MI_OK)
	{
        // Puts_to_SerialPort(g_cReceBuf);
    	g_cFWI    = 0xFF;//g_cReceBuf[11] >> 4; 
    	// g_cCidNad = 8;//((g_cReceBuf[11]&0x02)<<1) | ((g_cReceBuf[11]&0x01)<<3);
       status = M531PiccAttrib(&g_cReceBuf[1], g_cReceBuf[10] & 0x0F, &g_cReceBuf[12]);
	}

	if (status == MI_OK)
	{
        memset(g_cReceBuf, 0, 64);
        if ((status = Get_UID_TypeB(&g_cReceBuf[0])) == MI_OK)
        {
            // 将十六进制数据转化为字符串
            for (i=0; g_cReceBuf[i] != 0; i++)
            {
                // 高位数据
                if ((g_cReceBuf[i]>>4) < 10)
                    tempData[j++] = (g_cReceBuf[i]>>4) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i]>>4) - 10 + 'A';
                
                // 低位数据
                if ((g_cReceBuf[i]&0xf) < 10)
                    tempData[j++] = (g_cReceBuf[i]&0xf) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i]&0xf) - 10 + 'A';
            }
            if (strcmp(lastCardCode, tempData) != 0)
            {
                strcpy(lastCardCode, tempData);
                Puts_to_SerialPort("ISO14443B:");
                Puts_to_SerialPort(tempData);
                Puts_to_SerialPort("\r\n");
                
                // 清空原有数据
                OLED_ShowString(0, 0, "          ");
                OLED_ShowString(0, 2, "                ");
                OLED_ShowString(0, 4, "                ");
                
                OLED_ShowString(0, 0, "ISO14443B:");
                OLED_ShowString(0, 2, tempData);
                
                BEEP = 0;
                DelayMs(100);
                BEEP = 1;
            }
            TimerCount = 0;
            clsFlag = 0;
        }
	}
	else
	{
		//Puts_to_SerialPort("Fail ComTypeBRst...");
	}
}

/////////////////////////////////////////////////////////////////////
//响应上位机ISO15693_Inventory命令
/////////////////////////////////////////////////////////////////////
void ComISO15693_Inventory16()
{
    int i=0, j=0;
    unsigned char tempData[32] = {0};   // 存贮转化后的卡号
	if (MI_OK == ISO15693_Inventory16(0x16, 0x00, 0x00, &g_cReceBuf[0], &g_cReceBuf[0], &g_cReceBuf[1]))
	{
        //Putc_to_SerialPort(g_cReceBuf[0]);
        for (i=0; i<g_cReceBuf[0]; i++)
        {
            if (i%2 == 0)
            {
                if ((g_cReceBuf[i+1]>>4) < 10)
                    tempData[j++] = (g_cReceBuf[i+1]>>4) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i+1]>>4) - 10 + 'A';
            }
            else
            {
                if ((g_cReceBuf[i+1]&0xf) < 10)
                    tempData[j++] = (g_cReceBuf[i+1]&0xf) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i+1]&0xf) - 10 + 'A';
            }
        }
        if (g_cReceBuf[0] != 0)
        {
            if (strcmp(lastCardCode, tempData) != 0)
            {
                strcpy(lastCardCode, tempData);
                Puts_to_SerialPort("ISO15693:");
                Puts_to_SerialPort(tempData);
                Puts_to_SerialPort("\r\n");
                
                // 清空原有数据
                OLED_ShowString(0, 0, "          ");
                OLED_ShowString(0, 2, "                ");
                OLED_ShowString(0, 4, "                ");
                
                OLED_ShowString(0, 0, "ISO15693: ");
                OLED_ShowString(0, 2, tempData);
                
                BEEP = 0;
                DelayMs(100);
                BEEP = 1;
            }
            TimerCount = 0;
            clsFlag = 0;
        }
	}
	else
	{
		//Puts_to_SerialPort("Fail ComISO15693_Inventory16...");
	}
}

/////////////////////////////////////////////////////////////////////
//响应上位机ISO15693_Inventory命令
/////////////////////////////////////////////////////////////////////
void ComISO15693_Inventory()
{
    int i=0, j=0;
    unsigned char tempData[32] = {0};   // 存贮转化后的卡号
	if (MI_OK == ISO15693_Inventory(0x36, 0x00, 0x00, &g_cReceBuf[0], &g_cReceBuf[0]))
	{
        //Putc_to_SerialPort(g_cReceBuf[0]);
        for (i=0; i<g_cReceBuf[0]; i++)
        {
            if (i%2 == 0)
            {
                if ((g_cReceBuf[i+1]>>4) < 10)
                    tempData[j++] = (g_cReceBuf[i+1]>>4) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i+1]>>4) - 10 + 'A';
            }
            else
            {
                if ((g_cReceBuf[i+1]&0xf) < 10)
                    tempData[j++] = (g_cReceBuf[i+1]&0xf) + '0';
                else
                    tempData[j++] = (g_cReceBuf[i+1]&0xf) - 10 + 'A';
            }
        }
        if (g_cReceBuf[0] != 0)
        {
            if (strcmp(lastCardCode, tempData) != 0)
            {
                strcpy(lastCardCode, tempData);
                Puts_to_SerialPort("ISO15693:");
                Puts_to_SerialPort(tempData);
                Puts_to_SerialPort("\r\n");
                
                // 清空原有数据
                OLED_ShowString(0, 0, "          ");
                OLED_ShowString(0, 2, "                ");
                OLED_ShowString(0, 4, "                ");
                
                OLED_ShowString(0, 0, "ISO15693: ");
                OLED_ShowString(0, 2, tempData);
                
                BEEP = 0;
                DelayMs(100);
                BEEP = 1;
            }
            TimerCount = 0;
            clsFlag = 0;
        }
	}
	else
	{
        //Puts_to_SerialPort("Fail ComISO15693_Inventory...");
	}
}

// 主程序
void main()
{
	InitUART();
    OLED_Init();
    InitTimer0();
    
    showPower();
    
    //while(1);
    
    Rc632Ready();
    
    PcdConfigISOType('A');
    
    ComGetHardModel();
    
    while (1)
    {
        // A卡
        g_cReceBuf[6]='A';
        ComM632PcdConfigISOType();
        g_cReceBuf[6]=0x52;
        ComRequestA();
        ComAnticoll();
        DelayMs(10);
        
        // B卡
        g_cReceBuf[6]='B';
        ComM632PcdConfigISOType();
        g_cReceBuf[6]=0x01;
        ComTypeBRst();
        DelayMs(10);
        
        // ISO15693
        g_cReceBuf[6]='l';
        ComM632PcdConfigISOType();
        ComISO15693_Inventory16();
        ComISO15693_Inventory();
        DelayMs(10);
        
        if ((clsFlag == 1) && (lastCardCode[0] != 0))
        {
            OLED_Clear();
            showPower();
            OLED_ShowTitleChinese();
            clsFlag = 0;
            lastCardCode[0] = 0;
        }
    }
}
