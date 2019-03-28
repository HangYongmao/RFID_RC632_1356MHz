#ifndef __CLRC632_H__
#define __CLRC632_H__

#include <reg52.h>

/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
//复位RC632

char PcdReset();

//设置RC632的工作方式    
char PcdConfigISOType(unsigned char type);

//读RC632-EEPROM数据
extern char PcdReadE2(unsigned int startaddr, unsigned char length, unsigned char *readdata);

//写数据到RC632-EEPROM               
extern char PcdWriteE2(unsigned int startaddr, unsigned char length, unsigned char *writedata);

//开启RC632天线发射 
char PcdAntennaOn();

//关闭RC632天线发射                                         
char PcdAntennaOff();

//写RC632寄存器
void WriteRawRC(unsigned char Address, unsigned char value);

//读RC632寄存器                     
unsigned char ReadRawRC(unsigned char Address);

//置RC632寄存器位                                
void SetBitMask(unsigned char reg, unsigned char mask);

//清RC632寄存器位                         
void ClearBitMask(unsigned char reg, unsigned char mask);

//设置RC632定时器                       
void PcdSetTmo(unsigned char tmoLength);

//ISO14443通讯函数
char PcdComTransceive(struct TranSciveBuffer *pi);

//ISO15693通讯函数              
char ISO15693_Transceive(struct TranSciveBuffer *pi);


/////////////////////////////////////////////////////////////////////
//RC632命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_WRITEE2           0x01               //写EEPROM
#define PCD_READE2            0x03               //读EEPROM
#define PCD_LOADCONFIG        0x07               //调EEPROM中保存的寄存器设置
#define PCD_LOADKEYE2         0x0B               //将EEPROM中保存的密钥调入缓存
#define PCD_AUTHENT1          0x0C               //验证密钥第一步
#define PCD_AUTHENT2          0x14               //验证密钥第二步
#define PCD_RECEIVE           0x16               //接收数据
#define PCD_LOADKEY           0x19               //传送密钥
#define PCD_TRANSMIT          0x1A               //发送数据
#define PCD_TRANSCEIVE        0x1E               //传输数据
#define PCD_RESETPHASE        0x3F               //复位
#define PCD_CALCCRC           0x12               //CRC计算

/////////////////////////////////////////////////////////////////////
//RC632 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

//clrc632相关寄存器的定义
/////////////////////////////////////////////////////////////////////
//RC632寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0//第零页的寄存器
#define     RegPage               0x00 //页面选择寄存器   
#define     RegCommand            0x01 //启动和停止执行命令   
#define     RegFIFOData           0x02  //输入与输出64位缓冲寄存器 
#define     RegPrimaryStatus      0x03  //接收，发送，fifo标志寄存器 
#define     RegFIFOLength         0x04   //FIFO缓冲区的字节长度
#define     RegSecondaryStatus    0x05   //次级状态寄存器
#define     RegInterruptEn        0x06 //启用或禁用中断控制位
#define     RegInterruptRq        0x07  //中断请求标志  
// PAGE 1      Control and Status//第一页寄存器
#define     PageSe                0x08   //选择页面寄存器 
#define     RegControl            0x09   //定时器单元的控制标志 
#define     RegErrorFlag          0x0A  //错误状态
#define     RegCollpos            0x0B  //RF检测到的第一个比特的位置
#define     RegTimerValue         0x0C//定时器的值
#define     RegCRCResultLSB       0x0D  //协处理寄存器LSB
#define     RegCRCResultMSB       0x0E  //协处理寄存器MSB
#define     RegBitFraming         0x0F //调整面向位的帧
// PAGE 2      Transmitter and Coder Control//第二页寄存器地址
#define     PageSe2               0x10  //选择页
#define     RegTxControl          0x11 //控制天线驱动器引脚
#define     RegCwConductance      0x12  //选择天线驱动器引脚电导
#define     RegModConductance     0x13 //定义驱动器的输出电导
#define     RegCoderControl       0x14  //设置时钟频率和编码
#define     RegModWidth           0x15   //选择调制脉冲宽度
#define     RegModWidthSOF        0x16   //选择SOF脉宽调制
#define     RegTypeBFraming       0x17    //定义14443B通信帧
// PAGE 3      Receiver and Decoder Control 第三页寄存器
#define     PageSe3               0x18   //选择寄存器
#define     RegRxControl1         0x19   //控制接收器的行为
#define     RegDecoderControl     0x1A   //控制解码的行为
#define     RegBitPhase           0x1B   //选择发送器与接收器之间的相位
#define     RegRxThreshold        0x1C   //为位解码器选择阈值
#define     RegBPSKDemControl     0x1D   //控制BPSK接收器的行为
#define     RegRxControl2         0x1E   //控制解码器并定义接收器输入源
#define     RegClockQControl      0x1F   //时钟控制90相位Q通道时钟
// PAGE 4      RF-Timing and Channel Redundancy 第四页的寄存器
#define     PageSe4               0x20   //页选择
#define     RegRxWait             0x21   //在接收机启动之前选择传输之后的间隙
#define     RegChannelRedundancy  0x22   //选择用于检测用于数据完整性方法和模式射频频道
#define     RegCRCPresetLSB       0x23   //预设LSB的值
#define     RegCRCPresetMSB       0x24   //预设MSB的值
#define     RegTimeSlotPeriod     0x25    //选择自动传输帧之间的时间
#define     RegMfOutSelect        0x26   //选择施加的内部信号
#define     RFU27                 0x27   //不会变得值
// PAGE 5      FIFO, Timer and IRQ-Pin Configuration 第五页寄存器
#define     PageSe5               0x28  //选择页
#define     RegFIFOLevel          0x29   //定义溢出的级别
#define     RegTimerClock         0x2A   //选择定时器的时钟分频器
#define     RegTimerControl       0x2B  //选择定时器启动和停止条件
#define     RegTimerReload        0x2C  //定义定时器的预置值
#define     RegIRqPinConfig       0x2D  //配置引脚IRQ输出级
#define     RFU2E                 0x2E  //值不变
#define     RFU2F                 0x2F  //值不变
// PAGE 6      RFU 第六页寄存器
#define     RFU31                 0x31  //留到未来用
#define     RFU32                 0x32
#define     RFU33                 0x33
#define     RFU34                 0x34
#define     RFU35                 0x35
#define     RFU36                 0x36
#define     RFU37                 0x37
// PAGE 7      Test Control
#define     RFU39                 0x39  
#define     RegTestAnaSelect      0x3A   
#define     RFU3B                 0x3B   
#define     RFU3C                 0x3C   
#define     RegTestDigiSelect     0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		          0x3F


