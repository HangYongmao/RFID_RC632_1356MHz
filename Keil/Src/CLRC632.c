#include "clrc632.h"
#include "ISO14443A.h"
#include "ISO14443B.h"
#include "ISO15693.h"
#include "PUBLIC.h"

#define FSD 64

extern struct TranSciveBuffer{
	unsigned char MfCommand;
	unsigned int  MfLength;
	unsigned char MfData[64];
};

/////////////////////////////////////////////////////////////////////
//复位并初始化RC632
//注意:RC500上电后应延时500ms才能可靠初始化
/////////////////////////////////////////////////////////////////////
char PcdReset()
{
	char status = MI_OK;
	char n = 0xFF;
	unsigned int i = 3000;

	RC632_CE = 0;
	RC632_RST = 0;
	DelayMs(50);
	RC632_RST = 1;
	DelayMs(5);
	RC632_RST = 0;
	RC632_CE = 0;
	DelayMs(5);

	while (i != 0 && n)
	{
		n = ReadRawRC(RegCommand);
		i--;
	}

	if (i != 0)
	{
		WriteRawRC(RegPage, 0x80);
		n = 0x80;
		while ((i != 0) && (n & 0x80))
		{
			n = ReadRawRC(RegCommand);
			i--;
		}
		if (i == 0 || (n & 0xFF))
		{
			status = MI_RESETERR;
		}
	}
	else
	{
		status = MI_RESETERR;
	}

	if (status == MI_OK)
	{
		WriteRawRC(RegPage, 0x0);
	}

	return status;
}

