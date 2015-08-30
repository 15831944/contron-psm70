/**************************************************************************************************
文件名:BusinessDataType.h
功  能:本文件为权限认证模块，定义权限认证模块相关数据结构。
**************************************************************************************************/

#pragma  once
#include "platformLayer/WLDEF.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

//部门信息
typedef struct _Department
{
    DWORD32 ID;
    char name[32];
    DWORD32 UPID;
    DWORD32 type;
}TDepartment;

//人员信息
typedef struct _Staff
{
    DWORD32			ID;
    char			name[32];
    DWORD32			dptID;
    DWORD32			roleID;
    DWORD32			loginID;
    char			psw[8];
    DWORD32			defaultStation;
}TStaff;

//资源信息
typedef struct _Resource
{
    DWORD32 ID;
    DWORD32 resourceCode;   //资源编码（站编号）
    DWORD32 type;
}TResource;

//部门资源关系信息
typedef struct _DeptResourceShip
{
    DWORD32 ID;
    DWORD32 dptID;
    DWORD32 resourceID;
}TDeptResourceShip;

//功能信息
typedef struct _Function
{
    DWORD32		ID;
    DWORD32		privilegeCode;
    char		name[32];
    DWORD32		UPID;
    DWORD32		type;
}TFunction;

//角色模板信息
typedef struct _RoleTemplate
{
    DWORD32 ID;
    char name[32];
}TRoleTemplate;

//功能角色模板关系表
typedef struct _FuncRoleTemplateShip
{
    DWORD32 ID;
    DWORD32 roleTemplateID;
    DWORD32 privilegeCode;
}TFuncRoleTemplateShip;

//角色信息
typedef struct _Role
{
    DWORD32		ID;
    char		name[32];
}TRole;

//权限信息
typedef struct _Privilege
{
    DWORD32 ID;
    DWORD32 roleID;
    DWORD32 resourceCode;
    DWORD32 privilegeCode;
}TPrivilege;

//部门信息
typedef struct _Station
{
	DWORD32 ID;
	DWORD32 number;
	char name[64];
	DWORD32 taskNameTemplate;
	DWORD32 departmentID;
	DWORD32 volID;
}TStaion;

//间隔信息
typedef struct _Bay
{
	DWORD32 ID;
	DWORD32 stationID;
	char name[64];
	DWORD32 type;
	DWORD32 volID;
    DWORD32 deviceID;
    char term[256];
    char remark[256];
}TBay;

//设备信息
typedef struct _Device{
	DWORD32 ID;
	DWORD32 stationID;
	char number[64];
	char doubleNumber[128];
	DWORD32 volID;
	DWORD32 deviceTypeID;
	DWORD32 curState;
	DWORD32 bayID;
	DWORD32 termTemplateID;
	DWORD32 isLocked;
    DWORD32 isMonitor;
    DWORD32 isYX;
	DWORD32 isHaveWarningBoard;
	DWORD32 warningBoardID;
	DWORD32 isHaveBelongWarningBoard;
	DWORD32 belongWarningBoardID;
	DWORD32 isHaveEarthLine;
	DWORD32 earthLineID;
	DWORD32 isHaveBelongEarthLine;
	DWORD32 belongEarthLineID;
	otl_datetime earthLineCheck;
	DWORD32 isWorking;
	DWORD32 isRedLineDevice;
	DWORD32 isHaveBelongCommonTerm;
	DWORD32 belongCommonTermID;
	DWORD32 isHaveElectricityTerm;
	DWORD32 electricityTermID;
	DWORD32 isVirtual;
}TDevice;

// 地线控制器
typedef struct _EarthLineController
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 coding;
    char IP[32];
    DWORD32 port;
    char label[64];
} TEarthLineController;

// 地线组
typedef struct _EarthLineGroup
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 volID;
    DWORD32 coding;
    char label[64];
    DWORD32 type;
} TEarthLineGroup;

