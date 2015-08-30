#ifndef RTDBYKDLLDATATYPE_H
#define RTDBYKDLLDATATYPE_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int        DWORD32;   //无符号四字节
typedef unsigned long long  DWORD64;   //无符号八字节，long long型，32位和64位中都为八字节

#ifdef _WIN32
typedef unsigned long       DWORD;
#else
typedef unsigned int DWORD;
#endif
//typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef int                 INT;
//typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

//单个设备遥控信息
typedef struct _YKMsgData
{
	DWORD32 msgType;		//消息类型：1，共创遥控规约；2，遥控选择；3，遥控执行；4，遥控撤销
	DWORD32 stationID;		//站编号
	DWORD32 pointID;		//设备点号
	DWORD32 state;			//遥控操作：0，分；1，合；0xFF，遥控禁止
}TYKMsgData;

//全站设备遥控禁止消息
typedef struct _stForbidAllForStationRqt
{
	DWORD32 staionNo;       //站编号
}TForbidAllForStationRqt;

//遥控操作咨询(外部询问票服务设备是否允许遥控)
//问答式遥控操作咨询请求
typedef struct _stYKAskRqt
{
	DWORD32 stationID;		//站编号
	DWORD32 ykNo;			//遥控点号（注意：这是实时库设备点表中的设备点号）
	DWORD32 state;          //设备目的状态
	DWORD32 type;           //请求类型，0遥控选择，1遥控执行
}TYKAskRqt;

//遥控操作咨询回复
//问答式遥控操作咨询回复
typedef struct _stYKAskRsp
{
	DWORD32 stationID;		//站编号
	DWORD32 ykNo;			//遥控点号（注意：这是实时库设备点表中的设备点号）
	DWORD32 state;          //设备目的状态
	DWORD32 type;           //请求类型，0遥控选择，1遥控执行
    DWORD32 isTicketOpt;    //是否有票正在操作对应设备:0，当前没有执行中的操作票；1，当前有执行中的操作票，并且当前执行步骤与请求一致；2，当前有执行中的操作票，但是当前执行步骤与请求不一致
    DWORD32 allowYKResult;  //是否允许遥控结果：0，允许遥控；1，不允许遥控（KBQ解锁失败）；2，不允许遥控（五防校核不通过）；3，不允许遥控（逻辑校验缺少前序动作判断）
}TYKAskRsp;

//设置KBQ请求
typedef struct _stSetKBQRqt
{
    DWORD32 stationID;      //站编号
    DWORD32 pointID;        //设备点号（注意：这是实时库设备点表中的设备点号）
	DWORD32 deviceSrcState; //设备原状态
    DWORD32 deviceDstState; //设备目标状态
    DWORD32 KBQDstState;    //KBQ目标状态
}TSetKBQRqt;

//设置KBQ回复
typedef struct _stSetKBQRsp
{
    DWORD32 stationID;      //站编号
    DWORD32 pointID;        //设备点号（注意：这是实时库设备点表中的设备点号）
	DWORD32 deviceSrcState; //设备原状态
    DWORD32 deviceDstState; //设备目标状态
    DWORD32 KBQDstState;    //KBQ目标状态
    DWORD32 result;         //KBQ设置结果。0失败，1成功
}TSetKBQRsp;

//设备操作咨询请求
//typedef struct _stDeviceOptAskRqt
//{
//	DWORD32 stationID;		//站ID
//	DWORD32 pointID;        //设备点号（对照表中设备点号）
//	DWORD32 deviceSrcState; //设备原状态
//	DWORD32 deviceDstState; //设备目标状态
//}TDeviceOptAskRqt;

//设备操作咨询回复
//typedef struct _stDeviceOptAskRsp
//{
//	DWORD32 stationID;		//站ID
//	DWORD32 pointID;        //设备点号（对照表中设备点号）
//	DWORD32 deviceSrcState; //设备原状态
//	DWORD32 deviceDstState; //设备目标状态
//	DWORD32 result;         //咨询结果。0不允许操作，1允许操作
//}TDeviceOptAskRsp;

//通知新票
//typedef struct _stSendTicketRqt
//{
//	DWORD32 stationID;  //暂时不启用，等客户端启用后开启
//	DWORD32 ticketID;
//}TSendTicketRqt;

//票操作结果
//typedef struct _stTicketOptResult
//{
//	DWORD32 stationID;      //站ID
//	DWORD32 ticketID;       //票ID
//	DWORD32 operateResult;  //操作结果。0失败，1成功
//	DWORD32 failedReason;   //失败原因(当操作结果为失败时该参数有效，解释失败原因)。0未知原因，1KBQ操作失败。
//}TTicketOptResult;

//终止票
//typedef struct _stTerminateTicketRqt
//{
//	DWORD32 ticketID;       //票ID
//}TTerminateTicketRqt;


#endif
