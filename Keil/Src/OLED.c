// GND    电源地
// VCC  接5V或3.3v电源
// D0   P1^0（SCL）
// D1   P1^1（SDA）
// RES  接P12
// DC   接P13
// CS   接P14
#include "OLED.h"
#include "OLEDFONT.h"

void oled_delay_ms(unsigned int i)
{
	while (i>0)
	{
		i--;
	}
}

// 向SSD1306写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat, u8 cmd)
{
	u8 i;
	TR0 = 0;
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();
	for (i = 0; i<8; i++)
	{
		OLED_SCLK_Clr();
		if (dat & 0x80)
		{
			OLED_SDIN_Set();
		}
		else
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat <<= 1;
	}
	OLED_CS_Set();
	OLED_DC_Set();
	TR0 = 1;
}
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}

// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear()
{
	u8 i, n;
	for (i = 0; i<8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);	// 设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);		// 设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);		// 设置显示位置—列高地址
		for (n = 0; n<128; n++)OLED_WR_Byte(0, OLED_DATA);
	}// 更新显示
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';// 得到偏移后的值
	if (x>Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	OLED_Set_Pos(x, y);
	for (i = 0; i<8; i++)
		OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
	OLED_Set_Pos(x, y + 1);
	for (i = 0; i<8; i++)
		OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
}

// 显示一个字符号串
void OLED_ShowString(u8 x, u8 y, u8 *chr)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		if (x>120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

//显示汉字
void OLED_ShowCHinese(u8 x, u8 y, u8 no, u8 Hzk[][16])
{
	u8 t, adder = 0;
	OLED_Set_Pos(x, y);
	for (t = 0; t<16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
		adder += 1;
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t<16; t++)
	{
		OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
		adder += 1;
	}
}

// 初始化SSD1306
void OLED_Init()
{
	OLED_RST_Set();
	oled_delay_ms(1000);
	OLED_RST_Clr();
	oled_delay_ms(1000);
	OLED_RST_Set();

	OLED_WR_Byte(0xAE, OLED_CMD);// --turn off oled panel
	OLED_WR_Byte(0x00, OLED_CMD);// ---set low column address
	OLED_WR_Byte(0x10, OLED_CMD);// ---set high column address
	OLED_WR_Byte(0x40, OLED_CMD);// --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81, OLED_CMD);// --set contrast control register
	OLED_WR_Byte(0xCF, OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1, OLED_CMD);// --Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8, OLED_CMD);// Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6, OLED_CMD);// --set normal display
	OLED_WR_Byte(0xA8, OLED_CMD);// --set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f, OLED_CMD);// --1/64 duty
	OLED_WR_Byte(0xD3, OLED_CMD);// -set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD);// -not offset
	OLED_WR_Byte(0xd5, OLED_CMD);// --set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80, OLED_CMD);// --set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9, OLED_CMD);// --set pre-charge period
	OLED_WR_Byte(0xF1, OLED_CMD);// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA, OLED_CMD);// --set com pins hardware configuration
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD);// --set vcomh
	OLED_WR_Byte(0x40, OLED_CMD);// Set VCOM Deselect Level
	OLED_WR_Byte(0x20, OLED_CMD);// -Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD);// 
	OLED_WR_Byte(0x8D, OLED_CMD);// --set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD);// --set(0x10) disable
	OLED_WR_Byte(0xA4, OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6, OLED_CMD);// Disable Inverse Display On (0xa6/a7)
	OLED_WR_Byte(0xAF, OLED_CMD);// --turn on oled panel

	OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
	OLED_Clear();
	OLED_Set_Pos(0, 0);
}

// 显示电量
void showPower()
{
	OLED_ShowCHinese(96, 0, 0, PowerFont);
	OLED_ShowCHinese(112, 0, 1, PowerFont);
}

void OLED_ShowHardCodeChinese()
{
    OLED_ShowCHinese(0, 2, 0, HardCode);
    OLED_ShowCHinese(16, 2, 1, HardCode);
    OLED_ShowCHinese(32, 2, 2, HardCode);
    OLED_ShowCHinese(48, 2, 3, HardCode);
    OLED_ShowCHinese(64, 2, 4, HardCode);
}

void OLED_ShowTitleChinese()
{
    OLED_ShowCHinese(00, 2, 0, Title);
    OLED_ShowCHinese(16, 2, 1, Title);
    OLED_ShowCHinese(32, 2, 2, Title);
    OLED_ShowCHinese(48, 2, 3, Title);
    OLED_ShowString(64, 2, "13.56MHz");
    OLED_ShowCHinese(00, 4, 4, Title);
    OLED_ShowCHinese(16, 4, 5, Title);
    OLED_ShowString(32, 4, "RFID");
    OLED_ShowCHinese(64, 4, 6, Title);
    OLED_ShowCHinese(80, 4, 7, Title);
    OLED_ShowCHinese(96, 4, 8, Title);
    OLED_ShowCHinese(112, 4, 9, Title);
}