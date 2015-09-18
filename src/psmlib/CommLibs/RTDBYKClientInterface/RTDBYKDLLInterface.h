/********************************************************************************************************************
功能：实时库转发遥控消息对外接口函数库
说明：（1）必须按一定顺序调用
      （2）调用顺序为：GCRTDBYKInitClientSvc()-->GCRTDBYKRegisterCallbackReceiveYK(可选)-->GCRTDBYKRegisterCallbackRecvAskYKResult(可选)-->GCRTDBYKStartupClientSvc()-->
           -->GCRTDBYKShutdownSvc()-->GCRTDBYKUninstantiateClientSvc()
      （3）本库暂不确定对于多线程是否安全
	  （4）遥控分【循环式遥控】、【问答式遥控】两种，说明如下：
		   循环式遥控：当操作票有遥控操作时，票服务循环向通信前置发送遥控消息
		   问答式遥控：通信前置主动询问票服务能否监控操作某个设备，票服务给予相应解答
作者：刘远安	2014-09-26
********************************************************************************************************************/
#ifndef	RTDBYKDLLINTERFACE_H
#define RTDBYKDLLINTERFACE_H


#define RTDBYKDLL_EXPORTS

#ifdef _WIN32
#ifdef RTDBYKDLL_EXPORTS
#define RTDBYKAPI extern "C" __declspec(dllexport)
#else
#define RTDBYKAPI extern "C" __declspec(dllimport)
#endif
#else
#define RTDBYKAPI
#endif

#ifdef _WIN32
#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
//#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
//#define CALL_TYPE
#else
#define CALL_TYPE
#endif

#include "RTDBYKDLLDataType.h"

/*********************************************************************
功能：实例化服务
参数：无
返回：NULL,失败。
      非NULL,服务实例
作者：刘远安 2014-12-22
*********************************************************************/
RTDBYKAPI void * CALL_TYPE GCRTDBYKInitClientSvc();

/*********************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由GCRTDBYKInitClientSvc返回
返回：<0,失败。
      >=0，成功
作者：刘远安 2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKUninstantiateClientSvc(void * pSvc);

/*********************************************************************
功能：循环式遥控通知回调函数
参数：msgData，遥控消息；pContext，回调函数上下文指针
返回：暂无意义
作者：刘远安  2014-12-22
*********************************************************************/
typedef int (* callbackReceiveYK)(TYKMsgData msgData,void *pContext);

/*********************************************************************
功能：注册遥控通知回调函数
参数：pSvc，服务实例
	  callbackFunc，回调函数指针
	  pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackReceiveYK(void * pSvc,callbackReceiveYK callbackFunc,void *pContext);

/*********************************************************************
功能：全站设备遥控禁止消息通知回调函数
参数：msgData，遥控消息；pContext，回调函数上下文指针
返回：暂无意义
作者：刘远安  2014-12-22
*********************************************************************/
typedef int (* callbackRecvForbidYK)(TForbidAllForStationRqt msgData,void *pContext);

/*********************************************************************
功能：注册遥控禁止通知回调函数
参数：pSvc，服务实例
	  callbackFunc，回调函数指针
	  pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackRecvForbidYK(void * pSvc,callbackRecvForbidYK callbackFunc,void *pContext);

/*********************************************************************
功能：启动服务
参数：pSvc，服务实例
	  localID,实时库客户端ID号，不允许重复
	  serverIP，实时库服务端IP地址
	  serverPort，实时库服务端端口号
	  pubServerPort，实时库消息发布端口号
返回：>=0 ,启动成功。
	  <0 ,启动失败。
作者：刘远安 2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKStartupClientSvc(void * pSvc,DWORD32 localID,const char * serverIP,WORD serverPort, WORD pubServerPort);

/*********************************************************************
功能：关闭服务
参数：pSvc，需要关闭的服务实例
返回：<0,失败。
      >=0,成功
作者：刘远安 2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKShutdownSvc(void * pSvc);



/*********************************************************************
功能：遥控操作咨询结果通知回调函数
参数：msgData，遥控操作咨询结果；pContext，回调函数上下文指针
返回：暂无意义
作者：刘远安  2014-12-22
*********************************************************************/
typedef int (* callbackRecvAskYKResult)(TYKAskRsp msgData,void *pContext);

