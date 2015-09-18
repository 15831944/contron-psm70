/***********************************************************************
功能：定义图形与票服务程序集合(通信包括票服务，票执行就地程序)通信协议内容
注意：该文件定义内容暂时限于提供给图形跟票执行就地程序的DLL使用
作者：唐小灿  2014-09-12 消息结构采用WLProtocolDataType.h的定义
************************************************************************/
#ifndef __CHARTTICKETSERVDATATYPE_H__
#define __CHARTTICKETSERVDATATYPE_H__

#include "ExtraDef.h"


//向图形汇报钥匙操作票执行情况
typedef struct _stTicketResult
{
	DWORD32 ticketID;
	int     ticketState;
}TTicketResult;

/*
命令码	功能
0X03	传送任务（采码票、检修票、普通票）
0X33	回传任务（采码票、检修票、普通票）
0X04	查询任务信息
0X05	请求回传日志
0X15	回传日志
0X06	清除任务
0X07	请求回传采码记录
0x17	回传采码记录
0x08	查询钥匙版本
0x09	校时钥匙
0x1A	下传采码记录
0x1B	查询钥匙状态信息
0x0E	查询适配器软件版本号
0x0F	配置适配器通讯口
0x11	查询钥匙信息
0x12	查询钥匙在位信息

result值 	result定义
0x00	接收命令不成功
0x01	接收命令并执行成功
0x02	无效的命令码
0x03	校验码错误
0x04	字节数不符
0x05	文件过大
0x06	任务数已满（应用范围：传送任务，钥匙最多只有5个任务）
0x07	任务票GUID重复（应用范围：传送任务）
0x08	任务票GUID不存在（应用范围：清除任务、请求日志回传、请求回传采码记录）
0x09	通讯失败
0x0A	取消（终止）当前操作成功
0x0B	(适配器)boot版本不支持升级
0x0C	(适配器终端Ymodem模式升级）文件接收错误
*/
//向图形汇报钥匙执行情况
typedef struct _KeyExecResult
{
    DWORD32 ticketID;           //操作票ID（执行与操作票无关的命令时，操作票ID为0）
    DWORD32 cmdCode;            //命令码（见上面定义）
    DWORD32 result;             //命令执行结果（见上面定义）
}TKeyExecResult;

//向图形汇报钥匙在线情况
typedef struct _KeyOnlineState
{
	BYTE keyAOnline;                //钥匙座A是否在线，0不在线；1在线
	BYTE keyBOnline;                //钥匙座B是否在线，0不在线；1在线
}TKeyOnlineState;

#endif