//地线表
typedef struct _EarthLine
{
	DWORD32 ID;
	DWORD32 stationID;
    DWORD32 groupID;
    DWORD32 controllerID;
	DWORD32 volID;
    DWORD32 coding;
	char label[64];
	DWORD32 state;
    DWORD32 type;
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
}TEarthLine;

// 单地线
typedef struct _SingleEarthLine
{
    DWORD32 ID;
    DWORD32 ELID;
    DWORD32 volID;
    DWORD32 coding;
} TSingleEarthLine;

// 闭锁桩
typedef struct _EarthLinePile
{
    DWORD32 ID;
    DWORD32 ELID;
    DWORD32 coding;
} TEarthLinePile;

// 地线统计
typedef struct _EarthLineStatistics
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 groupID;
    DWORD32 earthLineID;
    DWORD32 pileID;
} TEarthLineStatistics;

// 地线动作
typedef struct _EarthLineAction
{
    DWORD32 ID;
    DWORD32 ticketID;
    DWORD32 actionID;
    DWORD32 pileID;
    DWORD32 type;
    DWORD32 groupID;
    DWORD32 earthLineID;
} TEarthLineAction;

//电压表
typedef struct _Voltage
{
	DWORD32 ID;
	DWORD32 voltageValue;
	DWORD32 unit;
}TVoltage;

//标识牌表
typedef struct _WarningBoard
{
	DWORD32 ID;
	DWORD32 stationID;
	char name[64];
	DWORD32 width;
	DWORD32 height;
	DWORD32 fontColor;
	DWORD32 backgoundColor;
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
}TWarningBoard;

// 标识牌统计
typedef struct _WBStatistics
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 WBID;
    DWORD32 deviceID;
} TWBStatistics;

//红线设备表
typedef struct _RedLineDeviceTerm
{
	DWORD32 ID;
	DWORD32 stationID;
	DWORD32 beforeState;
	DWORD32 afterState;
	char term[64];
	DWORD32 isPrint;
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
    DWORD32 isUse;
}TRedLineDeviceTerm;

//常用术语组表
typedef struct _CommonTermGroup
{
	DWORD32 ID;
	DWORD32 stationID;
	char name[64];
}TCommonTermGroup;

//常用术语表
typedef struct _CommonTerm
{
	DWORD32 ID;
	DWORD32 commonTermGroupID;
	DWORD32 volID;
	DWORD32 deviceTypeID;
    char term[256];
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
}TCommonTerm;

//操作票表
typedef struct _OperateTicket
{	
	DWORD32 ID;
	DWORD32 stationID;
    char name[256];
	DWORD32 number;
    char makeTicketPerson[256];
	otl_datetime makeTicketTime;
	DWORD32 type;
    DWORD32 keyTicketType;
	DWORD32 ticketSource;
	DWORD32 bayID;
	DWORD32 volID;
    DWORD32 state;
    char stopCause[256];
	otl_datetime completeTime;
	DWORD32 operateWayBeginState;
	DWORD32 operateWayEndState;
	DWORD32 auditState;
    DWORD32 operater;
    DWORD32 guardian;
    DWORD32 dutyManager;
	char sendUnit[64];
	char sendPerson[16];
	char recvUnit[64];
	char recvPerson[16];
	DWORD32 dangerDescriptionID;
}TOperateTicket;

//操作票动作表
typedef struct _TicketActionList
{
	DWORD32 ID;
	DWORD32 ticketID;
	DWORD32 index;
	DWORD32 deviceID;
	DWORD32 isCompleted;
	char conditions[128];
	char action[32];
	DWORD32 beforeState;
	DWORD32 afterState;
	otl_datetime operateTime;
	DWORD32 isMonitor;
    DWORD32 isTicketItem;
    DWORD32 isKeyItem;
    DWORD32 keyItemIndex;
    DWORD32 isLinkItem;
    DWORD32 linkItemIndex;
    DWORD32 actionType;
    DWORD32 WBID;
    DWORD32 lockCode;
}TTicketActionList;