//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char PcdConfigISOType(unsigned char type)
{
	if (type == 'A')                    //ISO14443_A
	{
		ClearBitMask(RegControl, 0x08);

		WriteRawRC(RegClockQControl, 0x0);
		WriteRawRC(RegClockQControl, 0x40);
		Delay_50us(2);                   // wait approximately 100 us - calibration in progress
		ClearBitMask(RegClockQControl, 0x40);

		WriteRawRC(RegTxControl, 0x5b);
		WriteRawRC(RegCwConductance, 0x0F);
		WriteRawRC(RegModConductance, 0x3F);
		WriteRawRC(RegCoderControl, 0x19);
		WriteRawRC(RegModWidth, 0x13);
		WriteRawRC(RegModWidthSOF, 0x00);
		WriteRawRC(RegTypeBFraming, 0x00);

		WriteRawRC(RegRxControl1, 0x73);
		WriteRawRC(RegDecoderControl, 0x08);
		WriteRawRC(RegBitPhase, 0xAD);
		WriteRawRC(RegRxThreshold, 0xAA);
		WriteRawRC(RegBPSKDemControl, 0);
		WriteRawRC(RegRxControl2, 0x41);

		WriteRawRC(RegRxWait, 0x06);
		WriteRawRC(RegChannelRedundancy, 0x0F);
		WriteRawRC(RegCRCPresetLSB, 0x63);
		WriteRawRC(RegCRCPresetMSB, 0x63);
		WriteRawRC(RegTimeSlotPeriod, 0x00);
		WriteRawRC(RegMfOutSelect, 0x00);
		WriteRawRC(RFU27, 0x00);

		WriteRawRC(RegFIFOLevel, 0x3F);
		WriteRawRC(RegTimerClock, 0x07);
		WriteRawRC(RegTimerReload, 0x0A);
		WriteRawRC(RegTimerControl, 0x06);
		WriteRawRC(RegIRqPinConfig, 0x02);
		WriteRawRC(RFU2E, 0x00);
		WriteRawRC(RFU2F, 0x00);

		PcdSetTmo(106);
		DelayMs(1);
		PcdAntennaOn();
	}
	else if (type == 'B')
	{
		ClearBitMask(RegControl, 0x08);

		WriteRawRC(RegClockQControl, 0x0);
		WriteRawRC(RegClockQControl, 0x3F);      	//0x3f, Q时钟控制0X40
		Delay_50us(2);
		ClearBitMask(RegClockQControl, 0x40);

		WriteRawRC(RegTxControl, 0x4b);         		 //0x4b, 13.56MHz
		WriteRawRC(RegCwConductance, 0x3F);		 	 //0x3f, 设置输出驱动的电导系数0X17
		WriteRawRC(RegModConductance, 0x06);	 	     //0x06, 幅移键控ASk 12%
		WriteRawRC(RegCoderControl, 0x20);             //0x20, TypeB,波特率106kbps, NRZ编码
		WriteRawRC(RegModWidth, 0x13);
		WriteRawRC(RegModWidthSOF, 0x3F);
		WriteRawRC(RegTypeBFraming, 0x23);             //0x23, 定义ISO14443B帧格式0X3b

		WriteRawRC(RegRxControl1, 0x73);
		WriteRawRC(RegDecoderControl, 0x19);
		WriteRawRC(RegBitPhase, 0xAD);
		WriteRawRC(RegRxThreshold, 0x44);              //0x44, 可接收的最小信号强度0X88
		WriteRawRC(RegBPSKDemControl, 0x3E);           //0x3e, 忽略EOF,打开高通滤波0X7E
		WriteRawRC(RegRxControl2, 0x01);

		WriteRawRC(RegRxWait, 0x06);                  //0x06, 设置接收延时
		WriteRawRC(RegChannelRedundancy, 0x2C);       //0x2c, TypeB接收冗余校验   
		WriteRawRC(RegCRCPresetLSB, 0xFF);            //0xff, CRC预设值
		WriteRawRC(RegCRCPresetMSB, 0xFF);
		WriteRawRC(RegTimeSlotPeriod, 0x00);
		WriteRawRC(RegMfOutSelect, 0x00);
		WriteRawRC(RFU27, 0x00);

		WriteRawRC(RegFIFOLevel, 0x3F);
		WriteRawRC(RegTimerClock, 0x07);
		WriteRawRC(RegTimerReload, 0x0A);
		WriteRawRC(RegTimerControl, 0x06);
		WriteRawRC(RegIRqPinConfig, 0x02);
		WriteRawRC(RFU2E, 0x00);
		WriteRawRC(RFU2F, 0x00);
		PcdSetTmo(106);
		DelayMs(1);
		PcdAntennaOn();
	}
	else if (type == 'r')
	{
		ClearBitMask(RegControl, 0x08);

		WriteRawRC(RegClockQControl, 0x0);
		WriteRawRC(RegClockQControl, 0x40);
		Delay_50us(2);
		ClearBitMask(RegClockQControl, 0x40);

		WriteRawRC(RegTxControl, 0x4b);              //8);//?0x4b
		WriteRawRC(RegCwConductance, 0x06);
		WriteRawRC(RegModConductance, 0x03);
		WriteRawRC(RegCoderControl, 0x20);
		WriteRawRC(RegModWidth, 0x13);
		WriteRawRC(RegModWidthSOF, 0x00);
		WriteRawRC(RegTypeBFraming, 0x3B);

		WriteRawRC(RegRxControl1, 0x73);
		WriteRawRC(RegDecoderControl, 0x19);
		WriteRawRC(RegBitPhase, 0xAD);
		WriteRawRC(RegRxThreshold, 0x88);
		WriteRawRC(RegBPSKDemControl, 0x3E);
		WriteRawRC(RegRxControl2, 0x01);

		WriteRawRC(RegRxWait, 0x06);
		WriteRawRC(RegChannelRedundancy, 0x2C);
		WriteRawRC(RegCRCPresetLSB, 0xFF);
		WriteRawRC(RegCRCPresetMSB, 0xFF);
		WriteRawRC(RegTimeSlotPeriod, 0x00);
		WriteRawRC(RegMfOutSelect, 0x00);
		WriteRawRC(RFU27, 0x00);

		WriteRawRC(RegFIFOLevel, 0x1A);
		WriteRawRC(RegTimerClock, 0x07);
		WriteRawRC(RegTimerReload, 0x0A);
		WriteRawRC(RegTimerControl, 0x06);
		WriteRawRC(RegIRqPinConfig, 0x02);
		WriteRawRC(RFU2E, 0x00);
		WriteRawRC(RFU2F, 0x00);
		PcdSetTmo(106);
		DelayMs(1);
		PcdAntennaOn();
	}
	else if (type == 's')
	{
		ClearBitMask(RegControl, 0x08);

		WriteRawRC(RegClockQControl, 0x0);
		WriteRawRC(RegClockQControl, 0x40);
		Delay_50us(2);
		ClearBitMask(RegClockQControl, 0x40);

		WriteRawRC(RegTxControl, 0x4b);
		WriteRawRC(RegCwConductance, 0x06);
		WriteRawRC(RegModConductance, 0x03);
		WriteRawRC(RegCoderControl, 0x20);
		WriteRawRC(RegModWidth, 0x13);
		WriteRawRC(RegModWidthSOF, 0x00);
		WriteRawRC(RegTypeBFraming, 0x18);

		WriteRawRC(RegRxControl1, 0x73);
		WriteRawRC(RegDecoderControl, 0x19);
		WriteRawRC(RegBitPhase, 0xAD);
		WriteRawRC(RegRxThreshold, 0x88);
		WriteRawRC(RegBPSKDemControl, 0x3E);
		WriteRawRC(RegRxControl2, 0x01);
		//      WriteRawRC(RegClockQControl,0x00);//?

		WriteRawRC(RegRxWait, 0x06);
		WriteRawRC(RegChannelRedundancy, 0x2C);
		WriteRawRC(RegCRCPresetLSB, 0xFF);
		WriteRawRC(RegCRCPresetMSB, 0xFF);
		WriteRawRC(RegTimeSlotPeriod, 0x00);
		WriteRawRC(RegMfOutSelect, 0x00);
		WriteRawRC(RFU27, 0x00);

		WriteRawRC(RegFIFOLevel, 0x3F);
		WriteRawRC(RegTimerClock, 0x07);
		WriteRawRC(RegTimerReload, 0x0A);
		WriteRawRC(RegTimerControl, 0x06);
		WriteRawRC(RegIRqPinConfig, 0x02);
		WriteRawRC(RFU2E, 0x00);
		WriteRawRC(RFU2F, 0x00);
		PcdSetTmo(106);
		DelayMs(1);
		PcdAntennaOn();
	}
	else if (type == 'l')
	{
		ClearBitMask(RegControl, 0x08);

		WriteRawRC(RegTxControl, 0x48);
		WriteRawRC(RegCwConductance, 0x3F);
		WriteRawRC(RegModConductance, 0x05);       // must be measured for 15% Modulation Index
		WriteRawRC(RegCoderControl, 0x2F);         // 52.97kHz, 1-256 coding
		WriteRawRC(RegModWidth, 0x3F);             // Modulation width: 9.44us
		WriteRawRC(RegModWidthSOF, 0x3F);          // Standard mode: 9.44us
		WriteRawRC(RegTypeBFraming, 0x00);

		WriteRawRC(RegRxControl1, 0x8b);
		WriteRawRC(RegDecoderControl, 0x34);
		WriteRawRC(RegBitPhase, 0xcd);
		WriteRawRC(RegRxThreshold, 0x88);
		WriteRawRC(RegBPSKDemControl, 0);
		WriteRawRC(RegRxControl2, 0x01);
		WriteRawRC(RegClockQControl, 0x00);

		WriteRawRC(RegRxWait, 0x08);
		WriteRawRC(RegChannelRedundancy, 0x2C);    // Enable CRC TX/RX
		WriteRawRC(RegCRCPresetLSB, 0xFF);
		WriteRawRC(RegCRCPresetMSB, 0xFF);
		WriteRawRC(RegTimeSlotPeriod, 0x00);
		WriteRawRC(RegMfOutSelect, 0x00);         // enable SIGOUT = envelope
		WriteRawRC(RFU27, 0x00);

		WriteRawRC(RegFIFOLevel, 0x38);
		WriteRawRC(RegTimerClock, 0x0B);
		WriteRawRC(RegTimerReload, 0x00);
		WriteRawRC(RegTimerControl, 0x02);         // TStopRxEnd=0,TStopRxBeg=1,
		WriteRawRC(RFU2E, 0x00);
		WriteRawRC(RFU2F, 0x00);
		DelayMs(1);
		PcdAntennaOn();
	}
	else{ return -1; }
	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//读RC632寄存器位
//input:Address=寄存器地址
//ouput:读出的值
/////////////////////////////////////////////////////////////////////

//SPI接口
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char i, ucAddr;
	unsigned char ucResult = 0;
	RC632_SCK = 0;
	RC632_NSS = 0;
	ucAddr = ((Address << 1) & 0x7E) | 0x80;
	for (i = 8; i>0; i--)
	{
		RC632_MOSI = ((ucAddr & 0x80) == 0x80);
		RC632_SCK = 1;
		ucAddr <<= 1;
		RC632_SCK = 0;
	}

	for (i = 8; i>0; i--)
	{
		RC632_SCK = 1;
		ucResult <<= 1;
		ucResult |= (bit)RC632_MISO;
		RC632_SCK = 0;
	}

	RC632_NSS = 1;
	RC632_SCK = 1;
	return ucResult;
}


