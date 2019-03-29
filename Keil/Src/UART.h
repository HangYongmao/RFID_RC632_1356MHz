#ifndef __UART_H__
#define __UART_H__

#include "PUBLIC.h"

void Putc_to_SerialPort(uchar c);   // 向串口发送字符
void Puts_to_SerialPort(uchar *s);  // 向串口发送字符串
void InitUART();    // 初始化串口

#endif