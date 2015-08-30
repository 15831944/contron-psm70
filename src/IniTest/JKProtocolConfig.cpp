#include "JKProtocolConfig.h"
//#include "platformLayer/WLOutDebug.h"
//#include "Shares/MYGLOBAL.h"
#include "GCInitFile.h"

#define WLOutDebug printf

CJKProtocolConfig::CJKProtocolConfig()
{
    memset(m_filePath, 0, sizeof(m_filePath));
    memset(&m_TransmitCfgData, 0, sizeof(TTransmitCfgData));
    memset(&m_FrontCommonCfgData, 0, sizeof(TFrontCommonCfgData));
    m_pIniFile = new CGCInitFile();
    m_pSlaveStationCfgList = new TJKSlaveStationCfgData[100];
    m_slaveSumCount = 100;
    m_slaveCount = 0;
    memset(m_pSlaveStationCfgList,0,sizeof(TJKSlaveStationCfgData)*m_slaveSumCount);
}

/************************************************************
功能：析构函数,释放资源
作者：刘远安	2014-4-1
************************************************************/
CJKProtocolConfig::~CJKProtocolConfig()
{
    if (m_pIniFile != NULL)
    {
        delete m_pIniFile;
    }
    if(NULL != m_pSlaveStationCfgList)
    {
        delete[] m_pSlaveStationCfgList;
        m_pSlaveStationCfgList = NULL;
    }
}