/////////////////////////////////////////////////////////////////////
//和RC632通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_CHK_OK                      0

#define MI_NOTAGERR                    (-1)
#define MI_CHK_FAILED                  (-1)
#define MI_CRCERR                      (-2)
#define MI_CHK_COMPERR                 (-2)
#define MI_EMPTY                       (-3)
#define MI_AUTHERR                     (-4)
#define MI_PARITYERR                   (-5)
#define MI_CODEERR                     (-6)
#define MI_SERNRERR                    (-8)
#define MI_KEYERR                      (-9)
#define MI_NOTAUTHERR                  (-10)
#define MI_BITCOUNTERR                 (-11)
#define MI_BYTECOUNTERR                (-12)
#define MI_IDLE                        (-13)
#define MI_TRANSERR                    (-14)
#define MI_WRITEERR                    (-15)
#define MI_INCRERR                     (-16)
#define MI_DECRERR                     (-17)
#define MI_READERR                     (-18)
#define MI_OVFLERR                     (-19)
#define MI_POLLING                     (-20)
#define MI_FRAMINGERR                  (-21)
#define MI_ACCESSERR                   (-22)
#define MI_UNKNOWN_COMMAND             (-23)
#define MI_COLLERR                     (-24)
#define MI_RESETERR                    (-25)
#define MI_INITERR                     (-25)
#define MI_INTERFACEERR                (-26)
#define MI_ACCESSTIMEOUT               (-27)
#define MI_NOBITWISEANTICOLL           (-28)
#define MI_QUIT                        (-30)
#define MI_RECBUF_OVERFLOW             (-50)
#define MI_SENDBYTENR                  (-51)
#define MI_SENDBUF_OVERFLOW            (-53)
#define MI_BAUDRATE_NOT_SUPPORTED      (-54)
#define MI_SAME_BAUDRATE_REQUIRED      (-55)
#define MI_WRONG_PARAMETER_VALUE       (-60)
#define MI_BREAK                       (-99)
#define MI_NY_IMPLEMENTED              (-100)
#define MI_NO_MFRC                     (-101)
#define MI_MFRC_NOTAUTH                (-102)
#define MI_WRONG_DES_MODE              (-103)
#define MI_HOST_AUTH_FAILED            (-104)
#define MI_WRONG_LOAD_MODE             (-106)
#define MI_WRONG_DESKEY                (-107)
#define MI_MKLOAD_FAILED               (-108)
#define MI_FIFOERR                     (-109)
#define MI_WRONG_ADDR                  (-110)
#define MI_DESKEYLOAD_FAILED           (-111)
#define MI_WRONG_SEL_CNT               (-114)
#define MI_WRONG_TEST_MODE             (-117)
#define MI_TEST_FAILED                 (-118)
#define MI_TOC_ERROR                   (-119)
#define MI_COMM_ABORT                  (-120)
#define MI_INVALID_BASE                (-121)
#define MI_MFRC_RESET                  (-122)
#define MI_WRONG_VALUE                 (-123)
#define MI_VALERR                      (-124)
#define MI_COM_ERR                     (-125)

#endif