/**************************************************************************************************
功能：票关联接口扩展。
说明：提供给集控/调控中票关联操作。包括产生票，控制票。
注意：GCTREXDeicevOpt使用后请调用GCTREXDeicevOptCancle来清理操作中产生的多余信息
作者：唐小灿  2015-06-03
**************************************************************************************************/
#ifndef __TICKETRELATEEXINTERFACE_H__
#define __TICKETRELATEEXINTERFACE_H__

#ifdef _WIN32
#ifdef TICKETRELATEDLLEX_EXPORTS
#define TREXAPI extern "C" __declspec(dllexport)
#else
#define TREXAPI extern "C" __declspec(dllimport)
#endif
#else
#define TREXAPI
#endif

//非创建动态时使用
//#define TREXAPI


#ifdef _WIN32
//#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define CALL_TYPE
#endif


#include "TicketRelateExDataType.h"


/**************************************************************************************************
功能：初始化环境上下文
参数：无
返回：<0,失败。
      >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXInitEnvContext();


/**************************************************************************************************
功能：终止环境上下文
参数：无
返回：<0,失败。
      >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXUninitEnvContext();


/**************************************************************************************************
功能：实例化服务
参数：无
返回：NULL,失败。
	  非NULL,服务实例
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI void * CALL_TYPE GCTREXInstantiateSvc();


/**************************************************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXUninstantiateSvc(void * pSvc);



/**************************************************************************************************
功能：启动服务
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      localID，与实时库交互的本地标识，请确保不与其他实时库客户端标识冲突
      WFSvcIP，五防服务IP
	  WFSvcPort，五防服务消息端口
返回：<0，失败
      >=0，成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXStartupSvc(void * pSvc, DWORD32 localID, const char * WFSvcIP, WORD WFSvcPort);


/**************************************************************************************************
功能：注册KBQ操作结果处理回调函数(票执行时解闭KBQ结果)
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-03
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXRegisterKBQOptResult(void * pSvc, GCTREXCallbackKBQOptResult callbackFun, void * pContext);


/**************************************************************************************************
功能：注册KBQ状态汇报回调函数(用于KBQ状态改变时主动汇报给外部)
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-04
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXRegisterKBQStateReport(void * pSvc, GCTREXCallbackKBQStateReport callbackFun, void * pContext);


/**************************************************************************************************
功能：注册回调函数，票操作结果处理(针对调控产生的票反馈执行结果)
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      callbackFun，回调函数
	  pContext，回调函数上下文。允许为空。内部不使用此参数。
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-03
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXRegisterTicketOptResult(void * pSvc, GCTREXCallbackTicketOptResult callbackFun, void * pContext);


/**************************************************************************************************
功能：设备操作询问(用于询问当前设备是否允许操作)
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      rqtData，询问信息
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXDeviceOptAsk(void * pSvc, TTREXDeviceOptAskRqt * pRqtData, TTREXDeviceOptAskRsp * pRspData);


/**************************************************************************************************
功能：设备操作
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      stationNo，站编号
	  orderList，操作序列列表
	  listCount，操作序列列表数
	  isOptAtOnce，是否立即云操作，0不立即执行，1立即执行(立即执行即P70服务根据操作序列产生票并开
	               始执行，不立即执行即P70服务根据操作序列产生票但开始执行)
	  optID，操作标识，用于标识本次操作序列操作，由服务端分配反馈，目前为票ID
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXDeicevOpt(void * pSvc, DWORD32 stationNo, TTREXDevOptOrder orderList[], int listCount, DWORD32 isOptAtOnce, DWORD32 & optID);


/**************************************************************************************************
功能：取消设备操作
参数：pSvc，服务实例，由GCTREXInstantiateSvc返回
      stationNo，站编号
	  optID，操作标识，用于标识本次操作序列操作，指定待取消的操作序列
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-02
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXDeicevOptCancle(void * pSvc, DWORD32 stationNo, DWORD32 optID);


/**************************************************************************************************
功能：获取指定站编号下设备与KBQ关联数
参数：pSvc，服务实例，由GCTRInstantiateSvc返回
      stationNo，站编号
	  count，接收设备与KBQ关联数
返回：<0,失败。
	  >=0,成功
作者：唐小灿 2015-06-04
**************************************************************************************************/
TREXAPI int CALL_TYPE GCTREXGetDevKBQRelateCount(void * pSvc, DWORD32 stationNo, DWORD32 & count);


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
TREXAPI int CALL_TYPE GCTREXGetDevKBQRelate(void * pSvc, DWORD32 stationNo, TTREXKBQStateReport devKBQList[], DWORD32 listCount, DWORD32 & gotCount);

#endif