/************************************************************
功能：初始化函数
参数：filePath	需要读取的配置文件名称
返回：<0 读取失败
        >=0	读取成功
作者：刘远安	2014-4-1
************************************************************/
int CJKProtocolConfig::readConfigFile(const char *filePath)
{
    int res,ret = -1;
    if (NULL == filePath)   return ret;

    WLOutDebug("config filePath = %s\n",filePath);

    memset(&m_TransmitCfgData, 0, sizeof(TTransmitCfgData));
    memset(&m_FrontCommonCfgData, 0, sizeof(TFrontCommonCfgData));
    memset(&m_JKProtocolCfgData,0,sizeof(TJKProtocolCfgData));
    //默认启用主站
    m_JKProtocolCfgData.isMasterEnable = 1;
    memset(m_filePath,0,sizeof(m_filePath));
    strcpy(m_filePath, filePath);

    ret--;
    BOOL isSuccess = m_pIniFile->OpenIniFile(m_filePath);
    if (FALSE == isSuccess) return ret;

    //--------------------转发信息相关配置
    //是否发送遥信到实时库，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YXRecvTable",0);
    m_TransmitCfgData.IsSendYXToRTDB = res;
    //是否发送遥测到实时库，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YCRecvTable",0);
    m_TransmitCfgData.IsSendYCToRTDB = res;
    //是否发送遥控到实时库，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YKRecvTable",0);
    m_TransmitCfgData.IsSendYKToRTDB = res;
    //是否发送遥信到综自后台，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YXSendTable",0);
    m_TransmitCfgData.IsSendYXToRTU = res;
    //是否发送遥测到综自后台，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YCSendTable",0);
    m_TransmitCfgData.IsSendYCToRTU = res;
    //是否发送遥控到综自后台，1表示发送，0表示不发送
    res = m_pIniFile->ReadInt("YKSendTable",0);
    m_TransmitCfgData.IsSendYKToRTU = res;

    //--------------------通信前置通用参数配置
    //规约
    m_FrontCommonCfgData.protocolType = m_pIniFile->ReadInt("ProtocolType",0);
    //规约服务通信配置
    m_FrontCommonCfgData.channelType = m_pIniFile->ReadInt("ChannelType",0);
    //实时库数据客户端ID号
    res = m_pIniFile->ReadInt("rt_id",0);
    m_FrontCommonCfgData.rtdb_dataID = res;
    //实时库遥控客户端ID号
    res = m_pIniFile->ReadInt("rtdb_YkID",0);
    m_FrontCommonCfgData.rtdb_ykID = res;
    //转发遥控到实时库时，目标客户端ID号
    res = m_pIniFile->ReadInt("rtdb_TargetID",0);
    m_FrontCommonCfgData.rtdb_targetID = res;
    //站号
    res = m_pIniFile->ReadInt("server_station_no",0);
    m_FrontCommonCfgData.stationNo = res;
    //与监控服务通信配置:本地端口号
    res = m_pIniFile->ReadInt("monitor_LocalPort",0);
    m_FrontCommonCfgData.monitorPort = res;

    //--------------------集控规约参数配置
    //网络参数配置
    const char * readRemoteIP = m_pIniFile->ReadString("wf_server_ip","127.0.0.1");
    strcat(m_JKProtocolCfgData.wfMainSvcCfg.remote_ip,readRemoteIP);
    m_JKProtocolCfgData.wfMainSvcCfg.remote_port = m_pIniFile->ReadInt("wf_server_port",0);
    //是否启用主站
    res = m_pIniFile->ReadInt("server_enable",0);
    m_JKProtocolCfgData.isMasterEnable = res;
    //主站监听端口
    res = m_pIniFile->ReadInt("server_port",0);
    m_JKProtocolCfgData.masterNetwork.local_port = res;
    //是否启用子站
    res = m_pIniFile->ReadInt("client_enable",0);
    m_JKProtocolCfgData.isSlaveEnable = res;
    //子站数量
    res = m_pIniFile->ReadInt("client_count",0);
    int slaveCount = res;
    while(slaveCount > m_slaveSumCount)
    {
        getMoreMemory();
    }

    clearSlaveStationCfg();
    TJKSlaveStationCfgData tmpRecord;
    for(int i=0;i<slaveCount;i++)
    {
        memset(&tmpRecord,0,sizeof(TJKSlaveStationCfgData));

        char readKey[256] = {0};
        sprintf(readKey,"client_id[%d]",i);
        tmpRecord.client_id = m_pIniFile->ReadInt(readKey,0);

        memset(readKey,0,sizeof(readKey));
        sprintf(readKey,"client_server_id[%d]",i);
        tmpRecord.client_server_id = m_pIniFile->ReadInt(readKey,0);

        memset(readKey,0,sizeof(readKey));
        sprintf(readKey,"client_port[%d]",i);
        tmpRecord.client_port = m_pIniFile->ReadInt(readKey,0);

        memset(readKey,0,sizeof(readKey));
        sprintf(readKey,"client_ip[%d]",i);
        const char * client_ip = m_pIniFile->ReadString(readKey,"127.0.0.1");
        strcat(tmpRecord.client_ip,client_ip);

        addSlaveStationCfg(&tmpRecord);
    }

    m_slaveCount = slaveCount;

    m_pIniFile->CloseIniFile();

    return 0;
}

/************************************************************
功能：获取配置数据
参数：pCFGData	存放配置数据指针(传出参数)
返回：<0 获取失败
        >=0	获取成功
作者：刘远安	2014-4-1
************************************************************/
int CJKProtocolConfig::getTransmitCfgData(TTransmitCfgData *pCFGData)
{
    if (pCFGData == NULL)
    {
        return -1;
    }
    memcpy(pCFGData, &m_TransmitCfgData, sizeof(TTransmitCfgData));

    return 0;
}

/************************************************************
功能：获取配置数据
参数：pCFGData	存放配置数据指针(传出参数)
返回：<0 获取失败
        >=0	获取成功
作者：刘远安	2014-4-1
************************************************************/
int CJKProtocolConfig::getFrontCommonCfgData(TFrontCommonCfgData *pCFGData)
{
    if (pCFGData == NULL)
    {
        return -1;
    }
    memcpy(pCFGData, &m_FrontCommonCfgData, sizeof(TFrontCommonCfgData));

    return 0;
}

int CJKProtocolConfig::getJKProtocolCfgData(TJKProtocolCfgData *pCFGData)
{
    if (pCFGData == NULL)
    {
        return -1;
    }
    memcpy(pCFGData, &m_JKProtocolCfgData, sizeof(TJKProtocolCfgData));

    return 0;
}