/////////////////////////////////////////////////////////////////////
//写RC632寄存器
//input:Address=寄存器地址
//      value=要写入的值
/////////////////////////////////////////////////////////////////////

//SPI接口
void WriteRawRC(unsigned char Address, unsigned char value)
{
	unsigned char i, ucAddr;

	RC632_SCK = 0;
	RC632_NSS = 0;
	ucAddr = ((Address << 1) & 0x7E);

	for (i = 0; i<8; i++)
	{
		RC632_MOSI = ((ucAddr & 0x80) == 0x80);
		RC632_SCK = 1;
		ucAddr <<= 1;
		RC632_SCK = 0;
	}

	for (i = 0; i<8; i++)
	{
		RC632_MOSI = ((value & 0x80) == 0x80);
		RC632_SCK = 1;
		value <<= 1;
		RC632_SCK = 0;
	}
	RC632_NSS = 1;
	RC632_SCK = 1;
}


/////////////////////////////////////////////////////////////////////
//置RC632寄存器位
//input:reg=寄存器地址
//      mask=置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg, unsigned char mask)
{
	char idata tmp = 0x00;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//清RC632寄存器位
//input:reg=寄存器地址
//      mask=清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg, unsigned char mask)
{
	char idata tmp = 0x00;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//设置RC632定时器
//input:tmolength=设置值
/////////////////////////////////////////////////////////////////////
void PcdSetTmo(unsigned char tmoLength)
{
	switch (tmoLength)
	{
	case 0:                         // (0.302 ms) FWI=0
		WriteRawRC(RegTimerClock, 0x07); // TAutoRestart=0,TPrescale=1284
		WriteRawRC(RegTimerReload, 0x21);// TReloadVal = 'h21 =33(dec) 
		break;
	case 1:                         // (0.604 ms) FWI=1
		WriteRawRC(RegTimerClock, 0x07); // TAutoRestart=0,TPrescale=128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h41 =65(dec) 
		break;
	case 2:                         // (1.208 ms) FWI=2
		WriteRawRC(RegTimerClock, 0x07); // TAutoRestart=0,TPrescale=128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 3:                         // (2.416 ms) FWI=3
		WriteRawRC(RegTimerClock, 0x09); // TAutoRestart=0,TPrescale=4*128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h41 =65(dec) 
		break;
	case 4:                         // (4.833 ms) FWI=4
		WriteRawRC(RegTimerClock, 0x09); // TAutoRestart=0,TPrescale=4*128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 5:                         // (9.666 ms) FWI=5
		WriteRawRC(RegTimerClock, 0x0B); // TAutoRestart=0,TPrescale=16*128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h41 =65(dec) 
		break;
	case 6:                         // (19.33 ms) FWI=6
		WriteRawRC(RegTimerClock, 0x0B); // TAutoRestart=0,TPrescale=16*128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 7:                         // (38.66 ms) FWI=7
		WriteRawRC(RegTimerClock, 0x0D); // TAutoRestart=0,TPrescale=64*128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h41 =65(dec) 
		break;
	case 8:                         // (77.32 ms) FWI=8
		WriteRawRC(RegTimerClock, 0x0D); // TAutoRestart=0,TPrescale=64*128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 9:                         // (154.6 ms) FWI=9
		WriteRawRC(RegTimerClock, 0x0F); // TAutoRestart=0,TPrescale=256*128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h41 =65(dec) 
		break;
	case 10:                        // (309.3 ms) FWI=10
		WriteRawRC(RegTimerClock, 0x0F); // TAutoRestart=0,TPrescale=256*128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 11:                        // (618.6 ms) FWI=11
		WriteRawRC(RegTimerClock, 0x13); // TAutoRestart=0,TPrescale=4096*128
		WriteRawRC(RegTimerReload, 0x11);// TReloadVal = 'h21 =17(dec) 
		break;
	case 12:                        // (1.2371 s) FWI=12
		WriteRawRC(RegTimerClock, 0x13); // TAutoRestart=0,TPrescale=4096*128
		WriteRawRC(RegTimerReload, 0x21);// TReloadVal = 'h41 =33(dec) 
		break;
	case 13:                        // (2.4742 s) FWI=13
		WriteRawRC(RegTimerClock, 0x13); // TAutoRestart=0,TPrescale=4096*128
		WriteRawRC(RegTimerReload, 0x41);// TReloadVal = 'h81 =65(dec) 
		break;
	case 14:                        // (4.9485 s) FWI=14
		WriteRawRC(RegTimerClock, 0x13); // TAutoRestart=0,TPrescale=4096*128
		WriteRawRC(RegTimerReload, 0x81);// TReloadVal = 'h81 =129(dec) 
		break;
	case 15:                        // (4.9485 s) FWI=15
		WriteRawRC(RegTimerClock, 0x9); // TAutoRestart=0,TPrescale=4096*128
		WriteRawRC(RegTimerReload, 0x0ff);// TReloadVal = 'h81 =129(dec) 
		break;
	default:                       // 
		WriteRawRC(RegTimerClock, 0x19); // TAutoRestart=0,TPrescale=128
		WriteRawRC(RegTimerReload, tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		break;
	}
	WriteRawRC(RegTimerControl, 0X06);
}

/////////////////////////////////////////////////////////////////////
//通过RC632和ISO14443卡通讯
//input: pi->MfCommand = RC632命令字
//       pi->MfLength  = 发送的数据长度
//       pi->MfData[]  = 发送数据
//output:status        = 错误字
//       pi->MfLength  = 接收的数据BIT长度
//       pi->MfData[]  = 接收数据
/////////////////////////////////////////////////////////////////////
char PcdComTransceive(struct TranSciveBuffer *pi)
{
	bit recebyte = 0;
	char status;
	unsigned char irqEn = 0x00;
	unsigned char waitFor = 0x00;
	unsigned char lastBits;
	unsigned char n;
	unsigned int i;
	switch (pi->MfCommand)
	{
	case PCD_IDLE:
		irqEn = 0x00;
		waitFor = 0x00;
		break;
	case PCD_WRITEE2:
		irqEn = 0x11;
		waitFor = 0x10;
		break;
	case PCD_READE2:
		irqEn = 0x07;
		waitFor = 0x04;
		recebyte = 1;
		break;
	case PCD_LOADCONFIG:
	case PCD_LOADKEYE2:
	case PCD_AUTHENT1:
		irqEn = 0x05;
		waitFor = 0x04;
		break;
	case PCD_CALCCRC:
		irqEn = 0x11;
		waitFor = 0x10;
		break;
	case PCD_AUTHENT2:
		irqEn = 0x04;
		waitFor = 0x04;
		break;
	case PCD_RECEIVE:
		irqEn = 0x06;
		waitFor = 0x04;
		recebyte = 1;
		break;
	case PCD_LOADKEY:
		irqEn = 0x05;
		waitFor = 0x04;
		break;
	case PCD_TRANSMIT:
		irqEn = 0x05;
		waitFor = 0x04;
		break;
	case PCD_TRANSCEIVE:
		irqEn = 0x3D;
		waitFor = 0x04;
		recebyte = 1;
		break;
	default:
		pi->MfCommand = MI_UNKNOWN_COMMAND;
		break;
	}

	if (pi->MfCommand != MI_UNKNOWN_COMMAND)
	{
		WriteRawRC(RegPage, 0x00);
		WriteRawRC(RegInterruptEn, 0x7F);
		WriteRawRC(RegInterruptRq, 0x7F);
		WriteRawRC(RegCommand, PCD_IDLE);
		SetBitMask(RegControl, 0x01);
		WriteRawRC(RegInterruptEn, irqEn | 0x80);
		for (i = 0; i<pi->MfLength; i++)
		{
			WriteRawRC(RegFIFOData, pi->MfData[i]);
		}
		WriteRawRC(RegCommand, pi->MfCommand);
		i = 0x3500;
		do
		{
			n = ReadRawRC(RegInterruptRq);
			i--;
		} while ((i != 0) && !(n&irqEn & 0x20) && !(n&waitFor));


		status = MI_COM_ERR;
		if ((i != 0) && !(n&irqEn & 0x20))
		{
			if (!(ReadRawRC(RegErrorFlag) & 0x17))
			{
				status = MI_OK;
				if (recebyte)
				{
					n = ReadRawRC(RegFIFOLength);
					lastBits = ReadRawRC(RegSecondaryStatus) & 0x07;
					if (lastBits)
					{
						pi->MfLength = (n - 1) * 8 + lastBits;
					}
					else
					{
						pi->MfLength = n * 8;
					}
					if (n == 0)
					{
						n = 1;
					}
					for (i = 0; i<n; i++)
					{
						pi->MfData[i] = ReadRawRC(RegFIFOData);
					}

				}
			}
			else if (ReadRawRC(RegErrorFlag) & 0x01)
			{
				status = MI_COLLERR;
				if (recebyte)
				{
					n = ReadRawRC(RegFIFOLength);
					lastBits = ReadRawRC(RegSecondaryStatus) & 0x07;
					if (lastBits)
					{
						pi->MfLength = (n - 1) * 8 + lastBits;
					}
					else
					{
						pi->MfLength = n * 8;
					}
					if (n == 0)
					{
						n = 1;
					}
					for (i = 0; i<n; i++)
					{
						pi->MfData[i + 1] = ReadRawRC(RegFIFOData);
					}
				}
				pi->MfData[0] = ReadRawRC(0x0B);
			}

		}
		else if (n & irqEn & 0x20)
		{
			status = MI_NOTAGERR;
		}
		else if (!(ReadRawRC(RegErrorFlag) & 0x17))
		{
			status = MI_ACCESSTIMEOUT;
		}
		else
		{
			status = MI_COM_ERR;
		}

		WriteRawRC(RegInterruptEn, 0x7F);
		WriteRawRC(RegInterruptRq, 0x7F);
		SetBitMask(RegControl, 0x04);           // stop timer now
		WriteRawRC(RegCommand, PCD_IDLE);
	}

	return status;
}


/////////////////////////////////////////////////////////////////////
//  通过RC632和ISO15693卡通讯
//input: pi->MfCommand = RC632命令字
//       pi->MfLength  = 发送的数据长度
//       pi->MfData[]  = 发送数据
//output:status        = 错误字
//       pi->MfLength  = 接收的数据BIT长度
//       pi->MfData[]  = 接收数据
/////////////////////////////////////////////////////////////////////
char ISO15693_Transceive(struct TranSciveBuffer *pi)
{
	bit recebyte = 0;
	char status = MI_COM_ERR;
	unsigned char n, waitFor, TimerReload;
	unsigned int i;
	switch (pi->MfCommand)
	{
	case PCD_TRANSMIT:
		waitFor = 0x10;
		break;
	default:
		waitFor = 0x28;
		recebyte = 1;
		break;
	}
	switch (pi->MfData[1])
	{
	case ISO15693_STAY_QUIET:
		TimerReload = 0x04;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_SELECT:
	case ISO15693_RESET_TO_READY:
		TimerReload = 0x0F;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_LOCK_AFI:
	case ISO15693_LOCK_DSFID:
	case ISO15693_LOCK_BLOCK:
	case ISO15693_WRITE_SINGLE_BLOCK:
	case ISO15693_WRITE_MULTIPLE_BLOCKS:
	case ISO15693_WRITE_AFI:
	case ISO15693_WRITE_DSFID:
		TimerReload = 0x29;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_READ_SINGLE_BLOCK:
		TimerReload = 0x17;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_INVENTORY:
		TimerReload = 0x1F;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_GET_SYSTEM_INFO:
		TimerReload = 0x25;    // 2048/fc => 0x01 = 151 us 
		break;
	case ISO15693_GET_MULTIPLE_BLOCK_SECURITY:
		TimerReload = 0x40;//(0x04+(cmd[cmdlen-1]+0x01))*0x02 + 0x04 + 0x01;    // 2048/fc => 0x01 = 151 us 
		break;														  // (0x04 + cmd[cmdlen - 1])*0x02 -> Time for all receivedBytes, 0x04 -> Time bevore and after response, 0x01 additional
	case ISO15693_READ_MULTIPLE_BLOCKS:
		TimerReload = 0x40;//(0x04+0x04*(cmd[cmdlen-1]+0x01))*0x02 + 0x04 + 0x01;    // 2048/fc => 0x01 = 151 us 
		break;
	default:
		TimerReload = 0x86; // 2048/fc => 0x01 = 151 us 
		break;
	}

	WriteRawRC(RegPage, 0x00);
	WriteRawRC(RegFIFOLevel, 0x1A);

	SetBitMask(RegChannelRedundancy, 0x04);
	WriteRawRC(RegTimerReload, TimerReload);
	WriteRawRC(RegTimerControl, 0x06);

	SetBitMask(RegControl, 0x01);

	WriteRawRC(RegCommand, 0x00);
	WriteRawRC(RegInterruptEn, 0x81);
	WriteRawRC(RegInterruptRq, 0x3F);
	WriteRawRC(RegInterruptEn, 0x38 | 0x80);

	for (i = 0; i<pi->MfLength; i++)
	{
		WriteRawRC(RegFIFOData, pi->MfData[i]);
	}

	WriteRawRC(RegCommand, PCD_TRANSCEIVE);	  // start to send command to label

	i = 0x3000;
	do
	{
		n = ReadRawRC(RegInterruptRq);
		i--;
	} while ((i != 0) && !(n&waitFor));

	if (!recebyte)
	{
		if (n & 0x10)
		{
			status = MI_OK;
		}
		WriteRawRC(RegInterruptEn, 0x10);
		WriteRawRC(RegTimerControl, 0x00);
		WriteRawRC(RegCommand, PCD_IDLE);
		ClearBitMask(RegCoderControl, 0x80);
		return status;
	}
	else
	{
		if ((i != 0) && (n & 0x08))
		{
			if (!(ReadRawRC(RegErrorFlag) & 0x0C))
			{
				n = ReadRawRC(RegFIFOLength);
				pi->MfLength = n * 8;
				for (i = 0; i<n; i++)
				{
					pi->MfData[i] = ReadRawRC(RegFIFOData);
				}
				if (pi->MfData[0] == 0)
				{
					status = MI_OK;
				}
			}
		}

		WriteRawRC(RegInterruptEn, 0x10);
		WriteRawRC(RegTimerControl, 0x00);
		WriteRawRC(RegCommand, PCD_IDLE);
		ClearBitMask(RegCoderControl, 0x80);

		return status;
	}
}

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
char PcdAntennaOn()
{
	unsigned char i;
	i = ReadRawRC(RegTxControl);
	if (i & 0x03)
	{
		return MI_OK;
	}
	else
	{
		SetBitMask(RegTxControl, 0x03);
		return MI_OK;
	}
}
