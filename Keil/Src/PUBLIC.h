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
sbit RC632_NSS  = P2 ^ 0;   //芯片使能选择
sbit RC632_SCK  = P2 ^ 1;   //时钟信号
sbit RC632_MOSI = P2 ^ 2;   //输入数据
sbit RC632_MISO = P2 ^ 3;   //输出数据
sbit RC632_RST  = P2 ^ 4;   //RC500ALE
sbit RC632_CE   = P2 ^ 5;   //RC500片选

// OLED
sbit OLED_SCL  = P3 ^ 7;	// 时钟 D0（SCLK）
sbit OLED_SDIN = P3 ^ 6;	// D1（MOSI） 数据
sbit OLED_RST  = P3 ^ 5;	// 复位
sbit OLED_DC   = P3 ^ 4;	// 数据/命令控制
sbit OLED_CS   = P3 ^ 3; 	// 片选

// 蜂鸣器
sbit BEEP = P3 ^ 2;

// LED
sfr LED = 0x90; // P1

void DelayMs(uint ms);  // 延时 ms

void Delay_50us(unsigned char _50us); //us

#endif
