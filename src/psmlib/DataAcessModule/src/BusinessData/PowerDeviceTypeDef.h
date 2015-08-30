/**************************************************************************************************
文件名:PowerDeviceTypeDef.h
功  能:本文件对电力系统中的图形中所有一次设备、二次设备、以及辅助设备的设备类型统一定义
作  者：李施施 2014-08-12
**************************************************************************************************/
#pragma once

#define POWER_DEV_TYPE_RFU							0x00		//系统预留
#define POWER_DEV_TYPE_BREAKER						0x01		//开关类
#define POWER_DEV_TYPE_DISCONNECTOR					0x02		//隔离类
#define POWER_DEV_TYPE_GROUND_DISCONNECTOR			0x03		//地刀类
#define POWER_DEV_TYPE_SHIELD_AREA_GATE				0x04		//网门类
#define POWER_DEV_TYPE_HANDCART						0x05		//小车（三态）类
#define POWER_DEV_TYPE_HANDCART_EXT					0x06		//小车（两态）类
#define POWER_DEV_TYPE_HANDCART_DISCONNECTOR		0x07		//小车刀闸类
#define POWER_DEV_TYPE_HANDCART_BREAKER				0x08		//小车开关类
#define POWER_DEV_TYPE_FUSE							0x09		//跌落熔丝
#define POWER_DEV_TYPE_GROUND_LAMP					0x0A		//地线灯
#define POWER_DEV_TYPE_CLEARANCE_TAG				0x0B		//工作牌
#define POWER_DEV_TYPE_WIREPOLE						0x0C		//电杆
#define POWER_DEV_TYPE_CABLE_CONNECTION_SINGLE		0x0D		//电缆头（单）
#define POWER_DEV_TYPE_CABLE_CONNECTION_DOUBLE		0x0E		//电缆头（双）
#define POWER_DEV_TYPE_GND							0x0F		//接地
#define POWER_DEV_TYPE_BUS							0x10		//母线
#define POWER_DEV_TYPE_ROUTE						0x11		//线路
#define POWER_DEV_TYPE_LOAD							0x12		//负荷
#define POWER_DEV_TYPE_TRANSFORM					0x13		//变压器
#define POWER_DEV_TYPE_REACTOR						0x14		//电抗
#define POWER_DEV_TYPE_ARRESTOR						0x15		//避雷器
#define POWER_DEV_TYPE_TSTATION						0x16		//T接
#define POWER_DEV_TYPE_EARTHLINE					0x17		//地线
#define POWER_DEV_TYPE_POWER                        0x18        //发电机/电源
#define POWER_DEV_TYPE_LOCK                         0x19        //锁具

#define BREAKER_UNDEFINE							0x00		//【开关类】 未明确指定
#define BREAKER_COM									0x01		//【开关类】 断路器
#define BREAKER_RECT								0x02		//【开关类】 方形开关
#define BREAKER_KC									0x03		//【开关类】 开关KC

#define DISCONNECTOR_UNDEFINE						0x00		//【隔离类】 未明确指定
#define DISCONNECTOR_COM							0x01		//【隔离类】 隔离开关
#define DISCONNECTOR_POL							0x02		//【隔离类】 柱上刀闸
#define DISCONNECTOR_TRITERMINAL							0x03		//【隔离类】 三端刀闸
#define DISCONNECTOR_TRITERMINAL_NEW						0x04		//【隔离类】 新三端刀闸
#define DISCONNECTOR_TRIHOTPOINT							0x05		//【隔离类】 三热点刀闸
#define DISCONNECTOR_LINK								0x06		//【隔离类】 活动刀闸
#define DISCONNECTOR_GND									0x07		//【隔离类】 带接地刀闸
#define DISCONNECTOR_LINK_LOCK							0x08		//【隔离类】 连锁刀闸

#define CROUND_DISCONNECTOR_UNDEFINE				0x00		//【地刀类】 未明确定义
#define CROUND_DISCONNECTOR_COM						0x01		//【地刀类】 接地刀闸
#define CROUND_DISCONNECTOR_TRIHOTPOINT				0x02		//【地刀类】 三热点地刀
#define CROUND_DISCONNECTOR_PILE					0x03		//【地刀类】 接地桩

