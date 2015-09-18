/********************************************************************************************************************
功能：实时库对外接口函数库。
说明：（1）必须按一定顺序调用。environmental context
      （2）调用顺序为：initEnvContext()-->instantiateSvc()-->startupSvc()-->getALLContainer()-->getRltIndex()/
	       setRltValue()-->shutdownSvc()-->uninstantiateSvc()-->uninitEnvContext()。
	  （3）本库暂不确定对于多线程是否安全
作者：叶文林	2013-12-26
********************************************************************************************************************/

#if !defined(__RTDB_BUSINESS_INTERFACE_H__)
#define __RTDB_BUSINESS_INTERFACE_H__

#ifdef _WIN32
#ifdef RTDBCLIENTDLL_EXPORTS
#define RTDBAPI extern "C" __declspec(dllexport)
#else
#define RTDBAPI extern "C" __declspec(dllimport)
#endif
#else
#define RTDBAPI
#endif

//#define RTDBAPI  

#include "RTDBBusinessInterfaceDatatype.h"

#ifdef _WIN32
#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
//#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define CALL_TYPE
#endif

/**************************************************************************************************
功能：初始化环境上下文
参数：无
返回：<0,失败。
	  >=0,成功
作者：叶文林 2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCInitEnvContext();

/**************************************************************************************************
功能：终止环境上下文
参数：无
返回：<0,失败。
	  >=0,成功
作者：叶文林 2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCUninitEnvContext();

/**************************************************************************************************
功能：实例化服务
参数：localID，本地ID
			 svrIP，服务器IP
			 svrPort，用于普通通信的服务器端口号
			 svrPortPbu，发布者的端口号
			 detectTimeInterval，发送心跳命令的间隔时间(毫秒)
			 totalCallCycle，总招唤全实时数据的周期(周期个心跳时间发一次总召唤)
返回：NULL,失败。
	  非NULL,服务实例
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI void * CALL_TYPE GCInstantiateSvc(DWORD32 localID,const char * svrIP,WORD svrPort, WORD svrPortPbu, DWORD32 detectTimeInterval = 1*60*1000,DWORD32 totalCallCycle = 10);

/**************************************************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由instantiateSvc返回
返回：<0,失败。
	  >=0，成功
作者：叶文林 2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCUninstantiateSvc(void * pSvc);

/**************************************************************************************************
功能：初始化实时库客户端获取服务端数据区索引信息
参数：pSvc，要启动的服务实例
返回：<0,失败。
      =0，成功
作者：唐小灿 2014-10-27
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCInitSrvDataAreaIndex(void * pSvc);

/**************************************************************************************************
功能：获取服务端数据区索引数量
参数：pSvc，要启动的服务实例
      count，输出数量参数
返回：<0,失败。
      =0，成功
作者：唐小灿 2014-10-27
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetSrvDataAreaIndexCount(void * pSvc, DWORD32 & count);

/**************************************************************************************************
功能：获取服务端数据区索引
参数：pSvc，要启动的服务实例
      index，数据区索引
      type，输出数据区类型
	  NO，输出数据区编号
返回：<0,失败。
      =0，成功
作者：唐小灿 2014-10-27
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetSrvDataAreaIndex(void * pSvc, int index, DWORD32 &type, DWORD32 &NO);

/**************************************************************************************************
功能：启动服务
参数：pSvc，要启动的服务实例
      inlist，客户端计划关注的数据区队列，权限设置不取数据时允许为NULL
	  incount，inlist队列元素个数
	  outlist，实际成功关注的数据区队列（可能比计划的要小），权限设置不取数据时允许为NULL
	  outcount，outlist队列元素个数
	  privilege，申请客户端权限
	  callback，本地实时值被远程改变时回调通知函数
	  pCallbackContext，回调函数上下文指针，作为回调函数参数值返回
	  callbackMsgDeal，中转消息处理回调
返回：>=0，成功
      <0，失败
	  0，表示部份关注的数据区不存在
	  -1，参数错误
	  -2，数据区控制头标明的个数与获得个数不同
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCStartupSvc(void * pSvc, TGCDataAreaID inlist[],DWORD32 incount,TGCDataAreaID outlist[],DWORD32 & outcount,DWORD32 privilege,TGCCallbackOnRltUpdateEx callback,void * pCallbackContext,TGCCallbaskOnMsgTransmit callbackMsgDeal);

/**************************************************************************************************
功能：关闭服务
参数：pSvc，需要关闭的服务实例
返回：<0,失败。
			 >=0,成功
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCShutdownSvc(void * pSvc);

/**************************************************************************************************
功能：取本地实时库指定数据区的所有容器
参数：pSvc，服务实例，由instantiateSvc返回
      pAreaID，数据区标识
	  list，传出参数空间由调用者申请，存放取到的容器列表
	  count，list的最大元素个数
	  gotCount，实际取到的容器列表
返回：=0，成功且取完
      <0，失败
      >0，成功但未取完，返回剩余未取数量
作者：叶文林 2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetALLContainer(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer list[],DWORD32 count,DWORD32 * gotCount);

/**************************************************************************************************
功能：取本地实时库指定数据区的实时值索引
参数：pSvc，服务实例，由instantiateSvc返回
	  pAreaID，数据区标识
	  pContainer，指定容器
	  list，传出参数，空间由调用者申请，存放取到的实时值索引列表
	  count，list的最大元素个数
	  gotCount，实际取到的实时值索引列表
返回：=0，成功且取完
      <0，失败
      >0，成功但未取完，返回剩余未取数量
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetRltIndex(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,TGCIndexRltValue list[],DWORD32 count,DWORD32 * gotCount);

/**************************************************************************************************
功能：取本地实时库指定数据区索引的实时值
参数：pSvc，服务实例，由instantiateSvc返回
			 pAreaID，数据区标识
			 pContainer，指定容器
			 valType，需要获取的实时值类型
			 index，需要获取的实时值索引
			 pVal，实际取到的实时值
返回：<0,失败：-1，参数错误；
	  >0，成功
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetRltValue(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TGCRltValue * pVal,int breakFaceNo=0);

/**************************************************************************************************
功能：取本地实时库指定数据区的实时值
参数：pSvc，服务实例，由instantiateSvc返回
      pAreaID，数据区标识
	  pContainer，指定容器
	  valType，需要获取的实时值类型
	  valList，需要获取的实时值索引，传出参数，空间由调用者申请，存放取到的实时值索引列表
	  count，valList的最大元素个数
	  gotCount，实际取到的实时值列表
返回：=0，成功且取完
      <0，失败
      >0，成功但未取完，返回剩余未取数量
作者：叶文林 2013-12-26
注释：李施施	2013-12-26
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCGetRltValueList(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,TGCRltValue valList[],DWORD32 count,DWORD32 * gotCount,int breakFaceNo=0);


/**************************************************************************************************
功能：更新本地实时库指定数据区的实时值
参数：pSvc，服务实例，由instantiateSvc返回
			 pAreaID，数据区标识
			 pContainer，指定容器
			 valType，需要更新的实时值类型
			 index，需要更新的实时值索引
			 pVal，传入参数，需要更新成的实时值
返回：<0,失败：-1，参数错误；
	  -100，已被封锁，不符合修改条件，修改失败
	  >0，成功
作者：叶文林 2013-12-26
注释：李施施 2014-06-20 加入断面认证码
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCSetValue(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TGCValue * pVal,int breakFaceNo=0, DWORD32 breakFaceAuthCode=0);


/**************************************************************************************************
功能：更新本地实时库指定数据区的实时值
参数：pSvc，服务实例，由instantiateSvc返回
	  pAreaID，数据区标识
	  pContainer，指定容器
	  valType，需要更新的实时值类型
	  index，需要更新的实时值索引
	  pRltVal，传入参数，需要更新成的实时值
	  breakFaceNo，断面号
	  breakFaceAuthCode，断面认证码
	  modifyMode，修改方式，参考ENUM_GC_RLTVAL_MODIFYMODE
返回：<0,失败：
      -1，参数错误；
	  -2，取本地值失败
	  -3，待修改值与本地值品质启用标识不一致(不允许修改)
	  -4，远程修改失败
	  -100，已被封锁，不符合修改条件，修改失败
      >=0，成功
作者：叶文林 2013-12-26
实现：唐小灿 2013-12-31
修改：李施施 2014-06-20 加入断面认证码
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCSetRltValue(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TGCRltValue * pRltVal,int breakFaceNo=0, DWORD32 breakFaceAuthCode=0, DWORD32 modifyMode=e_gc_rltval_modifymode_common);


/**************************************************************************************************
功能：锁定断面
参数：pSvc，服务实例，由instantiateSvc返回
	  pAreaID，数据区标识
	  pBreakFaceNo，传出参数，锁定的断面号
	  pBreakFaceAuthCode,断面认证码
返回：=0 成功
	  <0 失败
	  >0 断面已被锁定
作者：唐小灿 2014-05-30
修改：李施施 2014-06-30 不显示指定断面号，由服务端轮询决定空闲断面并锁定并将锁定断面号和认证码
      作为传出参数传出。
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCLockBreakFace(void * pSvc,TGCDataAreaID * pAreaID, int * pBreakFaceNo, DWORD32 * pBreakFaceAuthCode);


/**************************************************************************************************
功能：解锁断面
参数：pSvc，服务实例，由instantiateSvc返回
	  pAreaID，数据区标识
	  breakFaceNo，制定断面
	  breakFaceAuthCode,锁定断面认证码
返回：=0 成功
      <0 失败
作者：唐小灿 2014-05-30
修改：李施施 2014-06-20 加入认证码
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCUnlockBreakFace(void * pSvc,TGCDataAreaID * pAreaID, int breakFaceNo, DWORD32 breakFaceAuthCode);


/**************************************************************************************************
功能：转发消息
参数：pSvc，服务实例，由instantiateSvc返回
	  pMsgData，转发消息内容
	  len，转发消息长度
返回：=0 成功
      <0 失败
作者：唐小灿 2014-05-30
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCSendMsgTransmit(void * pSvc, void * pMsgData, int len);


/**************************************************************************************************
功能：通过innerID修改实时值
参数：pSvc，服务实例，由instantiateSvc返回
	  pAreaID，数据区标识
	  innerID，容器innerID
	  valType，实时值类型
	  index，实时值索引
	  pVal，新值，TGCValue精简结构
	  breakFaceNo，断面号
	  breakFaceAuthCode，断面认证码
	  modifyMode，修改方式，参考ENUM_GC_RLTVAL_MODIFYMODE
返回：=0 成功
      <0 失败
	  -100，已被封锁，不符合修改条件，修改失败
作者：唐小灿 2015-01-20
说明：该方法只适应PRIVILEGE_NODATA_UPDATEVAL_EX权限的操作，客户端无数据通过innerID修改值。
**************************************************************************************************/
RTDBAPI int CALL_TYPE GCSetRltValueByInnerID(void * pSvc, TGCDataAreaID * pAreaID, DWORD32 innerID, DWORD32 valType,DWORD32 index, TGCValue * pVal, int breakFaceNo=0, DWORD32 breakFaceAuthCode=0, DWORD32 modifyMode=e_gc_rltval_modifymode_common);

#endif