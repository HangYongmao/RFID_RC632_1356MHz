#include "PUBLIC.h"
#include "string.h"
#include "ISO14443B.h" 
#include "CLRC632.h"

extern unsigned char g_cFWI;

extern struct TranSciveBuffer{
	unsigned char MfCommand;
	unsigned int  MfLength;
	unsigned char MfData[64];
};

//////////////////////////////////////////////////////////////////////
//REQUEST B
//////////////////////////////////////////////////////////////////////
char M531PiccRequestB(unsigned char req_code, unsigned char AFI, unsigned char N, unsigned char *ATQB)
{
	char  status;
	struct TranSciveBuffer MfComData;
	struct TranSciveBuffer *pi;
	pi = &MfComData;
	ClearBitMask(RegControl, 0x08);
	// SetBitMask(RegTxControl, 0x03);      // Tx2RF-En, Tx1RF-En enable....

	PcdSetTmo(5);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 3;
	MfComData.MfData[0] = ISO14443B_ANTICOLLISION;     	       // APf code
	MfComData.MfData[1] = AFI;                                 // 
	MfComData.MfData[2] = ((req_code << 3) & 0x08) | (N & 0x07);     // PARAM

	status = PcdComTransceive(pi);

	if (status != MI_OK && status != MI_NOTAGERR)
	{
		status = MI_COLLERR;
	}

	if (MfComData.MfLength != 96)
	{
		status = MI_COM_ERR;
	}

	if (status == MI_OK)
	{
		memcpy(ATQB, &MfComData.MfData[0], 16);
		PcdSetTmo(ATQB[11] >> 4); // set FWT 
	}
	return status;
}

//////////////////////////////////////////////////////////////////////
//REQUEST B
//////////////////////////////////////////////////////////////////////
char Get_UID_TypeB(unsigned char *ATQB1)
{
	char  status;
	struct TranSciveBuffer MfComData;
	struct TranSciveBuffer *pi;
	pi = &MfComData;

	ClearBitMask(RegControl, 0x08);      // disable crypto 1 unit   

	// SetBitMask(RegTxControl,0x03);      // Tx2RF-En, Tx1RF-En enable

	PcdSetTmo(5);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 5;
	MfComData.MfData[0] = 0x00; //ISO14443B_ANTICOLLISION;     	       // APf code
	MfComData.MfData[1] = 0x36;// AFI;                // 
	MfComData.MfData[2] = 0x00; //((req_code<<3)&0x08) | (N&0x07);  // PARAM
	MfComData.MfData[3] = 0x00;
	MfComData.MfData[4] = 0x08;

	status = PcdComTransceive(pi);

	/*  if (status!=MI_OK && status!=MI_NOTAGERR)
	{   status = MI_COLLERR;   }

	if (MfComData.MfLength != 96)
	{   status = MI_COM_ERR;   }		   		*/

	if (status == MI_OK)
	{
		memcpy(ATQB1, &MfComData.MfData[0], 10);
		//      PcdSetTmo(ATQB[11]>>4); // set FWT 
	}
	return status;
}

char M531PiccAttrib(unsigned char *PUPI,unsigned char PARAM3,unsigned char *answer)
{
    char  status;
    struct TranSciveBuffer MfComData;
    struct TranSciveBuffer *pi;
    pi = &MfComData;
	PUPI=PUPI;
	PARAM3=PARAM3;

    MfComData.MfCommand = PCD_TRANSCEIVE;
    MfComData.MfLength  = 9;
    MfComData.MfData[0] = ISO14443B_ATTRIB;
   // memcpy(&MfComData.MfData[1], PUPI, 4);
	MfComData.MfData[1] = 0x00;
	MfComData.MfData[2] = 0x00;
	MfComData.MfData[3] = 0x00;
	MfComData.MfData[4] = 0x00;
    MfComData.MfData[5] = 0x00;  	    // EOF/SOF required, default TR0/TR1
    MfComData.MfData[6] = 0x08;//FSDI; // Max frame 64 
    MfComData.MfData[7] = 0x01;//PARAM3;  	    // Param3, ISO/IEC 14443-4 compliant?
    MfComData.MfData[8] = 0x08;//1;  	    // CID
    
    status  = PcdComTransceive(pi);

    if (status == MI_OK)
    {	
    	*answer = MfComData.MfData[0];
    } 	
    
    return status;
}