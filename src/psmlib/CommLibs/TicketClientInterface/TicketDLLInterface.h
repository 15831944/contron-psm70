/********************************************************************************************************************
功能：票服务提供对外DLL接口，用于图形与票服务及图形与票执行就地程序操作接口。功能包括图形往外发票，图形接收票服务
      票(票动作)完成处理，票执行(就地)程序给图形的通知(通知包括：可以下发到钥匙的票，票执行情况)
说明：
作者：唐小灿	2014-09-12
********************************************************************************************************************/
#ifndef __TICKETINTERFACE_H__
#define __TICKETINTERFACE_H__

#ifdef _WIN32
#ifdef TICKETINTERFACE_EXPORTS
#define TICKETAPI extern "C" __declspec(dllexport)
#else
#define TICKETAPI extern "C" __declspec(dllimport)
#endif
#else
#define TICKETAPI
#endif


#include "./ChartTicketServDataType.h"


//票转换操作类型状态
enum ENUM_TICKET_CONVERT_OPERATETYPE_STATE
{
	e_tcots_yk2jd = 0,  //遥控转就地
	e_tcots_jd2yk = 1,  //就地转遥控
};



/********************************************************************************************************************
发票处理接口 BEGIN
********************************************************************************************************************/
/*
功能：创建发票对象
参数：localID，本端ID
      remoteIP，远端IP
	  remotePort，远端端口
返回：NULL，失败
      !NULL,成功
作者：唐小灿  2014-09-10
*/
TICKETAPI void * inter_createSendObject(int localID,const char * remoteIP,unsigned short remotePort);


/*
功能：销毁发票对象
参数：pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-10
*/
TICKETAPI int inter_destorySendObject(void * pSvc);

/*
功能：与钥匙服务通讯情况回调
参数：connectState，连接状态，0，断开连接；1，连接正常
返回：由实际使用者确定
作者：刘远安  2014-09-15
*/
typedef int (* callbackConnectState)(DWORD32 connectState);

/*
功能：注册与钥匙服务通讯情况回调函数
参数：pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_registerCallbackConnectState(callbackConnectState callbackFunc,void * pSvc);

/*
功能：发票
参数：ticketID，票ID
      pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-10
*/
TICKETAPI int inter_sendTicket(DWORD32 ticketID, void * pSvc);


/*
功能：发送票遥控/就地转换消息
参数：ticketID，票ID
      state，状态，参考ENUM_TICKET_CONVERT_OPERATETYPE_STATE
返回：=0，成功
      <0，失败
作者：唐小灿  2014-11-14
*/
TICKETAPI int inter_convertTicketOperateType(DWORD32 ticketID, DWORD32 state, void * pSvc);


/*
功能：回传票
参数：ticketID，票ID
      pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
说明：说明：图形给票执行(就地)程序发送的消息，其他地方暂未使用该消息
作者：唐小灿  2014-09-15
*/
TICKETAPI int inter_rebackTicket(DWORD32 ticketID, void * pSvc);


/*
功能：通知远端给钥匙传票
参数：ticketID，票ID
      pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
说明：图形给票执行(就地)程序发送的消息，其他地方暂未使用该消息
作者：唐小灿  2014-09-15
*/
TICKETAPI int inter_sendTicketToKey(DWORD32 ticketID, void * pSvc);

/*
功能：查询钥匙在线情况
参数：无
返回：>=0，成功
      <0，失败
说明：图形给票执行(就地)程序发送的消息，其他地方暂未使用该消息
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_checkKeyOnline(void * pSvc);

/*
功能：查询可以下发到钥匙的操作票
参数：无
返回：>=0，成功
      <0，失败
说明：图形给票执行(就地)程序发送的消息，其他地方暂未使用该消息
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_checkTicketCanSendToKey(void * pSvc);


/*
功能：通知钥匙服务程序下发RFID到钥匙
参数：stationID，站编号
      pSvc，由inter_createSendObject返回的指针
返回：=0，成功
      <0，失败
说明：图形给票执行(就地)程序发送的消息，其他地方暂未使用该消息
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_sendRFIDToKey(DWORD32 stationID, void * pSvc);


/*
功能：回复票服务 请求票项确认
参数：stationID，站ID
      ticketID，票ID
	  ticketActID，票动作ID
	  ticketItemID，票项ID
	  state，状态，0未确认，1确认
说明：图形客户端给票服务回复消息，针对票项中部分需要客户端确认
作者：唐小灿  2015-03-17
*/
TICKETAPI int inter_rspRequestConfirmItem(void * pSvc, DWORD32 stationID, DWORD32 ticketID, DWORD32 ticketActID, DWORD32 ticketItemID, DWORD32 state);
/********************************************************************************************************************
发票处理接口 END
********************************************************************************************************************/









/********************************************************************************************************************
票及票动作完成处理接口 BEGIN
********************************************************************************************************************/
/*
功能：完成票或票动作回调
参数：ticketID，票ID
      ticketActID，票动作ID
	  isTicketFinish，票完成标识，0票未完成(票动作完成)，1票完成。用于区分票动作完成/票完成
返回：由实际使用者确定
作者：唐小灿  2014-09-10
*/
typedef int (* callbackFinishTicketOrAct)(DWORD32 ticketID, DWORD32 ticketActID, int isTicketFinish);