#define SHIELD_AREA_GATE_UNDEFINE					0x00		//【网门类】 未明确定义

#define HANDCART_UNDEFINE							0x00		//【小车三态类】 未明确定义
#define HANDCART_SINGLE_TERMINAL					0x01		//【小车三态类】 单头
#define HANDCART_COM								0x02		//【小车三态类】 双头

#define HANDCART_EXT_UNDEFINE						0x00		//【小车两态类】 未明确定义
#define HANDCART_EXT_SINGLE_TERMINAL				0x01		//【小车两态类】 单头
#define HANDCART_EXT_COM							0x02		//【小车两态类】 双头

#define WIREPOLE_UNDEFINE							0x00		//【电杆】 未明确指定
#define WIREPOLE_POLE								0x01		//【电杆】 杆塔
#define WIREPOLE_IRON_TOWER							0x02		//【电杆】 铁塔

#define ROUTE_UNDEFINE								0x00		//【线路】 未明确指定
#define ROUTE_COM									0x01		//【线路】 普通
#define ROUTE_CABLE									0x02		//【线路】 电缆

#define LOAD_UNDEFINE								0x00		//【负荷】 未明确指定
#define LOAD_COM									0x01		//【负荷】 普通
#define LOAD_POWER									0x02		//【负荷】 反送电

#define LOCK_UNDEFINE                               0x00        //【锁具】 未明确指定
#define LOCK_KBQ                                    0x81        //【锁具】 KBQ锁具


/********************************************************
 * 以下由佘佳明添加
 *******************************************************/
typedef enum _AbstractDeviceType                    // 设备抽象类
{
    ADT_WF= 0,                                      // 五防设备类
    ADT_CL,                                         // 潮流类
    ADT_BYQ,                                        // 变压器类
    ADT_CLYH,                                       // 潮流优化类
    ADT_ECFZ,                                       // 二次辅助类
    ADT_ECYBBH,                                     // 二次压板保护类
    ADT_OTHER                                       // 其它

} EAbstractDeviceType;

typedef enum _PowerDeviceType                       // 设备大类
{
    PDT_BREAKER = 0,                                // 开关类
    PDT_DISCONNECTOR,                               // 隔离类
    PDT_GROUND_DISCONNECTOR,                        // 地刀类
    PDT_SHIELD_AREA_GATE,                           // 网门类
    PDT_HANDCART,                                   // 小车（三态）类
    PDT_HANDCART_EXT,                               // 小车（两态）类
    PDT_HANDCART_DISCONNECTOR,                      // 小车刀闸类
    PDT_HANDCART_BREAKER,                           // 小车开关类
    PDT_FUSE,                                       // 跌落熔丝
    PDT_GROUND_LAMP,                                // 地线灯
    PDT_CLEARANCE_TAG,                              // 工作牌
    PDT_WIREPOLE,                                   // 电杆
    PDT_CABLE_CONNECTION_SINGLE,                    // 电缆头（单）
    PDT_CABLE_CONNECTION_DOUBLE,                    // 电缆头（双）
    PDT_GND,                                        // 接地
    PDT_BUS,                                        // 母线
    PDT_ROUTE,                                      // 线路
    PDT_LOAD,                                       // 负荷
    PDT_TRANSFORM,                                  // 变压器
    PDT_REACTOR,                                    // 电抗
    PDT_ARRESTOR,                                   // 避雷器
    PDT_TSTATION,                                   // T接
    PDT_EARTHLINE,                                  // 地线
    PDT_POWER,                                      // 发电机
	PDT_LOCK,                                       // 锁具
    PDT_OTHER                                       // 其它
} EPowerDeviceType;

