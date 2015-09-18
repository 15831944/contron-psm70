/***********************************************************************
功能：P7协议命令码定义文件
作者：唐小灿  2014-10-31
说明：P7系统各程序中所使用通信命令码参照本文件定义
************************************************************************/
#ifndef __P7PROTOCOLINSTYPE_H__
#define __P7PROTOCOLINSTYPE_H__


//防务校核通讯协议
#define INC_WF_LOGIC_CHECK			   (0xB0) //防务逻辑校核

//消息平台相关命令
#define INS_TRANSFORM_MSG_YK		   (0xC1) //转发遥控消息

//地线消息相关命令字
#define INS_GROUND_MSG_LOCK		       (0xCA) //转发地线闭锁消息
#define INS_GROUND_MSG_UNLOCK		   (0xCB) //转发地线解锁消息
#define INS_GROUND_MSG_SYNC_REQ        (0xCC) //转发地线自动请求同步消息
#define INS_GROUND_MSG_SYNC_RES        (0xCD) //转发地线自动同步应答消息
#define INS_GROUND_MSG_UPDATE 	       (0xCE) //转发地线查询状态消息
#define INS_GROUND_MSG_EARTHLINEACTION (0xCF) //转发地线动作，当票动作完成，将票动作对应地线动作通知图形客户端

//票相关命令，用于票服务，票执行(遥控执行)，钥匙服务程序(就地执行)，图形客户端之间通信
#define INS_TICKET_PAUSE               (0x10) //暂停/取消暂停票(客户端->票服务)
#define INS_TICKET_SWITCH              (0x11) //切换票(客户端->票服务)
#define INS_TICKET_MODIFY_STATE        (0x12) //广播修改票状态(由票服务发起广播)
#define INS_TICKET_SEND                (0xD1) //发票(客户端->票服务/客户端->钥匙服务/票服务->票监控执行)
#define INS_TICKET_FINISH_TICKET       (0xD2) //完成票(票服务->图形客户端)
#define INS_TICKET_TERMINATE_TICKET    (0xD3) //终止票
#define INS_TICKET_FORCE_TICKET        (0xD4) //强行终止票
#define INS_TICKET_FINISH_TICKETACT    (0xD5) //完成票动作(票服务->图形客户端)
#define INS_TICKET_FINISH_TICKETITEM   (0xD6) //完成票项(票动作下的票项)
#define INS_TICKET_CONVERT_OPERATETYPE (0xD7) //操作类型转换(遥控/就地转换，客户端->票服务)
#define INS_TICKET_REBACK              (0xD8) //回传票，暂为图形通知票执行(就地)程序命令
#define INS_TICKET_SEND_TICKET_TO_KEY  (0xD9) //通知票执行(就地)程序下发操作票到钥匙
#define INS_TICKET_CHECK_KEY_ONLINE	   (0xDA) //询问钥匙服务程序钥匙在线情况
#define INS_TICKET_CHECK_CAN_SEND_TIC  (0xDB) //询问哪些票可以下发到钥匙
#define INS_TICKET_REPORT_KEY_TICKET   (0xDC) //钥匙服务程序向票服务程序汇报钥匙票执行情况
#define INS_TICKET_SEND_RFID_TO_KEY    (0xDD) //通知钥匙服务程序下发RFID到钥匙
#define INS_TICKET_REQUESTCONFIRMITEM  (0xDE) //广播请求确认票项
#define INS_TICKET_MODIFY_TICKETSTATE  (0x95) //广播修改票状态
// 数据库备份与还原命令
#define INS_DB_BACKUP                  (0xE1) //发送数据库备份命令
#define INS_DB_BACKUP_RES              (0xE2) //数据库备份结果命令
#define INS_DB_RESTORE                 (0xE3) //发送数据库还原命令
#define INS_DB_RESTORE_RES             (0xE4) //数据库还原结果命令
//KBQͨ��ǰ�
#define INS_KBQ_SET_UNDER_DEVICE_ACT   (0xE9) //������豸�����KBQ
#define INS_KBQ_QUERY_STATE_SINGLE     (0xEA) //²�ѯ���KBQ�״̬
#define INS_KBQ_SET_STATE_SINGLE       (0xEB) //���õ��KBQ�״̬
#define INS_KBQ_SET_STATE_ALL          (0xEC) //�������KBQ�״̬

//实时库数据生成服务
#define INS_CREATE_STATIONID           (0xF0) //指定站号点表文件更新命令
#define INS_CREATE_UPDATE              (0xF1) //全部数据更新完毕命令

//-------------防误总服务相关命令
#define INS_GET_WF_MAIN_SVC_CFG         (0xA1) //获取防误主服务的配置
#define INS_GET_BIG_DATA_SVC_CFG        (0xA2) //获取大数据传输服务的配置
#define INS_GET_RTDB_SVC_CFG            (0xA3) //获取实时库服务的配置
#define INS_GET_DB_SVC_CFG              (0xA4) //获取数据库服务的配置
#define INS_GET_WF_LOGIC_SVC_CFG        (0xA5) //获取五防逻辑校核服务的配置
#define INS_GET_TOPOLOGY_SVC_CFG        (0xA6) //获取拓扑校核服务的配置
#define INS_GET_TICKET_SVC_CFG          (0xA7) //获取票服务的配置
#define INS_GET_TICKET_SN_SVC_CFG       (0xA8) //获取票编号服务的配置
#define INS_GET_RTDB_CLIENT_ID_SVC_CFG  (0xA9) //获取实时库客户端ID服务的配置
#define INS_GET_SERVER_CFG				(0xAA) //获取子服务的配置

#define INS_GET_FILE_COUNT              (0xB1) //获取文件数量
#define INS_GET_FILE_LIST               (0xB2) //获取文件列表
#define INS_GET_WFC_VER                 (0xB3) //获取五防图形客户端版本号
#define INS_GET_WFC_FOLDER              (0xB4) //获取五防图形客户端文件夹路径
#define INS_GET_WFC_EXE_PATH            (0xB5) //获取五防图形客户端exe文件路径
#define INS_GET_WFC_UI_PATH             (0xB6) //获取五防图形客户端UI文件路径
#define INS_GET_APP_CFG_PATH_BY_NAME    (0xB7) //通过应用程序名称获取配置文件路径
#define INS_GET_APP_CFG_PATH            (0xB8) //获取配置文件路径
#define INS_GET_FILE_PATH               (0xB9) //获取文件路径
#define INS_GET_APP_DETAILS             (0xBA) //获取应用程序运行包的详情
#define INS_GET_STATION_COUNT           (0xBB) //获取站数量
#define INS_GET_STATION_ID_LIST         (0xBC) //获取站ID列表
#define INS_SET_SERVER_IP               (0xBD) //设置服务端IP地址
#define INS_STATION_UPDATE              (0xBE) //客户端更新了站数据

#define INS_GET_EARTHLINE_CTRL_COUNT    (0x90) //获取地线控制器数量
#define INS_GET_EARTHLINE_CTRL_CFG_LIST (0x91) //获取地线控制器配置列表

#define INS_WARNINGBOARD_OPERATE        (0x92) //标志牌操作(挂/拆标志牌)
//-----------end--防误总服务相关命令

//-------------站数据生成服务相关命令
//客户端发起命令
#define INS_CLIENT_NOTIFY_NEW_STATION          (0x21) //通知有新站数据需要生成

//服务端发起命令
#define INS_SERVER_NOTIFY_STATION_DATA_DETAILS (0x2A) //通知新站数据生成情况
//-----------end--站数据生成服务相关命令

#endif // !__P7PROTOCOLINSTYPE_H__
