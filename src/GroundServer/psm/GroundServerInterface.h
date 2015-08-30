/********************************************************************************************************************
功能：地线服务通讯模块相关接口
说明：（1）必须按一定顺序调用
作者：韦树远	2014-09-26
********************************************************************************************************************/
#ifndef	GROUNDSERVERINTERFACE_H
#define GROUNDSERVERINTERFACE_H

//#define GROUNDSERVER_EXPORTS

#ifdef _WIN32
#ifdef GROUNDSERVER_EXPORTS
#define GROUNDSERVERAPI extern "C" __declspec(dllexport)
#else
#define GROUNDSERVERAPI extern "C" __declspec(dllimport)
#endif
#else
#define GROUNDSERVERAPI
#endif

#ifdef _WIN32
//#define GROUNDCALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
#define GROUNDCALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define GROUNDCALL_TYPE
#endif

typedef struct _GroundServerConfig
{
	unsigned int stationNO;		//站号
	unsigned int controllerNO;   //控制器号
	unsigned int stakeNO;        //桩号
	unsigned char label[16];     //地线标签
}TGroundServerPointConfig;

//地线控制器配置参数
typedef struct _GroundServerControllerConfig
{
	unsigned int stationNO;		//站号
	unsigned int controllerNO;  //控制器号
	unsigned char ipAddr[16];   //地址
	unsigned int  port;         //端口号 
} TGroundServerControllerConfig;

/*********************************************************************
功能：实例化服务
参数：无
返回：0,成功；非0,失败。
作者：韦树远 2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundServerInit();

/*********************************************************************
功能：撤销实例化服务
参数：无
返回：<0,失败。
      >=0，成功
作者：韦树远 2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundServerUninit();

/*********************************************************************
功能：解锁/闭锁函数回调指针
参数：stationNO 站号
      controllerNO 控制器号
      stakeNO 桩号
返回：0表示成功，非0表示不成功
作者：韦树远  2015-01-20
*********************************************************************/
typedef int (* GroundCallBack)(unsigned int stationNO, unsigned int controllerNO, unsigned int stakeNO);

/*********************************************************************
功能：通知回调函数指针
参数：stationNO 站号
返回：0表示成功，非0表示不成功
作者：韦树远  2015-01-20
*********************************************************************/
typedef int (* SynCallBack)(unsigned int stationNO, TGroundServerPointConfig* config, unsigned int count);

/*********************************************************************
功能：注册回调函数
参数：Lock 地线闭锁回调函数指针
      UnLock 地线解锁回调函数指针
	  Update 更新地线状态回调函数指针
	  Syn 自动同步回调函数指针
返回：无
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI void GROUNDCALL_TYPE GCRoundServerRegedit(GroundCallBack Lock, GroundCallBack UnLock, GroundCallBack Update, SynCallBack Syn);

/*********************************************************************
功能：从点表中读取到的地线点表个数
参数：无
返回：地线点表个数
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetPointCount();

/*********************************************************************
功能：从点表中读取到的地线点信息
参数：config 从点表中读到的地线信息数组，在调用此函数前申请内存空间
      count 表示config的大小
返回：> 0 表示成功，实际返回的配置个数
	  < 0 表示不成功
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetPointConfig(TGroundServerPointConfig* config, unsigned int count);

/*********************************************************************
功能：获取地线控制器配置个数
参数：无
返回：地线点表个数
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetControllerCount();

/*********************************************************************
功能：获取地线控制器配置
参数：controller 从点表中读到的地线信息数组，在调用此函数前申请内存空间
      count 表示controller的大小
返回：> 0 表示成功，实际返回的配置个数
      < 0 表示不成功
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetControllerConfig(TGroundServerControllerConfig* controller, unsigned int count);

/*********************************************************************
功能：设置地线状态
参数：controllerNO控制器号
      stakeNO桩号
返回：0表示成功，非0表示不成功
作者：韦树远  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundSetValue(unsigned int stationNO, unsigned int controllerNO, unsigned int stakeNO, unsigned int value);

#endif // GROUNDSERVERINTERFACE_H