//强制对位设备表
typedef struct _ForceDuiWei
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 deviceID;
    DWORD32 staffID;
    DWORD32 beforeState;
    DWORD32 afterState;
    otl_datetime time;
}TForceDuiWei;

//操作票项
typedef struct _TicketItemList
{
	DWORD32 ID;
    DWORD32 ticketID;
	DWORD32 ticketActionID;
	DWORD32 index;
	DWORD32 itemType;
    char itemContent[256];
	DWORD32 isPrint;
	DWORD32 itemShipType;
	DWORD32 subItemIndex;
    DWORD32 isCompleted;
    DWORD32 isTicketItem;
    DWORD32 isKeyItem;
    DWORD32 keyItemIndex;
    DWORD32 isLinkItem;
    DWORD32 linkItemIndex;
    DWORD32 isToKey;
    DWORD32 promptType;
}TTicketItemList;

//钥匙项表
typedef struct _KeyItemList
{
	DWORD32 ID;
	DWORD32 ticketActionID;
	DWORD32 index;
	DWORD32 itemType;
    char itemContent[256];
    DWORD32 isCompleted;
}TKeyItemList;

//任务名模板表
typedef struct _TaskNameTemplate
{
	DWORD32 ID;
	DWORD32 taskTypeID;
    char templateContent[256];
}TTaskNameTemplate;

//任务类型信息
typedef struct _TaskType
{
	DWORD32 ID;
	char name[32];
}TTaskType;

//设备类型表
typedef struct _DeviceType
{
	DWORD32 ID;
	DWORD32 type;
	DWORD32 propertys;
	char name[32];
	DWORD32 stateCount;
}TDeviceType;

//设备联动表
typedef struct _LinkDevice
{
	DWORD32 ID;
	DWORD32 stationID;
	DWORD32 masterDeviceID;
	DWORD32 beforeStateOfMastor;
	DWORD32 afterStateOfMastor;
	DWORD32 slaveDeviceID;
	DWORD32 beforeStateOfSlave;
	DWORD32 afterStateOfSlave;
    DWORD32 isMainDeviceBefore;
}TLinkDevice;

//五防逻辑表
typedef struct _LDataLogic
{
	DWORD32 ID;
	DWORD32 stationID;
	DWORD32 deviceID;
	DWORD32 actionType;
}TLDataLogic;

//五防逻辑关系表
typedef struct _LDataLogicShip
{
	DWORD32 ID;
	DWORD32 lDataLogicID;
	char condition[64];
    char expression[256];
	DWORD32 index;	//add 2014-08-06
}TLDataLogicShip;

//操作逻辑表
typedef struct _RDataLogic
{
	DWORD32 ID;
	DWORD32 stationID;
	DWORD32 deviceID;
	DWORD32 actionType;
}TRDataLogic;

//操作逻辑关系表
typedef struct _RDataLogicShip
{
	DWORD32 ID;
	DWORD32 rDataLogicID;
	char condition[64];
    char expression[256];
	DWORD32 index;	//add 2014-08-06
}TRDataLogicShip;

//验电术语表
typedef struct _ElectricityTerm
{
	DWORD32 ID;
    char electricityTerm[256];
}TElectricityTerm;

//专家术语模板
typedef struct _TermTemplate
{
	DWORD32 ID;
    DWORD32 stationID;
	char name[32];
	DWORD32 index;
    char remark[256];
}TTermTemplate;

//操作描述表
typedef struct _OperateDescription
{
	DWORD32 ID;
//	DWORD32 stationID;
	DWORD32 termTemplateID;
	DWORD32 volID;
	DWORD32 beforeState;
	DWORD32 afterState;
    char description[32];
}TOperateDescription;

//操作术语表
typedef struct _OperateTerm
{
	DWORD32 ID;
	DWORD32 operateDescriptionID;
	DWORD32 operateType;
    char operateTerm[256];
	char conditions[128];
	DWORD32 itemShipType;
	DWORD32 isPrint;
	DWORD32 index;
	DWORD32 isBelongStation;
	DWORD32 belongStationID;
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
}TOperateTerm;

