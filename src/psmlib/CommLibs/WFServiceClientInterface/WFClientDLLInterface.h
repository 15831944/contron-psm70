/********************************************************************************************************************
功能：五防服务程序客户端对外接口函数库
说明：（1）必须按一定顺序调用
      （2）调用顺序为：GCWFInitEnvContext()-->GCWFRegisterCallbackConnectState(可选)-->GCWFStartupClientSvc()-->GCWFGetBigDataSvcCfg()或GCWFGetFilePath()
           -->GCWFShutdownSvc()-->GCWFUninstantiateClientSvc()
      （3）本库暂不确定对于多线程是否安全
作者：刘远安	2014-09-26
********************************************************************************************************************/
#ifndef WFCLIENTDLLINTERFACE_H
#define WFCLIENTDLLINTERFACE_H

//#define WFCLIENTDLL_EXPORTS

#ifdef _WIN32
#ifdef WFCLIENTDLL_EXPORTS
#define WFCAPI extern "C" __declspec(dllexport)
#else
#define WFCAPI extern "C" __declspec(dllimport)
#endif
#else
#define WFCAPI
#endif

#ifdef _WIN32
//#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define CALL_TYPE
#endif


#include "WFClientDLLDataType.h"

/*********************************************************************
功能：实例化服务
参数：localID，本地ID
      remoteIP，服务器IP
      remotePort，服务器端口号
      localPort，本地端口号
返回：NULL,失败。
      非NULL,服务实例
作者：刘远安 2014-03-17
*********************************************************************/
WFCAPI void * CALL_TYPE GCWFInitClientSvc(int localID,const char * remoteIP,WORD remotePort,WORD localPort = 0);

/*********************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由GCWFInitClientSvc返回
返回：<0,失败。
      >=0，成功
作者：刘远安 2014-03-17
*********************************************************************/
WFCAPI int CALL_TYPE GCWFUninstantiateClientSvc(void * pSvc);

/*********************************************************************
功能：启动服务
参数：pSvc，服务实例
返回：>=0 ,启动成功。
	  <0 ,启动失败。
作者：刘远安 2014-03-17
*********************************************************************/
WFCAPI int CALL_TYPE GCWFStartupClientSvc(void * pSvc);

/*********************************************************************
功能：关闭服务
参数：pSvc，需要关闭的服务实例
返回：<0,失败。
      >=0,成功
作者：刘远安 2014-03-17
*********************************************************************/
WFCAPI int CALL_TYPE GCWFShutdownSvc(void * pSvc);

/*
功能：链路连接状态变化通知回调函数
参数：connectState，连接状态，0，断开连接；1，连接正常
返回：由实际使用者确定
作者：刘远安  2014-09-15
*/
typedef int (* callbackConnectState)(DWORD32 connectState);

/*
功能：注册链路连接状态变化通知回调函数
参数：pSvc，服务实例
返回：=0，成功
      <0，失败
作者：刘远安  2014-09-15
*/
WFCAPI int CALL_TYPE GCWFRegisterCallbackConnectState(callbackConnectState callbackFunc,void * pSvc);

