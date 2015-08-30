// WLProtocolDataType.h: .
// WL通信协议框架数据类型定义
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLPROTOCOLDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_)
#define AFX_WLPROTOCOLDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLPlatformLayer.h"

//定义状态字长度
#define SW_LEN 2                             //状态字长度为 sizeof(WORD)，两个字节

//协议通用状态字定义 90XX，FFXX，6AXX，61XX 为协议框架通用状态码，系统保留，具体协议不能占用
#define SW_SUCCESS                    0x9000 //成功
#define SW_NO_FULL_SUCCESS            0x9001 //未完全成功，部分成功，部份失败
#define SW_SUCCESSED_YOURSELF         0x9002 //成功
#define SW_NO_RSP                     0xFFFF //对方无响应  通信错误 超时
#define SW_INPUT_PARAM_ERR            0xFFFE //传入参数不合法
#define SW_FAILED_FORWARD             0xFFF0 //转发数据包失败 
#define SW_UNKNOWN_ERR                0x6A01 //未知错误  
#define SW_NO_INIT                    0x6A02 //未初始化
#define SW_INS_NOTSUOOORT             0x6A03 //命令字不支持
#define SW_CRC_ERR                    0x6A04 //检验码错误
#define SW_CMD_ERR                    0x6A05 //命令有误(命令字 和 CRC 正确，但是报文还有其它错误，如CLA错、LC错) 
#define SW_BUSY                       0x6A06 //服务忙，不处理
#define SW_BREAKFASE_ISUSE            0x6A07 //断面已被使用
#define SW_DB_CONNECT_ERR             0x6A08 //数据库未连接错误
#define SW_DB_OPERATION_ERR           0x6A09 //数据库操作错误
#define SW_RQT_REDO                   0x6100 //0x61XX 数据未传输完，估计再执行XX次可完成，请重复请求



//CLA类型
#define CLA_ENCRYPTION 0x80//DATA区加密
#define CLA_NOENCRYPT  0x00//DATA区未加密
#define CLA_MAC        0x40//DATA区有MAC
#define CLA_NOMAC      0x00//DATA区无MAC
#define CLA_RQT 0x00       //请求
#define CLA_RSP 0x08       //响应


//INS类型(注意00-0F命令框架保留，自定义协议时不允许占用)
//上位机 <--> 下位机
#define INS_COMM_DETECT                0x01 //通信检测，用于心跳
#define INS_GET_SVR_TIME               0X02 //取服务时间

//协议格式定义
#define SYNC_HEADER 0xA5A5 //通信同步头
#define SYNC_TAILER 0xA3A3 //通信同步尾

//#pragma pack (1)

//应用数据单元头
typedef struct  _APDUHeader{
	WORD  syncheader;      //命令同步头，固定为0xA5A5
	BYTE  cla;             //协议类别，见注一
	BYTE  ins;             //命令字，同一个请求与响应INS相同，由CLA判断是请求或响应，见注一
	BYTE  p1;              //参数一，默认填0。意义需参照具体命令
	BYTE  p2;              //同上
	WORD  sn;              //流水号，默认填0。绝大多数命令此项无意义，意义需参照具体命令。
	DWORD id;              //发起方唯一标识，如站号
	DWORD remoteID;        //接收方ID,默认给0（需转发时才起作用，用于路由）
	WORD  flag;            //空置（暂时只起字节对齐填充作用，将来此处可以用来放置加密算法标识等信息）
	WORD  lc;              //数据区长度:0--1000
}TAPDUHeader;

//应用数据单元尾
typedef struct _APDUTailer{
	WORD crc;              //检验字符，暂时用crc8计算出一个字节放在其低位
	WORD synctailer;       //命令同步尾，固定为0xA3A3
}TAPDUTailer;

//请求报文输入定义
typedef struct _RQTCMDIN{
	BYTE cla;
	BYTE ins;
	BYTE p1;
	BYTE p2;
	WORD sn;
	WORD lc;
	BYTE * pData;
}TRqtCmdIn;

//响应报文输入定义
typedef struct _RSPCMDIN{
	BYTE cla;
	BYTE ins;
	BYTE p1;
	BYTE p2;
	WORD sn;
	WORD lc;
	WORD sw;
	BYTE * pData;
}TRspCmdIn;

void debugProtocolWL(const char * str,BYTE * pStrem,int len,const char * ip,WORD port);

#endif // !defined(AFX_NETSVCDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_)