typedef enum _DetailDeviceType                      // 设备大类下的详细设备类
{
    DDT_BREAKER_UNDEFINE = 0,                       // 【开关类】 未明确指定
    DDT_BREAKER_COM,                                // 【开关类】 断路器
    DDT_BREAKER_RECT,                               // 【开关类】 方形开关
    DDT_BREAKER_KC,                                 // 【开关类】 开关KC
    DDT_DISCONNECTOR_UNDEFINE,                      // 【隔离类】 未明确指定
    DDT_DISCONNECTOR_COM,                           // 【隔离类】 隔离开关
    DDT_DISCONNECTOR_POL,                           // 【隔离类】 柱上刀闸
    DDT_DISCONNECTOR_TRITERMINAL,                   // 【隔离类】 三端刀闸
    DDT_DISCONNECTOR_TRITERMINAL_NEW,               // 【隔离类】 新三端刀闸
    DDT_DISCONNECTOR_TRIHOTPOINT,                   // 【隔离类】 三热点刀闸
    DDT_DISCONNECTOR_LINK,                          // 【隔离类】 活动刀闸
    DDT_DISCONNECTOR_GND,                           // 【隔离类】 带接地刀闸
    DDT_DISCONNECTOR_LINK_LOCK,                     // 【隔离类】 连锁刀闸
    DDT_CROUND_DISCONNECTOR_UNDEFINE,               // 【地刀类】 未明确定义
    DDT_CROUND_DISCONNECTOR_COM,                    // 【地刀类】 接地刀闸
    DDT_CROUND_DISCONNECTOR_TRIHOTPOINT,            // 【地刀类】 三热点地刀
    DDT_CROUND_DISCONNECTOR_PILE,                   // 【地刀类】 接地桩
    DDT_SHIELD_AREA_GATE_UNDEFINE,                  // 【网门类】 未明确定义
    DDT_HANDCART_UNDEFINE,                          // 【小车三态类】 未明确定义
    DDT_HANDCART_SINGLE_TERMINAL,                   // 【小车三态类】 单头
    DDT_HANDCART_COM,                               // 【小车三态类】 双头
    DDT_HANDCART_EXT_UNDEFINE,                      // 【小车两态类】 未明确定义
    DDT_HANDCART_EXT_SINGLE_TERMINAL,               // 【小车两态类】 单头
    DDT_HANDCART_EXT_COM,                           // 【小车两态类】 双头
    DDT_HANDCART_DISCONNECTOR_UNDEFINE,             // 【小车刀闸类】 未明确定义
    DDT_HANDCART_BREAKER_UNDEFINE,                  // 【小车开关类】 未明确定义
    DDT_FUSE_UNDEFINE,							    // 【跌落熔丝类】 未明确定义
    DDT_GROUND_LAMP_UNDEFINE,                       // 【地线灯类】 未明确定义
    DDT_CLEARANCE_TAG_UNDEFINE,                     // 【工作牌类】 未明确定义
    DDT_WIREPOLE_UNDEFINE,                          // 【电杆】 未明确指定
    DDT_WIREPOLE_POLE,                              // 【电杆】 杆塔
    DDT_WIREPOLE_IRON_TOWER,                        // 【电杆】 铁塔
    DDT_CABLE_CONNECTION_SINGLE_UNDEFINE,           // 【电缆头（单）】 未明确指定
    DDT_CABLE_CONNECTION_DOUBLE_UNDEFINE,           // 【电缆头（双）】 未明确指定
    DDT_GND_UNDEFINE,                               // 【接地】 未明确指定
    DDT_BUS_UNDEFINE,                               // 【母线】 未明确指定
    DDT_ROUTE_UNDEFINE,                             // 【线路】 未明确指定
    DDT_ROUTE_COM,                                  // 【线路】 普通
    DDT_ROUTE_CABLE,                                // 【线路】 电缆
    DDT_LOAD_UNDEFINE,                              // 【负荷】 未明确指定
    DDT_LOAD_COM,                                   // 【负荷】 普通
    DDT_LOAD_POWER,                                 // 【负荷】 反送电
    DDT_TRANSFORM_UNDEFINE,                         // 【变压器】 未明确指定
    DDT_REACTOR_UNDEFINE,                           // 【电抗】 未明确指定
    DDT_DARRESTOR_UNDEFINE,                         // 【避雷器】 未明确指定
    DDT_TSTATION_UNDEFINE,                          // 【 T接】 未明确指定
    DDT_EARTHLINE_UNDEFINE,                         // 【地线】 未明确指定
    DDT_POWER_UNDEFINE,                             // 【发电机】 未明确指定
	DDT_LOCK_UNDEFINE,                              // 【锁具】 未明确指定
	DDT_LOCK_KBQ,                                   // 【锁具】 KBQ锁
    DDT_OTHER                                       // 【其它】
} EDetailDeviceType;