/*
功能：创建实时库客户端操作对象
参数：localID，本端ID
      RTDBSvcIP，实时库服务端IP
	  RTDBSvcPort，实时库服务端端口
	  RTDBSvcPortPbu，实时库服务端发布端口
返回：NULL，失败
      !NULL,成功
作者：唐小灿  2014-09-10
*/
TICKETAPI void * inter_createRTDBClientObject(DWORD32 localID, const char * RTDBSvcIP, WORD RTDBSvcPort, WORD RTDBSvcPortPbu, callbackFinishTicketOrAct callback);


/*
功能：销毁实时库客户端操作对象
参数：pSvc，由inter_createRTDBClientObject返回的指针
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-10
*/
TICKETAPI int inter_destoryRTDBClientObject(void * pSvc);
/********************************************************************************************************************
票及票动作完成处理接口 END
********************************************************************************************************************/











/********************************************************************************************************************
图形接收票执行(就地)程序通知接口 BEGIN
********************************************************************************************************************/
/*
功能：可下发到钥匙的票列表回调
参数：ticketCount，票数量
      ticketIDList，票ID列表
返回：由实际使用者确定
作者：唐小灿  2014-09-10
*/
typedef int (* callbackToKeyTicketList)(int ticketCount, DWORD32 ticketIDList[]);


/*
功能：票结果回调
参数：resultCount，票结果数量
      reslutList，票结果列表
返回：由实际使用者确定
作者：唐小灿  2014-09-15
*/
typedef int (* callbackTicketResultList)(int resultCount, TTicketResult reslutList[]);


/*
功能：创建图形接收通知对象(票执行本地操作程序所发送通知)
参数：localID，本端ID
      localPort，本端端口
	  callbackToKeyTicket，可发送到钥匙的票列表消息处理回调
返回：NULL，失败
      !NULL,成功
作者：唐小灿  2014-09-15
*/
TICKETAPI void * inter_createChartRecvNoticeObject(int localID,WORD localPort, callbackToKeyTicketList callbackToKeyTicket, callbackTicketResultList callbackResult);

/*
功能：销毁图形接收票执行就地操作程序消息类对象
参数：pSvc，由inter_createChartRecvNoticeObject返回的指针
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-15
*/
TICKETAPI int inter_destoryChartRecvNoticeObject(void * pSvc);

/*
功能：钥匙执行命令结果回调（下发钥匙票、钥匙票回传等是否成功）
参数：execResult，命令执行结果
返回：由实际使用者确定
作者：刘远安  2014-09-15
*/
typedef int (* callbackKeyExecResult)(TKeyExecResult execResult);

/*
功能：注册钥匙执行命令结果回调函数
参数：pSvc，由inter_createChartRecvNoticeObject返回的指针
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_registerCallbackKeyExecResult(callbackKeyExecResult callbackFunc,void * pSvc);

/*
功能：钥匙在线情况回调
参数：execResult，命令执行结果
返回：由实际使用者确定
应用场景：1.图形下发查询钥匙在位命令后通过该回调函数汇报查询内容
		  2.钥匙在线情况变化后通过该回调函数主动通知图形
作者：刘远安  2014-09-15
*/
typedef int (* callbackKeyOnlineState)(TKeyOnlineState keyOnlineState);

/*
功能：注册钥匙在线情况回调函数
参数：pSvc，由inter_createChartRecvNoticeObject返回的指针
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
TICKETAPI int inter_registerCallbackKeyOnlineState(callbackKeyOnlineState callbackFunc,void * pSvc);




/********************************************************************************************************************
图形接收票执行(就地)程序通知接口 END
********************************************************************************************************************/











/********************************************************************************************************************
票执行(就地)程序给图形的通知接口 BEGIN
********************************************************************************************************************/
/*
功能：创建票执行(就地)程序通知对象(通知发往图形端)
参数：localID，本端ID
      remoteIP，远端(图形端)IP
	  remotePort，远端(图形端)端口
	  localPort，本端ID
返回：NULL，失败
      !NULL,成功
作者：唐小灿  2014-09-15
*/
TICKETAPI void * inter_createNativeNoticeToChartObject(int localID,const char * remoteIP,WORD remotePort,WORD localPort);


/*
功能：销毁票执行(就地)程序通知对象
参数：pSvc，由inter_createNativeNoticeToChartObject返回的指针
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-15
*/
TICKETAPI int inter_destoryNativeNoticeToChartObject(void * pSvc);


/*
功能：发送消息 可传送给钥匙票ID列表
参数：count，票数
      ticketIDList，票ID列表
      pSvc，由inter_createNativeNoticeToChartObject创建的对象
返回：=0，成功
      <0，失败
	  -1，参数错误
	  -2，未接收到反馈
作者：唐小灿  2014-09-15
*/
TICKETAPI int NoticeToKeyTicketList(int count, DWORD32 ticketIDList[], void * pSvc);


/*
功能：发送 票结果 通知
参数：count，票结果数
      resultList，票结果列表
返回：=0，成功
      <0，失败
作者：唐小灿  2014-09-15
*/
TICKETAPI int NoticeTicketResultList(int count, TTicketResult resultList[], void * pSvc);

/*
功能：发送 钥匙执行命令结果 通知
参数：execResult，钥匙执行命令结果
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
TICKETAPI int NoticeKeyExecResult(TKeyExecResult execResult, void * pSvc);

/*
功能：发送 钥匙在线情况 通知
参数：execResult，钥匙执行命令结果
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
TICKETAPI int NoticeKeyOnlineState(TKeyOnlineState keyOnlineState, void * pSvc);

/********************************************************************************************************************
票执行(就地)程序给图形的通知接口 END
********************************************************************************************************************/
#endif