/*********************************************************************
功能：获取大数据服务配置
参数：pSvc，服务实例；bigDataCfg，输出参数，大数据服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetBigDataSvcCfg(void * pSvc,TBigDataCfgData *bigDataCfg);

/*********************************************************************
功能：获取实时库服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，实时库服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetRTDBSvcCfg(void * pSvc,TRTDBSvcCfgData *pOutCfgData);

/*********************************************************************
功能：获取数据库服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，数据库服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetDBSvcCfg(void * pSvc,TDBCfgData *pOutCfgData);

/*********************************************************************
功能：获取五防逻辑校核服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，五防逻辑校核服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetWFLogicCheckSvcCfg(void * pSvc,TWFLogicCheckCfgData *pOutCfgData);

/*********************************************************************
功能：获取拓扑校核服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，拓扑校核服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetTopologyCheckSvcCfg(void * pSvc,TTopologyCheckCfgData *pOutCfgData);

/*********************************************************************
功能：获取票服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，票服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetTicketSvcCfg(void * pSvc,TTicketSvcCfgData *pOutCfgData);

/*********************************************************************
功能：获取票编号服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetTicketSNSvcCfg(void * pSvc,TTicketSNDeneratorCfgData *pOutCfgData);

/*********************************************************************
功能：获取实时库客户端ID生成服务的配置
参数：pSvc，服务实例；pOutCfgData，输出参数，实时库客户端ID生成服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/

WFCAPI int CALL_TYPE GCWFGetRTDBClientIDSvcCfg(void * pSvc,TRTDBClientIDCfgData *pOutCfgData);
/*********************************************************************
功能：获取子服务的配置（目前支持【实时库数据创建服务】、【备份数据库服务】）
参数：pSvc，服务实例；pOutCfgData，输出参数，子服务配置参数
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetSubServerCfg(void * pSvc,SubServerType serverType,TSubServerCfgData *pOutCfgData);

/*********************************************************************
功能：获取五防图形客户端配置文件路径
参数：pSvc，服务实例；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetWFClientCfgPath(void * pSvc,TFileDetails *pOutPath);

/*********************************************************************
功能：获取五防图形客户端UI文件路径
参数：pSvc，服务实例；stationNo，站编号；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetWFClientUIPath(void * pSvc,DWORD32 stationNo,TFileDetails *pOutPath);

/*********************************************************************
功能：获取设备点表文件路径
参数：pSvc，服务实例；stationNo，站编号；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetPointTablePath(void * pSvc,DWORD32 stationNo,TFileDetails *pOutPath);

/*********************************************************************
功能：获取KBQ点表文件路径
参数：pSvc，服务实例；stationNo，站编号；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetKbqTablePath(void * pSvc,DWORD32 stationNo,TFileDetails *pOutPath);

/*********************************************************************
功能：获取防误校核（表达式）文件路径
参数：pSvc，服务实例；stationNo，站编号；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetWFLogicDataPath(void * pSvc,DWORD32 stationNo,TFileDetails *pOutPath);

/*********************************************************************
功能：获取钥匙打包文件路径
参数：pSvc，服务实例；stationNo，站编号；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetKeyInitDataPath(void * pSvc,DWORD32 stationNo,TFileDetails *pOutPath);

/*********************************************************************
功能：获取实时库文件路径
参数：pSvc，服务实例；pOutPath，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetRTDBDataPath(void * pSvc,TFileDetails *pOutPath);

/*********************************************************************
功能：获取服务端“应用程序数据包”路径
参数：pSvc，服务实例
      pOutFileInfo，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetDataPacketPath(void * pSvc,TFileDetails *pOutPath);

/*********************************************************************
功能：获取服务端“运行包”路径
参数：pSvc，服务实例
      pOutFileInfo，输出参数，文件或文件夹存放路径
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetRunPacketPath(void * pSvc,TFileDetails *pOutPath);


/*********************************************************************
功能：获取站数量
参数：pSvc，服务实例；stationCount，输出参数，站数量
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetStationCount(void * pSvc,DWORD32 & stationCount);

/*********************************************************************
功能：获取站编号列表
参数：pSvc，服务实例；
      pOutList，输出参数，站编号列表
	  inCount,pOutList分配的内存单元数
      outCount，输出参数，站编号数量
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetStationIDList(void * pSvc,TStationIDItem *pOutList,DWORD32 inCount,DWORD32 &outCount);

/*********************************************************************
功能：通知服务端站数据需要更新
参数：pSvc，服务实例；stationNo，站编号
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFNotifyStationUpdate(void * pSvc, DWORD32 stationNo );

/*********************************************************************
功能：获取地线控制器数量
参数：pSvc，服务实例；earthlineCtrlCount，输出参数，地线控制器数量
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetEarthlineCtrlCount(void * pSvc,DWORD32 & earthlineCtrlCount);

/*********************************************************************
功能：获取地线控制器配置列表
参数：pSvc，服务实例；
      pOutList，输出参数，地线控制器配置列表
      inCount,pOutList分配的内存单元数
      outCount，输出参数，地线控制器配置数量
返回：<0，失败
     >=0，成功
作者：刘远安  2014-09-10
*********************************************************************/
WFCAPI int CALL_TYPE GCWFGetEarthlineCtrlCfgList(void * pSvc,TEarthlineCtrlCfg *pOutList,DWORD32 inCount,DWORD32 &outCount);

#endif // BIGDATATRANSCLIENTINTERFACE_H
