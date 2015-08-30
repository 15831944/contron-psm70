#ifndef JKDEF
#define JKDEF

#include "sys/platform.h"
#include "psm_def.h"

#define DATA_SIZE           1024
#define HEADER_SIZE         4
#define DEVICE_PER_FRAME    40

/*!
  缩写约定
  PRL   =   protocol            协议
  CMD   =   command             命令码
  FNN   =   function            功能码
  PT    =   protocol type       协议类型
  CT    =   control type        控制类型
  ST    =   station type        站类型  
 */
#define PRL_PT_JK               4           //集控主子站协议

#define PRL_CT_SEND             0x40        //主动发送
#define PRL_CT_BACK             0x80        //应答
#define PRL_CT_ACK              0x81        //确认

#define PRL_ST_YT               100         //优特五防
#define PRL_ST_CONTRON          110         //共创五防
#define PRL_ST_KX               120         //拓新五防
#define PRL_ST_NR               130         //南瑞五防
#define PRL_ST_XF               140         //湖北襄樊五防
#define PRL_ST_OTHER            240         //其他五防

#define PRL_ACK_ERROR_NONE      0x0         //ack数据正确
#define PRL_ACK_ERROR_LENGTH    0x1         //ack数据长度错误
#define PRL_ACK_ERROR_CMD       0x2         //ack命令码错误
#define PRL_ACK_ERROR_FNN       0x3         //ack功能码错误
#define PRL_ACK_ERROR_DATA      0x4         //ack数据错误，用于crc校验错误和站号不匹配


#define PRL_STRUCT_POINT        0           //数据结构使用点表
#define PRL_STRUCT_NAME         1           //数据结构使用设备主编号

#define PRL_CMD_HEARTBEAT                       0x04    //心跳命令码
#define PRL_FNN_HEARTBEAT                       0x00    //心跳功能码

#define PRL_CMD_TIME                            0x01    //时间命令码
#define PRL_FNN_SYNC_TIME                       0x01    //时间同步功能码

#define PRL_CMD_REQUEST_DATA                    0x02    //请求遥信及闭锁信息帧数命令码
#define PRL_FNN_REQUEST_DATA_COUNT              0x00    //请求遥信及闭锁信息帧数功能码
#define PRL_FNN_REQUEST_DATA                    0x01    //请求遥信及闭锁信息数据帧功能码
#define PRL_FNN_UPLOAD_DEVICE_STATE             0x02    //上报变位遥信功能码
#define PRL_FNN_UPLOAD_DEVICE_LOCK_STATE        0x03    //上报设备闭锁功能码
#define PRL_FNN_UPLOAD_YK_DEVICE_STATE          0x09    //上报遥控闭锁继电器状态功能码


#define PRL_CMD_TICKET                          0x03    //操作票
#define PRL_FNN_SEND_TICKET                     0x01    //主站传送操作序列

#define PRL_CMD_REQUEST_DEVICE                  0x06    //请求设备控制
#define PRL_FNN_REQUEST_DEVICE_UNLOCK           0x01    //请求设备解锁
#define PRL_FNN_REQUEST_DEVICE_LOCK             0x02    //请求设备闭锁
#define PRL_FNN_REQUEST_DEVICE_UNLOCK_ALL       0x03    //请求设备全解锁
#define PRL_FNN_REQUEST_DEVICE_LOCK_ALL         0x04    //请求设备全闭锁



typedef struct _command
{
    UCHAR   header[HEADER_SIZE];        //同步头
    USHORT  data_size;                  //数据长度，包括命令码和功能码
    UCHAR   protocol_type;              //协议类型
    UCHAR   control_type;               //控制类型
    USHORT  source_address;             //源地址
    USHORT  target_address;             //目标地址
    UCHAR   source_type;                //源类型，参考站类型
    UCHAR   target_type;                //目标类型，参考站类型
    USHORT  crc;                        //校验码
    char    *data;                      //数据区，包括命令码和功能码
} command_t, jk_command_t;


#endif // JKDEF

