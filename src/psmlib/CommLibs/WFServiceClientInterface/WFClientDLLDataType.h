#ifndef WFCLIENTDLLDATATYPE_H
#define WFCLIENTDLLDATATYPE_H

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

//子服务类型
enum SubServerType {
	CreateRTDBSvc,				//实时库数据生成服务
	BackupDBSvc,				//数据库备份服务
	CreateLdataFile,			//生成五防逻辑文件服务
	CreateStationDataFile		//生成站数据文件服务
};

//大数据服务参数配置结构体
typedef struct _BigDataCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TBigDataCfgData;

//实时库参数配置结构体
typedef struct _RTDBSvcCfgData
{
	char    remote_ip[32];  //远程IP
	DWORD32 remote_port;    //实时值发布端口
	DWORD32 remote_pubport; //消息发布端口
}TRTDBSvcCfgData;

////数据库参数配置结构体
//typedef struct _DBCfgData
//{
//    char db_address[128];           //数据库地址，如：10.7.5.136\sqlexpress
//    char db_name[128];              //数据库名，如：FieryDrag827
//    char db_uid[32];                //数据库登录用户，如：root
//    char db_pwd[8];                 //数据库登录密码
//    char db_dsn[32];                //数据源名称
//} TDBCfgData;

//数据库参数配置结构体
typedef struct _DBCfgData
{
	DWORD32 db_type;                //数据库类型:0,MySQL;1,SQLite;2,SQLServer
	char db_driver[128];            //数据库驱动名称
	char db_address[128];           //数据库地址，如：10.7.5.136\sqlexpress
	char db_name[260];              //数据库名，如：FieryDrag827
	char db_uid[32];                //数据库登录用户，如：root
	char db_pwd[8];                 //数据库登录密码
	char db_dsn[32];                //数据源名称
} TDBCfgData;

//五防逻辑校核服务参数配置结构体
typedef struct _WFLogicCheckCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TWFLogicCheckCfgData;

//拓扑校核服务参数配置结构体
typedef struct _TopologyCheckCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TTopologyCheckCfgData;

//票服务的配置参数配置结构体
typedef struct _TicketSvcCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TTicketSvcCfgData;

//票编号发生器的配置参数配置结构体
typedef struct _TicketSNDeneratorCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TTicketSNDeneratorCfgData;

//实时库客户端ID生成服务的配置参数结构体
typedef struct _RTDBClientIDCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TRTDBClientIDCfgData;

//子服务的配置参数结构体
typedef struct _SubServerCfgData
{
	char    remote_ip[32];
	DWORD32 remote_port;
}TSubServerCfgData;

typedef enum _FileType			// 文件类型
{
	FT_WFClient	=	1,			// 五防图形客户端文件
	FT_WFClientCfg,				// 五防图形客户端配置文件
	FT_UI,                      // UI文件
	FT_PointTable,              // 点表文件
	FT_Other					// 其它
}EFileType;

//文件信息
// typedef struct _fileInfo{
// 	char filePath[MAX_PATH];
// }TFileInfo;

//文件或文件夹路径
typedef struct _fileDetails{
	DWORD32 type;               //类型：0，文件路径；1，文件夹路径
	char filePath[MAX_PATH];    //文件或文件夹路径
}TFileDetails;

//记录站编号的结构体
typedef struct _StationIDItem{
    DWORD32 stationNo;          //站编号
	DWORD32 stationType;        //站类型
}TStationIDItem;

//地线控制器配置的结构体
typedef struct _EarthlineCtrlCfg{
    DWORD32 stationID;				//站ID
    DWORD32 controllerID;			//地线控制器编号
    TSubServerCfgData addr;			//地线控制器IP和端口
}TEarthlineCtrlCfg;

#endif
