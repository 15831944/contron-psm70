#ifndef PSM_DEF_H
#define PSM_DEF_H

#include "psm_device.h"

typedef struct _psm_config
{
    char        *wf_ip;         //五防总服务地址
    int         wf_port;        //五防总服务端口
    int         rt_id;          //实时库ID，人工保证不能重复
} psm_config_t;


/*!
 * \brief 设备
 * -------------------------------------------
 * 设备状态定义：
 * -------------------------------------------
 * 值    |   开关  |   小车  |   地线  |   网门
 * -------------------------------------------
 *  0    |   分   |  检修   |  拆   |   关
 * -------------------------------------------
 *  1    |   合   |  运行   |  挂   |   开
 * -------------------------------------------
 *  2    |        |  试验   |       |
 * -------------------------------------------
 *  3    |        |  错误   |       |
 * -------------------------------------------
 *
 */
typedef struct _jk_device
{
    UCHAR   device_len;                     //设备主编号长度
    char    device[MAX_DEVICE_LEN];         //设备主编号
    UCHAR   state_1;                        //状态一，不同场景定义如下：
                                            //1.全状态：设备状态，见设备状态定义
                                            //2.设备操作:1-合操作，0-分操作
                                            //3.操作票：操作前状态，见设备状态定义
                                            //4.遥控闭锁继电器状态：继电器类型，0-分位继电器，1-合位继电器，2-合分位共用一个继电器
    UCHAR   state_2;                        //状态二，不同场景定义如下：
                                            //1.全状态：设备闭锁状态，0-解锁，1-闭锁
                                            //2.设备操作结果:1-成功，2-失败
                                            //3.操作票：操作后状态，见设备状态定义
                                            //4.遥控闭锁继电器状态：继电器状态，0-闭锁状态，1-解锁状态，2-接地状态

} jk_device_t;

/*!
 * \brief 设备列表
 */
typedef struct _jk_device_list
{
    int             count;      //设备数量
    jk_device_t     *device;    //多个设备
} jk_device_list_t;

typedef struct _jk_time
{
    USHORT      year;
    UCHAR       month;
    UCHAR       day;
    UCHAR       hour;
    UCHAR       minute;
    UCHAR       second;
} jk_time_t;

typedef void (*psm_request_data)(int);
typedef void (*psm_device)(int, int, char *, int);
typedef void (*psm_device_result)(int, int, char *, int, int);
typedef void (*psm_device_list)(int, jk_device_list_t *);
typedef void (*psm_sync_time)(int, jk_time_t *);
typedef void (*psm_device_all)(int, int);

#endif // PSM_DEF_H