//锁具表
typedef struct _Lock
{
	DWORD32 ID;
	DWORD32 lockType;
	DWORD32 lockCodeType;
    DWORD32 lockCode;
	char rfidCode[32];
	DWORD32 deviceID;
}TLock;

//危险点描述表
typedef struct _DangerPointDescp
{
	DWORD32 ID;
	DWORD32 deviceTypeID;
	DWORD32 stationID;
	DWORD32 volID;
    char description[256];
	DWORD32 beforeState;
	DWORD32 afterState;
}TDangerPointDescp;

//危险点术语表
typedef struct _DangerPointTerm
{
	DWORD32 ID;
	DWORD32 dangerPointDescriptionID;
	char dangerPoint[128];
    char preventMeasure[256];
	DWORD32 index;
	DWORD32 isBelongDevice;
	DWORD32 belongDeviceID;
}TDangerPointTerm;

//专用危险点表
typedef struct _BelongDangerPoint 
{
	DWORD32 ID;
	char operateItem[32];
	char dangerPoint[128];
    char preventMeasure[256];
}TBelongDangerPoint ;

//大数据服务配置表
typedef struct _DataServiceConfig
{
	DWORD32 ID;
	char IP[16];
	DWORD32 port;
}TDataServiceConfig;

//客户端路径配置表
typedef struct _ClientPathConfig
{
	DWORD32 ID;
	char name[64];
    DWORD32 appType;
	DWORD32 system;
	DWORD32 systemVersion;
	DWORD32 programVersion;
	DWORD32 isForceUpdate;
    char path[256];
    DWORD32 isFile;
}TClientPathConfig;

//配置文件路径配置表
typedef struct _ConfigfilePathConfig
{
	DWORD32 ID;
    DWORD32 type;
	char path[256];
    DWORD32 isFile;
}TConfigfilePathConfig;

//图形文件路径配置表
typedef struct _UifilePathConfig
{
	DWORD32 ID;
	DWORD32 number;
    char path[256];
    DWORD32 isFile;
}TUifilePathConfig;

//设备点表文件路径配置表
typedef struct _RecordPathConfig
{
    DWORD32 ID;
    DWORD32 number;
    char path[256];
    DWORD32 isFile;
}TRecordPathConfig;

//防误校核文件路径配置表
typedef struct _LdataPathConfig
{
    DWORD32 ID;
    DWORD32 number;
    char path[256];
    DWORD32 isFile;
}TLdataPathConfig;

//实时库数据文件路径配置表
typedef struct _RTDBPathConfig
{
    DWORD32 ID;
    DWORD32 number;
    char path[256];
    DWORD32 isFile;
}TRTDBPathConfig;

//钥匙打包文件路径配置表
typedef struct _KeyInitPathConfig
{
    DWORD32 ID;
    DWORD32 number;
    char path[256];
    DWORD32 isFile;
}TKeyInitPathConfig;

// 模拟参数选项表
typedef struct _SimulateParams
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 number;
    char content[256];
    DWORD32 isEnabled;
} TSimulateParams;

//文件路径表
typedef struct _FilePath
{
    DWORD32 ID;
    DWORD32 number;
    DWORD32 fileType;
    char path[256];
    DWORD32 isFile;
}TFilePath;

//系统文件部署路径表
typedef struct _SystemArrangePath
{
    DWORD32 ID;
    DWORD32 pathType;
    char path[256];
    DWORD32 isFile;
}TSystemArrangePath;

//系统日志表
typedef struct _SystemLog
{
    DWORD32 ID;
    DWORD32 staffID;
    otl_datetime time;
    DWORD32 type;
}TSystemLog;

//设备日志表
typedef struct _DeviceLog
{
    DWORD32 ID;
    DWORD32 stationID;
    DWORD32 staffID;
    DWORD32 deviceID;
    DWORD32 beforeState;
    DWORD32 afterState;
    DWORD32 type;
    otl_datetime time;
}TDeviceLog;

