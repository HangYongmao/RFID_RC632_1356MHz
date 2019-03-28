#include "ISO14443A.h"
#include "CLRC632.h"

#define FSD 64              //RC500 FIFO BUFFER SIZE

typedef struct TranSciveBuffer{
	unsigned char MfCommand;
	unsigned int  MfLength;
	unsigned char MfData[64];
}TranSciveBuffer;

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//         pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro
//                0x0403 = Mifare_ProX
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code, unsigned char *pTagType)
{
	char  idata status;
	TranSciveBuffer MfComData;
	TranSciveBuffer *pi;
	pi = &MfComData;

	WriteRawRC(RegChannelRedundancy, 0x03);
	ClearBitMask(RegControl, 0x08);
	WriteRawRC(RegBitFraming, 0x07);
	SetBitMask(RegTxControl, 0x03);
	PcdSetTmo(4);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 1;
	MfComData.MfData[0] = req_code;

	status = PcdComTransceive(pi);

	if (!status)
	{
		if (MfComData.MfLength != 0x10)
		{
			status = MI_BITCOUNTERR;
		}
	}
	*pTagType = MfComData.MfData[0];
	*(pTagType + 1) = MfComData.MfData[1];

	return status;
}

/////////////////////////////////////////////////////////////////////
//防冲撞
//input: g_cSNR=存放序列号(4byte)的内存单元首地址
//output:status=MI_OK:成功
//得到的序列号放入指定单元
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{

	char  idata status;
	unsigned char i;
	unsigned char ucBits;
	unsigned char ucBytes;
	unsigned char snr_check = 0;
	unsigned char ucCollPosition = 0;
	unsigned char ucTemp;
	unsigned char idata ucSNR[5] = { 0, 0, 0, 0, 0 };
	struct TranSciveBuffer MfComData;
	struct TranSciveBuffer *pi;
	pi = &MfComData;

	WriteRawRC(RegDecoderControl, 0x28);
	ClearBitMask(RegControl, 0x08);
	WriteRawRC(RegChannelRedundancy, 0x03);
	PcdSetTmo(4);
	do
	{
		ucBits = (ucCollPosition) % 8;
		if (ucBits != 0)
		{
			ucBytes = ucCollPosition / 8 + 1;
			WriteRawRC(RegBitFraming, (ucBits << 4) + ucBits);
		}
		else
		{
			ucBytes = ucCollPosition / 8;
		}

		MfComData.MfCommand = PCD_TRANSCEIVE;
		MfComData.MfData[0] = PICC_ANTICOLL1;
		MfComData.MfData[1] = 0x20 + ((ucCollPosition / 8) << 4) + (ucBits & 0x0F);
		for (i = 0; i<ucBytes; i++)
		{
			MfComData.MfData[i + 2] = ucSNR[i];
		}
		MfComData.MfLength = ucBytes + 2;

		status = PcdComTransceive(pi);

		ucTemp = ucSNR[(ucCollPosition / 8)];

		if (status == MI_COLLERR)
		{
			for (i = 0; i < 5 - (ucCollPosition / 8); i++)
			{
				ucSNR[i + (ucCollPosition / 8)] = MfComData.MfData[i + 1];
			}
			ucSNR[(ucCollPosition / 8)] |= ucTemp;
			ucCollPosition = MfComData.MfData[0];
		}

		else if (status == MI_OK)
		{
			for (i = 0; i < (MfComData.MfLength / 8); i++)
			{
				ucSNR[4 - i] = MfComData.MfData[MfComData.MfLength / 8 - i - 1];
			}
			ucSNR[(ucCollPosition / 8)] |= ucTemp;
		}


	} while (status == MI_COLLERR);

	if (status == MI_OK)
	{
		for (i = 0; i<4; i++)
		{
			*(pSnr + i) = ucSNR[i];
			snr_check ^= ucSNR[i];
		}
		if (snr_check != ucSNR[i])
		{
			status = MI_COM_ERR;
		}
	}

	ClearBitMask(RegDecoderControl, 0x20);
	return status;
}


/////////////////////////////////////////////////////////////////////
//选定一张卡
//input:g_cSNR=序列号
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr, unsigned char *pSize)
{
	unsigned char idata  i;
	char idata  status;
	unsigned char idata  snr_check = 0;
	TranSciveBuffer MfComData;
	TranSciveBuffer *pi;
	pi = &MfComData;

	WriteRawRC(RegChannelRedundancy, 0x0F);
	ClearBitMask(RegControl, 0x08);
	PcdSetTmo(4);

	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 7;
	MfComData.MfData[0] = PICC_ANTICOLL1;
	MfComData.MfData[1] = 0x70;
	for (i = 0; i<4; i++)
	{
		snr_check ^= *(pSnr + i);
		MfComData.MfData[i + 2] = *(pSnr + i);
	}
	MfComData.MfData[6] = snr_check;

	status = PcdComTransceive(pi);

	if (status == MI_OK)
	{
		if (MfComData.MfLength != 0x8)
		{
			status = MI_BITCOUNTERR;
		}
		else
		{
			*pSize = MfComData.MfData[0];
		}
	}

	return status;
}
