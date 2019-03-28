#ifndef __ISO15693_H__
#define __ISO15693_H__

/////////////////////////////////////////////////////////////////////
//ISO15693 COMMAND
///////////////////////////////////////////////////////////////////// 
#define	ISO15693_INVENTORY                      0x01//搜索在场的所有卡片
#define	ISO15693_STAY_QUIET                     0x02 //保持静默
#define	ISO15693_READ_SINGLE_BLOCK              0x20
#define	ISO15693_WRITE_SINGLE_BLOCK             0x21
#define	ISO15693_LOCK_BLOCK                     0x22
#define	ISO15693_READ_MULTIPLE_BLOCKS           0x23
#define	ISO15693_WRITE_MULTIPLE_BLOCKS          0x24
#define	ISO15693_SELECT                         0x25
#define	ISO15693_RESET_TO_READY                 0x26
#define	ISO15693_WRITE_AFI                      0x27
#define	ISO15693_LOCK_AFI                       0x28
#define	ISO15693_WRITE_DSFID                    0x29
#define	ISO15693_LOCK_DSFID                     0x2A
#define	ISO15693_GET_SYSTEM_INFO                0x2B
#define	ISO15693_GET_MULTIPLE_BLOCK_SECURITY    0x2C

/////////////////////////////////////////////////////////////////////
//		ISO15693 INVENTORY
/////////////////////////////////////////////////////////////////////   
char ISO15693_Inventory (unsigned char flags, unsigned char AFI, unsigned char masklengh, unsigned char *uid, unsigned char *resp);

/////////////////////////////////////////////////////////////////////
//		ISO15693 INVENTORY_16
/////////////////////////////////////////////////////////////////////                           
char ISO15693_Inventory16(unsigned char flags, unsigned char AFI, unsigned char masklengh, unsigned char *uid, unsigned char *resplen, unsigned char *resp);

#endif