//打印参数,结束方式(张林颢增加)
enum EndType
{
    EndType_Text = 0,            //以文字结束
    EndType_Pic                  //以图片结束
};

//操作票打印参数设置(张林颢增加)
typedef struct _PrintParam{
    DWORD32	ID;
    DWORD32 stationID;           //站号
    char    bodyFontFamily[64];  //字体
    DWORD32 fontSize;            //字号
    DWORD32 indexCol;            //序号所在列
    DWORD32 contentsCol;         //票项所在列
    DWORD32 isFixedTicketName;   //自定义操作票名
    char    fixedTicketName[64]; //操作票名称
    DWORD32 isfirstLineGetUp;    //启用上接
    char    getUpText[64];       //上接文本
    DWORD32 isLastLineGetDown;   //启用下接
    char    getDownText[64];     //下接文本
    DWORD32 isFirstLineIndex;    //首行带序号
    DWORD32 isFirstLineFlag;     //显示首行文本
    char    firstLineText[64];   //首行文本
    DWORD32 isLastLineIndex;     //末行带序号
    DWORD32 isLastLineFlag;      //显示末行文本
    char    lastLineText[64];    //末行文本
    DWORD32 isEndFlag;           //结束标识
    EndType endType;             //以文本结束/以图片结束
    char    endText[64];         //结束文本
    char    dateTimeFaram[32];   //时间格式
    char    currentPageFaram[32];//当前页码文本格式
    char    pagesCountFaram[32]; //总页数文本格式
    DWORD32 isPrintTicketName;   //每页总打印任务名称
    char    endPicPath[MAX_PATH];//图片路径
}TPrintParam;

//编号递增方式(张林颢增加)
enum NumAddType
{
    NumAddType_Page = 0,            //按页递增
    NumAddType_Task,                //按任务递增
    NumAddType_Manual               //手动填写
};

//编号清零方式(张林颢增加)
enum NumClearType
{
    NumClearType_PerYear = 0,       //按年清零
    NumClearType_PerMonth           //按月清零
};

//编号规则设置(张林颢增加)
typedef struct _TicketSNParam{
    DWORD32	ID;
    DWORD32 stationID;           //站号:0表示集控,其他为单站
    NumAddType addType;          //递增方式
    NumClearType clearType;      //清零方式
    DWORD32 currentNum;          //当前流水号
    DWORD32 numDigits;           //流水号位数
    char    SNParam[128];        //编号格式
}TTicketSNParam;

//操作票打印历史项
typedef struct _TicketPrintHistoryItem{
	DWORD32 ID;
	DWORD32 ticketID;           //操作票号
	DWORD32 stationID;          //站号:0表示集控,其他为单站
	DWORD32 pageCount;          //打印页数
	DWORD32 ticketPrintID;      //操作票打印流水号
    otl_datetime time;          //打印成功时间戳
}TTicketPrintHistoryItem;

//数据库备份日志表
typedef struct _DBBackupLog
{
    DWORD32 ID;
    DWORD32 staffID;
    DWORD32 type;
    char fileName[64];
    otl_datetime time;
    char reason[255];
}TDBBackupLog;

//黑匣子票日志表
typedef struct _BlackBoxTicLog
{
    DWORD32 ID;
    DWORD32 ticketID;
    DWORD32 stationNumber;
    DWORD32 ticketProperty;
    DWORD32 stepCount;
    otl_datetime createTime;
    otl_datetime endTime;
}TBlackBoxTicLog;

//黑匣子票操作日志表
typedef struct _BlackBoxTicOperateLog
{
    DWORD32 ID;
    DWORD32 ticketID;
    DWORD32 index;
    DWORD32 deviceID;
    DWORD32 lockType;
    DWORD32 deviceProperty;
    DWORD32 action;
    char rfidCode[32];
    DWORD32 operateCondition;
    otl_datetime operateTime;
}TBlackBoxTicOperateLog;

//图形文件版本表
typedef struct _UIFileVersion
{
    DWORD32 ID;
    DWORD32 stationNum;
    DWORD32 version;
}TUIFileVersion;