int CJKProtocolConfig::getSlaveStationCfgCount()
{
    return m_slaveCount;
}

int CJKProtocolConfig::getSlaveStationCfgByIndex(int index, TJKSlaveStationCfgData *pOutData)
{
    int ret = -1;
    if((index < 0) || (index >= m_slaveCount) || (NULL == pOutData))    return ret;

    memcpy(pOutData,&m_pSlaveStationCfgList[index],sizeof(TJKSlaveStationCfgData));

    return 0;
}

const TJKSlaveStationCfgData *CJKProtocolConfig::getSlaveStationCfgByIndex(int index)
{
    int ret = -1;
    if((index < 0) || (index >= m_slaveCount))    return NULL;

    return &m_pSlaveStationCfgList[index];

    return NULL;
}

const TJKSlaveStationCfgData *CJKProtocolConfig::findByClientID(DWORD32 clientID)
{
    int index = -1;
    for(int i=0;i<m_slaveCount;i++)
    {
        if(m_pSlaveStationCfgList[i].client_id == clientID)
        {
            index = i;
            break;
        }
    }

    if(-1 == index) return NULL;

    return &m_pSlaveStationCfgList[index];
}

/************************************************************
功能：添加记录到内存
参数：pRecord，记录对象
返回：>=0，成功；
     <0，失败
作者：刘远安	2014-09-03
************************************************************/
int CJKProtocolConfig::addSlaveStationCfg(TJKSlaveStationCfgData *pRecord)
{
    int res,ret = -1;
    if(NULL == pRecord)	return ret;

    ret--;
    if(m_slaveCount >= m_slaveSumCount)
    {
        res = getMoreMemory();
        if(res < 0)
        {
            WLOutDebug("getMoreMemory failed!");
            return ret;
        }
    }
    memcpy(&m_pSlaveStationCfgList[m_slaveCount],pRecord,sizeof(TJKSlaveStationCfgData));
    m_slaveCount++;

    //更改ID值(ID为自增字段)
    DWORD32 ID = 1;
    if (m_slaveCount > 1)	ID = m_pSlaveStationCfgList[m_slaveCount-2].ID+1;
    m_pSlaveStationCfgList[m_slaveCount-1].ID   = ID;

    return 0;
}

/************************************************************
功能：清空内存记录
参数：无
返回：>=0，成功；
     <0，失败
作者：刘远安	2014-09-03
************************************************************/
int CJKProtocolConfig::clearSlaveStationCfg()
{
    int res,ret = -1;
    memset(m_pSlaveStationCfgList,0,sizeof(TJKSlaveStationCfgData)*m_slaveSumCount);
    m_slaveCount = 0;

    return 0;
}

int CJKProtocolConfig::updateSlaveStationCfg(TJKSlaveStationCfgData *pRecord)
{
    int ret = -1;
    if(NULL == pRecord) return ret;

    int index = -1;
    for(int i=0;i<m_slaveCount;i++)
    {
        if(m_pSlaveStationCfgList[i].ID == pRecord->ID)
        {
            index = i;
            break;
        }
    }

    ret--;
    if(-1 == index) return ret;

    memcpy(&m_pSlaveStationCfgList[index],pRecord,sizeof(TJKSlaveStationCfgData));

    return 0;
}

int CJKProtocolConfig::delSlaveStationCfgByClientID(DWORD32 clientID)
{
    int ret = -1;
    int index = -1;
    for(int i=0;i<m_slaveCount;i++)
    {
        if(m_pSlaveStationCfgList[i].client_id == clientID)
        {
            index = i;
            break;
        }
    }

    ret--;
    if(-1 == index) return ret;

    if (index == m_slaveCount -1)	m_slaveCount--;	//删除记录在队列尾
    else																//删除记录在队头或队中
    {
        memcpy(&m_pSlaveStationCfgList[index],&m_pSlaveStationCfgList[index + 1],(m_slaveCount - index -1)*sizeof(TJKSlaveStationCfgData));
        m_slaveCount--;
    }

    return 0;
}

