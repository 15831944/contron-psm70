/***********************************************************************
功能：读写集控规约配置文件

作者：刘远安
************************************************************************/
#ifndef JKPROTOCOLCONFIG_H
#define JKPROTOCOLCONFIG_H

#include "sys/platform.h"
#include "DataType.h"

//#include "platformLayer/WLDEF.h"
//#include "BusinessDataProtocol/BusinessDataTypeProtocol.h"

#define MAX_SLAVE_COUNT 64 //最大支持子站数量

//集控子站参数配置
typedef struct _JKSlaveStationCfgData
{
    DWORD32 ID;                 //主键
    DWORD32 client_id;          //子站站号
    char    client_ip[32];      //子站IP
    DWORD32 client_port;        //子站端口
    DWORD32 client_server_id;   //主站站号
}TJKSlaveStationCfgData;

class CGCInitFile;
class CJKProtocolConfig
{
public:
    CJKProtocolConfig();
    virtual ~CJKProtocolConfig();

    int readConfigFile(const char *filePath);

    int getTransmitCfgData(TTransmitCfgData *pCFGData);
    int getFrontCommonCfgData(TFrontCommonCfgData *pCFGData);
    int getJKProtocolCfgData(TJKProtocolCfgData *pCFGData);

    int getSlaveStationCfgCount();
    int addSlaveStationCfg(TJKSlaveStationCfgData *pRecord);
    int clearSlaveStationCfg();
    int updateSlaveStationCfg(TJKSlaveStationCfgData *pRecord);
    int delSlaveStationCfgByClientID(DWORD32 clientID);

    int getSlaveStationCfgByIndex(int index,TJKSlaveStationCfgData *pOutData);
    const TJKSlaveStationCfgData *getSlaveStationCfgByIndex(int index);
    const TJKSlaveStationCfgData *findByClientID(DWORD32 clientID);

    int saveTransmitCfgData(TTransmitCfgData *pCfgData);
    int saveTransmitCfgData(const char *filePath,TTransmitCfgData *pCfgData);
    int saveFrontCommonCfgData(TFrontCommonCfgData *pCfgData);
    int saveFrontCommonCfgData(const char *filePath,TFrontCommonCfgData *pCfgData);
    int saveJKProtocolCfgData(TJKProtocolCfgData *pCfgData);
    int saveJKProtocolCfgData(const char *filePath,TJKProtocolCfgData *pCfgData);

private:
    int getMoreMemory();

private:
    char m_filePath[MAX_PATH];
    CGCInitFile *m_pIniFile;

    TTransmitCfgData m_TransmitCfgData;                     //转发数据参数配置
    TFrontCommonCfgData m_FrontCommonCfgData;               //通信前置通用参数配置
    TJKProtocolCfgData m_JKProtocolCfgData;                 //集控规约参数配置
    TJKSlaveStationCfgData *m_pSlaveStationCfgList;         //子站通信参数配置
    DWORD32 m_slaveCount;
    DWORD32 m_slaveSumCount;
};

#endif // JKPROTOCOLCONFIG_H
