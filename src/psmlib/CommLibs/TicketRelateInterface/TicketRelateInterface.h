/**************************************************************************************************
功能：票关联接口。
说明：提供给集控/调控中票关联操作。包括产生票，控制票。
作者：唐小灿  2015-05-21
**************************************************************************************************/
#ifndef __TICKETRELATEINTERFACE_H__
#define __TICKETRELATEINTERFACE_H__

#ifdef _WIN32
#ifdef TICKETRELATEDLL_EXPORTS
#define TRAPI extern "C" __declspec(dllexport)
#else
#define TRAPI extern "C" __declspec(dllimport)
#endif
#else
#define TRAPI
#endif

//非创建动态时使用
//#define TRAPI

#ifdef _WIN32
//#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define CALL_TYPE
#endif


#include "TicketRelateDataType.h"


/**************************************************************************************************
功能：初始化环境上下文
参数：无
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRInitEnvContext();


/**************************************************************************************************
功能：终止环境上下文
参数：无
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRUninitEnvContext();


/**************************************************************************************************
功能：实例化服务
参数：无
返回：NULL,失败。
	  非NULL,服务实例
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI void * CALL_TYPE GCTRInstantiateSvc();


/**************************************************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRUninstantiateSvc(void * pSvc);


/**************************************************************************************************
功能：启动服务
参数：localID，与实时库交互的本地标识，请确保不与其他实时库客户端标识冲突
      RTDBSvrIP，实时库服务IP
	  RTDBSvrPort，实时库服务消息端口
      RTDBSvrPortPbu，实时库消息发布端口号
      ticketSvcIP，票服务IP
      ticketSvcPort，票服务端口
返回：<0，失败
      >=0，成功
作者：唐小灿 2015-05-24
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRStartupSvc(void * pSvc, DWORD32 localID, const char * WFSvcIP, WORD WFSvcPort);


/**************************************************************************************************
功能：设备操作询问(用于询问当前设备是否允许操作)
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      rqtData，询问信息
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRDeviceOptAsk(void * pSvc, TDeviceOptAskRqt * pRqtData, TDeviceOptAskRsp * pRsp);


/**************************************************************************************************
功能：注册KBQ操作结果处理回调函数
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int GCTRRegisterKBQOptResult(void * pSvc, GCTRCallbackKBQOptResult callbackFun, void * pContext);


/**************************************************************************************************
功能：注册KBQ状态汇报回调函数
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-04
**************************************************************************************************/
TRAPI int GCTRRegisterKBQStateReport(void * pSvc, GCTRCallbackKBQStateReport callbackFun, void * pContext);


/**************************************************************************************************
功能：注册票操作结果回调函数
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int GCTRRegisterCallbackTicketOptResult(void * pSvc, GCTRCallbackTicketOptResult callbackFun, void * pContext);


/**************************************************************************************************
功能：创建票
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  orderList，操作序列列表
	  listCount，操作序列列表数
	  ticketID，接收新创建票ID
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRTicketCreate(void * pSvc, DWORD32 stationNo, TTRDevOptOrder orderList[], int listCount, DWORD32 & ticketID, char *szTicketName=NULL, char *szPerson=NULL);


/**************************************************************************************************
功能：开始票
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  ticketID，票ID
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRTicketStart(void * pSvc, DWORD32 stationNo, DWORD32 ticketID);


/**************************************************************************************************
功能：终止票
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  ticketID，票ID
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-05-21
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRTicketTerminate(void * pSvc, DWORD32 stationNo, DWORD32 ticketID);


/**************************************************************************************************
功能：获取指定站编号下设备与KBQ关联数
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  count，接收设备与KBQ关联数
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-04
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRGetDevKBQRelateCount(void * pSvc, DWORD32 stationNo, DWORD32 & count);


/**************************************************************************************************
功能：获取指定站编号下设备与KBQ关联
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  devKBQList，接收关联信息空间，由外部管理空间
	  listCount，devKBQList数
	  gotCount，实际获取数
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-04
**************************************************************************************************/
TRAPI int CALL_TYPE GCTRGetDevKBQRelate(void * pSvc, DWORD32 stationNo, TTRKBQStateReport devKBQList[], DWORD32 listCount, DWORD32 & gotCount);


#endif