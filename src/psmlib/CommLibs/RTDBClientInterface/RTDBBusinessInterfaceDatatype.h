/********************************************************************************************************************
功能：定义实时库业务接口数据类型。
说明：
作者：叶文林	2013-12-26
********************************************************************************************************************/

#if !defined(__RTDB_BUSINESS_INTERFACE_DATATYPE_H__)
#define __RTDB_BUSINESS_INTERFACE_DATATYPE_H__

#include "ExtraDef.h"
#include <stddef.h>

//客户端权限，多个权限可累加，如：PRIVILEGE_USER_DATA | PRIVILEGE_RECV_MSG同时拥有操作数据及接受消息的权限
#define PRIVILEGE_USER_DATA_EX            (0x00000001) //使用实时库数据权限
#define PRIVILEGE_RECV_MSG_EX             (0x00000002) //接受消息权限
#define PRIVILEGE_NODATA_UPDATEVAL_EX     (0x00000004) //不使用数据但允许通过innerID更新值

//值修改方式
enum ENUM_GC_RLTVAL_MODIFYMODE
{
	e_gc_rltval_modifymode_common = 0,  //普通修改
	e_gc_rltval_modifymode_force  = 1,  //强制修改
};

typedef union _ValueEx{
	DWORD64 val64;                    //8字节无符号整型值，一般放电度累加值
	struct{
		DWORD32 var32High;            //4字节无符号整型值，表示某一个值的高四字节，一般无效
		DWORD32 var32Low;             //4字节无符号整型值，表示某一个值的低四字节，一般放遥信等
	};
	struct{
		BYTE varByte7;                //1字节定点值，最高字节，以下如此但是字节位递减
		BYTE varByte6; 
		BYTE varByte5; 
		BYTE varByte4; 
		BYTE varByte3; 
		BYTE varByte2; 
		BYTE varByte1; 
		BYTE varByte0; 
	};
	double  valDbl;                   //8字节浮点值
	struct{
		float varFltHigh;             //4字节浮点值，表示某一个值高四字节部份，几乎无效
		float varFltLow;              //4字节浮点值，表示某一个值低四字节部份
	};
}TGCValue;

//容器将组成一棵多叉树结构，且兄弟节点必须连续存放，树必须按层存放
typedef struct _containerEx{
	DWORD32 innerID;                      //内部ID，本质上存当前容器在数据区的偏移量用于业务系统找到容器时快速匹配【增加：唐小灿 2014-01-05】
	DWORD32 ID;                           //ID
	DWORD32 type;                         //类型				除设备外,其他容器都为2;设备为 1
	DWORD32 NO;                           //编号，意义不指定，由业务系统决定。一般来说 站号 可作为编号存储
	DWORD32 parentID;                     //父容器ID
	DWORD32 parentType;                   //父容器type
	DWORD32 childCount;                   //子容器数量
}TGCContainer;

typedef struct _indexRltValueEx{
	DWORD32 type;                         //值类型
	DWORD32 valueCount;				      //八字节实时值数量
}TGCIndexRltValue;

typedef struct _rltValueEx{
	DWORD32 type;                         //值类型、由卢工定义 暂时不区分 计算值和测量值 定义为 1
	//DWORD32 parentID;                     //父容器ID
	//DWORD32 parentType;                   //父容器type
	DWORD32 updateSrcID;                  //更新源ID
	DWORD32 quality;                      //品质，如被闭锁，被人工置位、可疑等等
	DWORD64 tickCount;                    //记时器（时间戳）增加时间:2014-10-23
	TGCValue  var;                          //值（由type决定值是如何存放的）
}TGCRltValue;

//数据区标识
typedef struct _DataAreaID{
	DWORD32 type;                              //定义站外区、站区（变电站、发电厂、线路等），由卢工统一定义
	DWORD32 NO;                                //区号（等同于容器ID号），本字段 与 type 共同组成键                   
}TGCDataAreaID;


//品质BYTE3(存储位置从右至左)
typedef struct _stGCQualityBYTE3
{
	BYTE isUseQuality : 1;    //是否启用品质说明，0不启用，1启用
	BYTE rfu1         : 1;
	BYTE rfu2         : 1;
	BYTE rfu3         : 1;
	BYTE isLock       : 1;    //是否被封锁，0未被封锁，1被封锁
	BYTE isDisplace   : 1;    //是否被取代，0未被取代，1,被取代
	BYTE isNotCurVal  : 1;    //是否非当前值，0当前值，1非当前值
	BYTE isNotValid   : 1;    //是否无效，0有效，1无效
}TGCQualityBYTE3;
//品质BYTE2
typedef struct _stGCQualityBYTE2
{
	BYTE isManualUnlock   : 1;  //人工解锁，0不启用，1启用
	BYTE isKeyUnlock      : 1;  //钥匙回传解锁，0不启用，1启用
	BYTE isActualYXUnlock : 1;  //实遥信解锁，0不启用，1启用
	BYTE rfu1             : 1;
	BYTE rfu2             : 1;
	BYTE rfu3             : 1;
	BYTE isManualModify   : 1;  //是否人工置位，0非人工置位，1人工置位
	BYTE YXModify         : 1;  //遥信置位，0实遥信置位，1虚遥信置位
}TGCQualityBYTE2;
//实时值中品质定义，小端处理
typedef struct _stGCRltValQuality
{
	BYTE rfu1;
	BYTE rfu2;
	TGCQualityBYTE2 byte2;
	TGCQualityBYTE3 byte3;
}TGCRltValQuality;

//实时值变化时通知回调函数  //int CGSetRltValue(void * pSvc,TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TValue * pVal);
typedef int (* TGCCallbackOnRltUpdateEx)(TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TGCRltValue * pVal,void * pContext,DWORD32 breakFaceNo);

//消息中转通知回调函数
typedef int (* TGCCallbaskOnMsgTransmit)(void * pData, int len);

#endif