/************************************************************
功能：保存配置数据
参数：pCFGData，待保存的配置数据
返回：<0，失败
     >=0，成功
作者：刘远安	2014-4-1
************************************************************/
int CJKProtocolConfig::saveTransmitCfgData(TTransmitCfgData *pCfgData)
{
    int res ,ret = -1;
    if(NULL == pCfgData)
    {
        return ret;
    }

    ret--;
    if(strlen(m_filePath) <= 0) return ret;

    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(m_filePath,"a+");
    fclose(f);

    ret--;
    BOOL isOpen = m_pIniFile->OpenIniFile(m_filePath);
    if (FALSE == isOpen)    return ret;

    //是否发送遥信到实时库，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YXRecvTable",pCfgData->IsSendYXToRTDB);
    //是否发送遥测到实时库，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YCRecvTable",pCfgData->IsSendYCToRTDB);
    //是否发送遥控到实时库，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YKRecvTable",pCfgData->IsSendYKToRTDB);
    //是否发送遥信到综自后台，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YXSendTable",pCfgData->IsSendYXToRTU);
    //是否发送遥测到综自后台，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YCSendTable",pCfgData->IsSendYCToRTU);
    //是否发送遥控到综自后台，1表示发送，0表示不发送
    m_pIniFile->WriteInt("YKSendTable",pCfgData->IsSendYKToRTU);

    ret--;
    bool isSuccess = m_pIniFile->WriteIniFile(m_filePath);
    if(!isSuccess)  return ret;

    return 0;
}

/************************************************************
功能：保存配置数据
参数：pCFGData，待保存的配置数据
返回：<0，失败
     >=0，成功
作者：刘远安	2014-4-1
************************************************************/
int CJKProtocolConfig::saveFrontCommonCfgData(TFrontCommonCfgData *pCfgData)
{
    int res ,ret = -1;
    if(NULL == pCfgData)
    {
        return ret;
    }

    ret--;
    if(strlen(m_filePath) <= 0) return ret;

    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(m_filePath,"a+");
    fclose(f);

    ret--;
    BOOL isOpen = m_pIniFile->OpenIniFile(m_filePath);
    if (FALSE == isOpen)    return ret;

    //--------------------通信前置通用参数配置
    //规约
    m_pIniFile->WriteInt("ProtocolType",pCfgData->protocolType);
    //规约服务通信配置
    m_pIniFile->WriteInt("ChannelType",pCfgData->channelType);
    //实时库数据客户端ID号
    m_pIniFile->WriteInt("rt_id",pCfgData->rtdb_dataID);
    //实时库遥控客户端ID号
    m_pIniFile->WriteInt("rtdb_YkID",pCfgData->rtdb_ykID);
    //转发遥控到实时库时，目标客户端ID号
    m_pIniFile->WriteInt("rtdb_TargetID",pCfgData->rtdb_targetID);
    //站号
    m_pIniFile->WriteInt("server_station_no",pCfgData->stationNo);
    //与监控服务通信配置:本地端口号
    m_pIniFile->WriteInt("monitor_LocalPort",pCfgData->monitorPort);


    ret--;
    bool isSuccess = m_pIniFile->WriteIniFile(m_filePath);
    if(!isSuccess)  return ret;

    return 0;
}

int CJKProtocolConfig::saveTransmitCfgData(const char *filePath, TTransmitCfgData *pCfgData)
{
    int res,ret = -1;
    if (NULL == filePath)   return ret;

    WLOutDebug("config filePath = %s\n",filePath);
    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(filePath,"a+");
    fclose(f);

    memset(m_filePath,0,sizeof(m_filePath));
    strcpy(m_filePath, filePath);

    return saveTransmitCfgData(pCfgData);
}

