#include "string.h"
#include "PUBLIC.h"
#include "CLRC632.h"
#include "iso15693.h" 

extern struct TranSciveBuffer{
	unsigned char MfCommand;
	unsigned int  MfLength;
	unsigned char MfData[64];
};

//所有函数的参数参见ISO15693-3/10.3
//返回数据在存入resp时去掉了flag字节
//当改写卡片上数据时（write or lock)如操作的是TI卡片设flags的bit6 Option_flag = 1，如操作的是I.CODE SLI卡?

/////////////////////////////////////////////////////////////////////
//ISO15693 INVENTORY
/////////////////////////////////////////////////////////////////////                           
char ISO15693_Inventory(unsigned char flags,
	unsigned char AFI,
	unsigned char masklengh,
	unsigned char *uid,
	unsigned char *resp)
{
	unsigned char SndCNT, cnt, status;
	struct TranSciveBuffer MfComData;
	struct TranSciveBuffer *pi;
	pi = &MfComData;

	ClearBitMask(RegCoderControl, 0x80);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfData[0] = flags;
	MfComData.MfData[1] = ISO15693_INVENTORY;
	SndCNT = 2;
	if (flags & 0x10)
	{
		MfComData.MfData[SndCNT] = AFI;
		SndCNT++;
	}
	MfComData.MfData[SndCNT] = masklengh;
	SndCNT++;
	if (masklengh % 8)
	{
		cnt = masklengh / 8 + 1;
	}
	else
	{
		cnt = masklengh / 8;
	}
	if (cnt)
	{
		memcpy(&MfComData.MfData[SndCNT], uid, cnt);
	}
	MfComData.MfLength = cnt + SndCNT;

	status = ISO15693_Transceive(pi);

	if (status == MI_OK)
	{
		if (MfComData.MfLength != 0x50)
		{
			status = MI_BITCOUNTERR;
		}
		else
		{
			memcpy(resp, &MfComData.MfData[1], 9);
		}
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//ISO15693 INVENTORY_16
/////////////////////////////////////////////////////////////////////                           
char ISO15693_Inventory16(unsigned char flags,
	unsigned char AFI,
	unsigned char masklengh,
	unsigned char *uid,
	unsigned char *resplen,
	unsigned char *resp)
{
	unsigned char idata SndCNT, cnt, status, status1, i, j;
	struct TranSciveBuffer MfComData;
	struct TranSciveBuffer *pi;
	pi = &MfComData;
	status = MI_COM_ERR;
	*resplen = 0;
	SetBitMask(RegChannelRedundancy, 0x04);
	ClearBitMask(RegCoderControl, 0x80);
	MfComData.MfCommand = PCD_TRANSMIT;
	MfComData.MfData[0] = flags;
	MfComData.MfData[1] = ISO15693_INVENTORY;
	SndCNT = 2;
	if (flags & 0x10)
	{
		MfComData.MfData[SndCNT] = AFI;
		SndCNT++;
	}
	MfComData.MfData[SndCNT] = masklengh;
	SndCNT++;
	if (masklengh % 8)
	{
		cnt = masklengh / 8 + 1;
	}
	else
	{
		cnt = masklengh / 8;
	}
	if (cnt)
	{
		memcpy(&MfComData.MfData[SndCNT], uid, cnt);
	}
	MfComData.MfLength = cnt + SndCNT;

	status1 = ISO15693_Transceive(pi);

	j = 0;

	for (i = 0; i<16; i++)
	{
		pi = &MfComData;
		ClearBitMask(RegChannelRedundancy, 0x04);
		SetBitMask(RegCoderControl, 0x80);
		SetBitMask(RegTxControl, 0x10);
		MfComData.MfCommand = PCD_TRANSCEIVE;
		MfComData.MfLength = 0;

		status1 = ISO15693_Transceive(pi);

		if ((status1 == MI_OK) && (MfComData.MfLength == 0x50))
		{
			status = MI_OK;
			*resplen += 9;
			memcpy(resp + 9 * j, &MfComData.MfData[1], 9);
			j++;
		}
		if (*resplen >= 36)
		{
			break;
		}
	}
	return status;
}
