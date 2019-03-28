#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <reg52.h>

#define uchar unsigned char
#define uint  unsigned int

//指定PCD接收缓冲值
#ifndef FSDI
#define FSDI 4
#endif

// 端口定义
// RC632
sbit RC632_NSS  = P3 ^ 2;   //芯片使能选择
sbit RC632_SCK  = P3 ^ 3;   //时钟信号
sbit RC632_MOSI = P3 ^ 4;   //输入数据
sbit RC632_MISO = P3 ^ 5;   //输出数据
sbit RC632_RST  = P3 ^ 6;   //RC500ALE
sbit RC632_CE   = P3 ^ 7;   //RC500片选

// OLED
sbit OLED_SCL  = P2 ^ 4;	// 时钟 D0（SCLK）
sbit OLED_SDIN = P2 ^ 3;	// D1（MOSI） 数据
sbit OLED_RST  = P2 ^ 2;	// 复位
sbit OLED_DC   = P2 ^ 1;	// 数据/命令控制
sbit OLED_CS   = P2 ^ 0; 	// 片选

// 蜂鸣器
sbit BEEP = P2 ^ 5;

// LED
sfr LED = 0x90; // P1

void DelayMs(uint ms);  // 延时 ms

void Delay_50us(unsigned char _50us); //us

#endif