int CJKProtocolConfig::saveFrontCommonCfgData(const char *filePath, TFrontCommonCfgData *pCfgData)
{
    int res,ret = -1;
    if (NULL == filePath)   return ret;

    WLOutDebug("config filePath = %s\n",filePath);
    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(filePath,"a+");
    fclose(f);

    memset(m_filePath,0,sizeof(m_filePath));
    strcpy(m_filePath, filePath);

    return saveFrontCommonCfgData(pCfgData);
}

int CJKProtocolConfig::saveJKProtocolCfgData(TJKProtocolCfgData *pCfgData)
{
    int res ,ret = -1;
    if(NULL == pCfgData)
    {
        return ret;
    }

    ret--;
    if(strlen(m_filePath) <= 0) return ret;

    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(m_filePath,"a+");
    fclose(f);

    ret--;
    BOOL isOpen = m_pIniFile->OpenIniFile(m_filePath);
    if (FALSE == isOpen)    return ret;

    //--------------------集控规约参数配置
    //网络参数配置
//    m_pIniFile->WriteString("wf_server_ip",g_curCFGData.wfc_ip);
//    m_pIniFile->WriteInt("wf_server_port",g_curCFGData.wfc_remote_port);
    //是否启用主站
    m_pIniFile->WriteInt("server_enable",pCfgData->isMasterEnable);
    //主站监听端口
    m_pIniFile->WriteInt("server_port",pCfgData->masterNetwork.local_port);
    //是否启用子站
    m_pIniFile->WriteInt("client_enable",pCfgData->isSlaveEnable);
    //子站数量
    if(0 == pCfgData->isSlaveEnable)
    {
        m_pIniFile->WriteInt("client_count",0);
    }
    else
    {
        m_pIniFile->WriteInt("client_count",m_slaveCount);
        for(int i=0;i<m_slaveCount;i++)
        {
            char readKey[256] = {0};
            sprintf(readKey,"client_id[%d]",i);
            m_pIniFile->WriteInt(readKey,m_pSlaveStationCfgList[i].client_id);

            memset(readKey,0,sizeof(readKey));
            sprintf(readKey,"client_server_id[%d]",i);
            m_pIniFile->WriteInt(readKey,m_pSlaveStationCfgList[i].client_server_id);

            memset(readKey,0,sizeof(readKey));
            sprintf(readKey,"client_port[%d]",i);
            m_pIniFile->WriteInt(readKey,m_pSlaveStationCfgList[i].client_port);

            memset(readKey,0,sizeof(readKey));
            sprintf(readKey,"client_ip[%d]",i);
            m_pIniFile->WriteString(readKey,m_pSlaveStationCfgList[i].client_ip);
        }
    }


    ret--;
    bool isSuccess = m_pIniFile->WriteIniFile(m_filePath);
    if(!isSuccess)  return ret;

    return 0;
}

int CJKProtocolConfig::saveJKProtocolCfgData(const char *filePath, TJKProtocolCfgData *pCfgData)
{
    int res,ret = -1;
    if (NULL == filePath)   return ret;

    WLOutDebug("config filePath = %s\n",filePath);
    // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
    FILE *f = fopen(filePath,"a+");
    fclose(f);

    memset(m_filePath,0,sizeof(m_filePath));
    strcpy(m_filePath, filePath);

    return saveJKProtocolCfgData(pCfgData);
}

int CJKProtocolConfig::getMoreMemory()
{
    int sumCount = m_slaveSumCount + 100;
    TJKSlaveStationCfgData *pNewBuf = new TJKSlaveStationCfgData[sumCount];

    if(NULL == pNewBuf)
    {
        WLOutDebug("CJKProtocolConfig::getMoreMemory()-->申请内存失败!");
        return -1;
    }
    m_slaveSumCount = sumCount;
    memset(pNewBuf, 0x00, sizeof(TJKSlaveStationCfgData)*m_slaveSumCount);
    memcpy(pNewBuf, m_pSlaveStationCfgList, sizeof(TJKSlaveStationCfgData)*m_slaveCount);
    delete [] m_pSlaveStationCfgList;

    m_pSlaveStationCfgList = pNewBuf;
    return 0;
}
