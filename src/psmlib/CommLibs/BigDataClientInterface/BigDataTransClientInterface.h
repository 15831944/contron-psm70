/********************************************************************************************************************
功能：大数据传输客户端对外接口函数库。
说明：（1）必须按一定顺序调用。
      （2）调用顺序为：GCBDInitClientSvc()-->GCBDRegisterCallbackConnectState(可选)-->GCBDStartupClientSvc()-->GCBDUpLoadFile()/
           GCBDGetFileList()-->GCBDDownLoadFile()-->GCBDDownLoadFolder()-->GCBDShutdownSvc()-->GCBDUninstantiateClientSvc()。
      （3）本库暂不确定对于多线程是否安全
作者：刘远安	2014-03-17
********************************************************************************************************************/
#ifndef BIGDATATRANSCLIENTINTERFACE_H
#define BIGDATATRANSCLIENTINTERFACE_H

//#define BIGDATATRANSCLIENTDLL_EXPORTS

#ifdef _WIN32
#ifdef BIGDATATRANSCLIENTDLL_EXPORTS
#define BDTCAPI extern "C" __declspec(dllexport)
#else
#define BDTCAPI extern "C" __declspec(dllimport)
#endif
#else
#define BDTCAPI
#endif

#ifdef _WIN32
//#define CALL_TYPE _stdcall	//windows系统用的回调函数一般都是_stdcall，WIN32 Api都采用_stdcall调用方式
#define CALL_TYPE __cdecl	//这是C和C++默认调用方式，使用这种方式QT creator编辑器可以静态/隐式调用
#else
#define CALL_TYPE
#endif

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

#include <stdio.h>

typedef unsigned short      WORD;
#ifdef _WIN32
typedef unsigned long       DWORD;
#else
typedef unsigned int       DWORD;
#endif
typedef unsigned int        DWORD32;
typedef unsigned long long  DWORD64;   //无符号八字节，long long型，32位和64位中都为八字节
//文件信息
typedef struct _fileInfo{
	char filePath[MAX_PATH];
}TFileInfo;

/*********************************************************************
功能：与大数据传输服务通讯情况回调
参数：connectState，连接状态，0，断开连接；1，连接正常
返回：暂无意义
作者：刘远安  2014-09-15
*********************************************************************/
typedef int (* callbackConnectState)(DWORD32 connectState);

/*********************************************************************
功能：实例化服务
参数：localID，本地ID
      remoteIP，服务器IP
      remotePort，服务器端口号
      callbackFunc,回调函数指针,服务端非正常断开连接后,调用此函数
      localPort，本地端口号
返回：NULL,失败。
      非NULL,服务实例
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI void * CALL_TYPE GCBDInitClientSvc(int localID,const char * remoteIP,WORD remotePort,WORD localPort = 0);

/*********************************************************************
功能：撤销实例化服务
参数：pSvc，服务实例，由GCBDInitClientSvc返回
返回：<0,失败。
      >=0，成功
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDUninstantiateClientSvc(void * pSvc);

/*********************************************************************
功能：注册与大数据传输服务通讯情况回调函数
参数：pSvc，服务实例，由GCBDInitClientSvc返回
返回：<0,失败。
      >=0，成功
作者：刘远安  2014-09-15
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDRegisterCallbackConnectState(callbackConnectState callbackFunc,void * pSvc);

/*********************************************************************
功能：启动服务
参数：pSvc，服务实例
返回：>=0 ,启动成功。
	  <0 ,启动失败。
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDStartupClientSvc(void * pSvc);

/*********************************************************************
功能：关闭服务
参数：pSvc，需要关闭的服务实例
返回：<0,失败。
      >=0,成功
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDShutdownSvc(void * pSvc);

/*********************************************************************
功能：网络上传文件
参数：pSvc，服务实例
      filePath，文件绝对路径；
	  sendPackSize，每次发送数据包大小，范围100--65536；
	  fileSize，输出参数，文件大小（单位字节）
      fileSaveDir，文件保存目录，默认为服务器的当前目录
      newName,文件上传后保存到服务器的名称（文件名可以带后缀，也可以不带后缀），默认使用上传前的名称
返回：<0,失败。
	  >=0,成功
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDUpLoadFile(void * pSvc,const char * filePath,DWORD sendPackSize,DWORD64 & fileSize,const char * fileSaveDir="./",const char * newName=NULL);

/*********************************************************************
功能：网络上传文件夹
参数：pSvc，服务实例
      folder，文件夹绝对路径；
      fileSaveDir，文件保存目录，默认为服务器的当前目录
返回：<0，失败；>=0，成功。
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDUpLoadFolder(void * pSvc,const char * folder,const char * fileSaveDir="./");

/*********************************************************************
功能：获取服务器端文件列表
参数：pSvc，服务实例
	  fileInfoList，输出参数，存放文件信息；
	  inCount，fileInfoList分配的内存单元数；
	  outCount，输出参数，获取的文件信息数量
返回：<0，失败；>=0，成功。0，数据全部取完；1，数据未全部取完
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDGetFileList(void * pSvc,TFileInfo fileInfoList[],DWORD inCount,DWORD & outCount);

/*********************************************************************
功能：网络下载文件
参数：pSvc，服务实例;filePath，文件路径；fileSize，输出参数，文件大小（单位字节）
     fileSaveDir，文件下载后保存到本地磁盘的目录，默认为当前目录
     newName,文件下载后保存到本地磁盘的名称（文件名可以带后缀，也可以不带后缀），默认不重命名
返回：>=0，成功;
	 <0，失败：-100，请求的文件不存在
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDDownLoadFile(void * pSvc,const char * filePath,DWORD64 & fileSize,const char * fileSaveDir="./",const char * newName=NULL);

/*********************************************************************
功能：网络下载文件夹
参数：pSvc，服务实例;folder，文件夹路径；fileSaveDir，输出参数，文件大小（单位字节）
      fileSaveDir，文件存放目录，默认为当前目录
返回：<0，失败；>=0，成功。
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDDownLoadFolder(void * pSvc,const char * folder,const char * fileSaveDir="./");


/*********************************************************************
功能：询问服务端文件或文件夹是否存在
参数：pSvc，服务实例;fileOrDirPath，文件或文件夹绝对路径；
	  isExist,输出参数，0，不存在；1，存在
返回：<0，失败；>=0，成功。
作者：刘远安 2014-03-17
*********************************************************************/
BDTCAPI int CALL_TYPE GCBDIsFileOrFolderExist(void * pSvc,const char * fileOrDirPath,DWORD32 &isExist);

#endif // BIGDATATRANSCLIENTINTERFACE_H
