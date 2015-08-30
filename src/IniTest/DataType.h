//网络通信参数配置
typedef struct _NetworkCfgData
{
    DWORD32 local_id;
    char    local_ip[32];
    DWORD32 local_port;
    char    remote_ip[32];
    DWORD32 remote_port;
}TNetworkCfgData;

//转发数据参数配置
typedef struct _TransmitCfgData
{
    DWORD32 IsSendYXToRTDB;     //是否发送遥信到实时库
    DWORD32 IsSendYCToRTDB;     //是否发送遥测到实时库
    DWORD32 IsSendYKToRTDB;     //是否发送遥控到实时库
    DWORD32 IsSendYXToRTU;		//是否发送遥信到综自后台
    DWORD32 IsSendYCToRTU;		//是否发送遥测到综自后台
    DWORD32 IsSendYKToRTU;		//是否发送遥控到综自后台
    DWORD32 IsSendKbqDevToRTU;  //是否发送KBQ所属设备信息到综自后台
}TTransmitCfgData;

//通信前置通用参数配置
typedef struct _FrontCommonCfgData
{
    DWORD32  rtdb_dataID;           //实时库数据客户端ID号
    DWORD32  rtdb_ykID;             //实时库遥控客户端ID号
    DWORD32  rtdb_targetID;         //转发遥控到实时库时，目标客户端ID号
    DWORD32  rtdb_ticketID;         //实时库客户端ID号(收发票信息)
    DWORD32  stationNo;             //站号
    DWORD32  monitorPort;           //与监控服务通信配置:本地端口号
    DWORD32 protocolType;           //规约类型:1,CDT规约客户端；2，CDT规约服务端；3，104规约客户端；4，104规约服务端
    DWORD32 channelType;            //通道类型:1,TCP客户端；2，TCP服务端；3，UDP；4，串口
}TFrontCommonCfgData;

//集控规约通信前置参数配置结构体
typedef struct _JKProtocolCfgData
{
    TNetworkCfgData wfMainSvcCfg;    //防误总服务配置
    //主站配置
    DWORD32 isMasterEnable;          //是否启用主站，0－不启用，1－启用
    TNetworkCfgData masterNetwork;   //主站网络参数配置
//    DWORD32 masterStationNo;         //主站站号
    //子站配置
    DWORD32 isSlaveEnable;           //是否启用子站，0－不启用，1－启用
} TJKProtocolCfgData;
