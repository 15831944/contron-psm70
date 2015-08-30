#ifndef PSM_FILE_H
#define PSM_FILE_H

#include "sys/platform.h"

/*!
 * \brief   自定义时间数据类型
 */
typedef struct _psm_file_time {
    WORD year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
    BYTE RFU;
}psm_file_time_t;

/*!
 * \brief   实时库文件头结构
 */
typedef struct _psm_file_header {
    DWORD32  recoredCount;          //记录数量
    DWORD32  perRecLen;             //单条记录长度
    BYTE fileCreatTime[8];          //文件创建日期
    BYTE fileUpdateTime[8];         //最后修改日期
}psm_file_header_t;

/*!
 * \brief   文件头结构
 * \note    用于遥信、遥控、遥测、KBQ点表文件
 */
typedef struct _psm_file_header_fixed{
    DWORD   version;                    //版本号
    DWORD   recoredCount;               //记录数量
    DWORD   perRecLen;                  //单条记录长度
    DWORD   createrID;                  //创建者标识
    psm_file_time_t fileCreatTime;              //文件创建日期
    DWORD   updaterID;                  //修改者标识
    psm_file_time_t fileUpdateTime;             //最后修改日期
    BYTE    rfu[16];                    //预留字节
    BYTE    customFields[48];           //自定义区，存储内容结构与最终的文件相关
}psm_file_header_fixed_t;
/*!
 * \brief   实时库设备结构
 */
typedef struct _psm_file_record
{
    DWORD32 stationID;		// 站编号
    BYTE macNum[16];		// 设备主编号
    DWORD32 pointID;		// 设备点号
    DWORD32 innerID;		// 实时库内部ID
    DWORD32 macID;			// 设备ID
    DWORD32 type;			// 设备类型
    DWORD32 vtype;			// 1表示遥信,2表示遥测
    DWORD32 vindex;			// 0表示唯一操作权, 1表示状态
    DWORD32 value;			// 设备状态值
}psm_file_record_t;

/*!
 * \brief   遥信结构
 */
typedef struct _psm_file_yx
{
    DWORD32             ID;             // 主键
    DWORD32             yxNo;           // 遥信点号
    WORD                yxType;         // 遥信点类型：0，单位置；1，双位置(当为单位置时，posType字段无效)
    WORD                posType;        // 检测点类型：0，合位置；1，分位置
    int                 yxState;        // 遥信分或合位置状态
    WORD                isOpposite;     // 遥信状态是否取反：0，不取反；1，取反
    WORD                isThreeState;   // 是否为三态设备：0，否；1，是
    psm_file_record_t   record;         // 实时库设备记录
}psm_file_yx_t;

/*!
 * \brief   遥控结构
 */
typedef struct _YKRecord
{
    DWORD32             ID;             // 主键
    DWORD32             ykNo;           // 遥控点号
    psm_file_record_t   record;         // 实时库设备记录
}TYKRecord;

/*!
 * \brief   遥测结构
 */
typedef struct _YCRecord
{
    DWORD32             ID;             // 主键
    DWORD32             ycNo;           // 遥测点号
    psm_file_record_t   record;         // 实时库设备记录
}TYCRecord;

/*!
 * \brief   KBQ结构
 */
typedef struct _KBQRecord
{
    DWORD32             ID;                 // 主键
    DWORD32             devBeforeState;     // 设备操作前状态
    DWORD32             devAfterState;      // 设备操作后状态
    DWORD32             kbqAddr;            // KBQ地址
    DWORD32             kbqPort;            // KBQ端口
    psm_file_record_t   kbqRecord;          // KBQ关联纪录（对应实时库设备记录）
    psm_file_record_t   devRecord;          // 设备关联纪录（对应实时库设备记录）
}TKBQRecord;

int psm_file_load_yx(char *file_name, psm_file_yx_t **record, int &record_count);
int psm_file_load_kbq(char *file_name, TKBQRecord **record, int &record_count);
int psm_file_load_yk(char *file_name, TYKRecord **record, int &record_count);

#endif // PSM_FILE_H
