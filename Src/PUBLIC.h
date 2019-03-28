#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <reg52.h>

#define uchar unsigned char
#define uint  unsigned int

// 端口定义
sbit RC632_CE   = P2 ^ 7;   //RC500片选
sbit RC632_RST  = P3 ^ 3;   //RC500ALE
sbit RC632_SCK  = P1 ^ 0;   //时钟信号
sbit RC632_MOSI = P1 ^ 1;   //输入数据
sbit RC632_MISO = P1 ^ 2;   //输出数据
sbit RC632_NSS  = P1 ^ 3;   //芯片使能选择

void DelayMs(uint ms);  // 延时 ms

void Delay_50us(unsigned char _50us); //us

#endif
