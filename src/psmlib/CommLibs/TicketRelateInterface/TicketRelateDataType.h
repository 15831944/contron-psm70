#ifndef __TICKETRELATEDATATYPE_H__
#define __TICKETRELATEDATATYPE_H__

typedef unsigned int        DWORD32;   //无符号四字节
typedef unsigned long long  DWORD64;   //无符号八字节，long long型，32位和64位中都为八字节

#ifdef _WIN32
typedef unsigned long       DWORD;
#else
typedef unsigned int        DWORD;
#endif
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;


//设备操作咨询请求
typedef struct _stDeviceOptAskRqt
{
	DWORD32 stationNO;		//站编号(注意与站ID区分)
	DWORD32 pointID;        //设备点号（对照表中设备点号）
	DWORD32 deviceSrcState; //设备原状态
	DWORD32 deviceDstState; //设备目标状态
}TDeviceOptAskRqt;

//设备操作咨询回复
typedef struct _stDeviceOptAskRsp
{
	DWORD32 stationNO;		//站编号(注意与站ID区分)
	DWORD32 pointID;        //设备点号（对照表中设备点号）
	DWORD32 deviceSrcState; //设备原状态
	DWORD32 deviceDstState; //设备目标状态
	DWORD32 result;         //咨询结果。0不允许操作，1允许操作
}TDeviceOptAskRsp;

//设备操作序列
typedef struct _stTRDevOptOrder
{
	char macNum[64];         //设备主编号
	DWORD32 devSrcState;     //设备原状态
	DWORD32 devDstState;     //设备目标状态
}TTRDevOptOrder;


typedef struct _stTRKBQOptResult
{
	DWORD32 stationNO;       //站编号
	DWORD32 ticketID;        //票ID
	DWORD32 devPointID;      //设备点号
	DWORD32 decSrcState;     //设备原状态
	DWORD32 devDstState;     //设备目标状态
	DWORD32 KBQDstState;     //KBQ目标状态
	DWORD32 KBQOptResult;    //KBQ操作结果。0失败，1成功
}TTRKBQOptResult;

//KBQ状态汇报，用于KBQ状态改变时主动汇报给外部
typedef struct _stTRKBQStateReport
{
	DWORD32 stationNO;       //站编号
	DWORD32 devPointID;      //设备点号
	DWORD32 isKBQShare;      //KBQ是否共享(设备对应KBQ是否分合一体，即设备分合为相同KBQ控制)
	DWORD32 HKBQState;       //设备分锁对应KBQ状态
	DWORD32 LKBQState;       //设备闭锁对应KBQ状态
}TTRKBQStateReport;

//票操作结果
typedef struct _stTicketOptResult
{
	DWORD32 stationNO;      //站ID
	DWORD32 ticketID;       //票ID
	DWORD32 operateResult;  //操作结果。0失败，1成功
	DWORD32 failedReason;   //失败原因(当操作结果为失败时该参数有效，解释失败原因)。0未知原因，1KBQ操作失败。
}TTicketOptResult;


//KBQ操作结果处理回调函数(票执行时解闭KBQ结果)
typedef int (* GCTRCallbackKBQOptResult)(TTRKBQOptResult * pKBQOptResult, void * pContext);
//票操作结果处理回调函数(针对调控产生的票反馈执行结果)
typedef int (* GCTRCallbackTicketOptResult)(TTicketOptResult msgData, void *pContext);
//KBQ状态汇报处理回调函数(用于KBQ状态改变时主动汇报给外部)
typedef int (* GCTRCallbackKBQStateReport)(TTRKBQStateReport * pMsgData, void *pContext);


#endif