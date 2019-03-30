#ifndef __OLED_H__
#define __OLED_H__

#include "PUBLIC.h"

#define  u8 unsigned char
#define  u32 unsigned int

#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

#define OLED_CS_Clr()  OLED_CS=0
#define OLED_CS_Set()  OLED_CS=1

#define OLED_RST_Clr() OLED_RST=0
#define OLED_RST_Set() OLED_RST=1

#define OLED_DC_Clr() OLED_DC=0
#define OLED_DC_Set() OLED_DC=1

#define OLED_SCLK_Clr() OLED_SCL=0
#define OLED_SCLK_Set() OLED_SCL=1

#define OLED_SDIN_Clr() OLED_SDIN=0
#define OLED_SDIN_Set() OLED_SDIN=1;

#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

void oled_delay_ms(unsigned int ms);
void showPower();
void OLED_WR_Byte(u8 dat, u8 cmd);
void OLED_Init();
void OLED_InitALLTime();
void OLED_Init_Temp();
void OLED_Clear();
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowString(u8 x, u8 y, u8 *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x, u8 y, u8 no, u8 Hzk[][16]);

void OLED_ShowHardCodeChinese();

#endif