/*********************************************************************
功能：注册遥控操作咨询结果通知回调函数
参数：pSvc，服务实例
	  callbackFunc，回调函数指针
	  pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackRecvAskYKResult(void * pSvc,callbackRecvAskYKResult callbackFunc,void *pContext);

/*********************************************************************
功能：遥控操作咨询
参数：pSvc，服务实例
	  askYKData，遥控操作内容
说明：咨询的结果通过callbackRecvAskYKResult回调函数返回
返回：>=0，成功
      <0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKAskYK(void * pSvc,TYKAskRqt askYKData);


/*********************************************************************
功能：请求操作KBQ结果通知回调函数
参数：msgData，操作KBQ结果；pContext，回调函数上下文指针
返回：暂无意义
作者：刘远安  2014-12-22
*********************************************************************/
typedef int (* callbackRecvSetKBQResult)(TSetKBQRsp msgData,void *pContext);

/*********************************************************************
功能：注册操作KBQ结果通知回调函数
参数：pSvc，服务实例
      callbackFunc，回调函数指针
      pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackRecvSetKBQResult(void * pSvc,callbackRecvSetKBQResult callbackFunc,void *pContext);

/*********************************************************************
功能：请求操作KBQ
参数：pSvc，服务实例
      rqtData，请求内容
说明：操作的结果通过callbackRecvSetKBQResult回调函数返回
返回：>=0，成功
<0，失败
作者：刘远安  2014-12-22
*********************************************************************/
RTDBYKAPI int CALL_TYPE GCRTDBYKSetKBQ(void * pSvc,TSetKBQRqt rqtData);


/*********************************************************************
功能：设备操作咨询结果通知回调函数
参数：msgData，咨询结果；pContext，回调函数上下文指针
返回：暂无意义
作者：唐小灿  2015-05-18
*********************************************************************/
//typedef int (* callbackRecvDeviceOptAskResult)(TDeviceOptAskRsp msgData, void *pContext);


/*********************************************************************
功能：注册设备操作咨询结果通知回调函数
参数：pSvc，服务实例
      callbackFunc，回调函数指针
      pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：唐小灿  2015-05-15
*********************************************************************/
//RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackRecvDeviceOptAskResult(void * pSvc,callbackRecvDeviceOptAskResult callbackFunc,void *pContext);


/*********************************************************************
功能：设备操作咨询
说明：用于调控操作设备前请求票服务逻辑校核
      操作的结果通过callbackRecvDeviceOptAskResult回调函数返回
参数：pSvc，服务实例
	  rqtData，设备操作咨询内容
返回：>=0，成功
      <0，失败
作者：唐小灿  2015-05-15
*********************************************************************/
//RTDBYKAPI int CALL_TYPE GCRTDBDeviceOptAsk(void * pSvc, TDeviceOptAskRqt rqtData);


/*********************************************************************
功能：票操作结果通知回调函数
参数：msgData，票操作结果；pContext，回调函数上下文指针
返回：暂无意义
作者：唐小灿  2015-05-18
*********************************************************************/
//typedef int (* callbackRecvTicketOptResult)(TTicketOptResult msgData, void *pContext);


/*********************************************************************
功能：注册票操作结果通知回调函数
参数：pSvc，服务实例
      callbackFunc，回调函数指针
      pContext，回调函数上下文指针，允许为空
返回：>=0，成功
      <0，失败
作者：唐小灿  2015-05-15
*********************************************************************/
//RTDBYKAPI int CALL_TYPE GCRTDBYKRegisterCallbackRecvTicketOptResult(void * pSvc,callbackRecvTicketOptResult callbackFunc,void *pContext);


/*********************************************************************
功能：通知新票
说明：用于调控生成票后通知票服务
      票的执行结果通过callbackRecvTicketOptResult回调函数返回
参数：pSvc，服务实例
	  rqtData，新票内容
返回：>=0，成功
      <0，失败
作者：唐小灿  2015-05-15
*********************************************************************/
//RTDBYKAPI int CALL_TYPE GCRTDBNotifyNewTicket(void * pSvc, TSendTicketRqt rqtData);


/*********************************************************************
功能：通知终止票
说明：用于调控中途终止票
参数：pSvc，服务实例
	  rqtData，待终止票内容
返回：>=0，成功
      <0，失败
作者：唐小灿  2015-05-19
*********************************************************************/
//RTDBYKAPI int CALL_TYPE GCRTDBTerminateTicket(void * pSvc, TTerminateTicketRqt rqtData);

#endif // RTDBYKDLLINTERFACE_H
