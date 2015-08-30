/**************************************************************************************************
功能：定义PSM70公共使用数据类型
作者：唐小灿
说明：数据定义有变动或增加请及时更新该文件
**************************************************************************************************/


/*
实时库数据定义
*/
//实时库容器类型定义
typedef enum _RTDBContainerType
{
	RTDB_Container_Device = 1,  //设备
	RTDB_Container_Other        //其他
}ERTDBContainerType;


//实时库数据类型定义
typedef enum _RTDBValType
{
	RTDB_Val_YX = 1,            //遥信
	RTDB_Val_YC = 2,            //遥测
	RTDB_Val_Other,
}ERTDBValType;


//实时库遥信值索引对应意义定义
typedef enum _RTDBValYXIndex
{
	RTDB_ValYX_Right = 0,       //唯一操作权(即index为0的值为唯一操作权)
	RTDB_ValYX_State = 1,       //状态(即index为1的值为状态)
	RTDB_ValYX_Other
}ERTDBValYXIndex;