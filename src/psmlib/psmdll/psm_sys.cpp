#include "psm_sys.h"

#include "RTDBBusinessInterface.h"
#include "ExtraDef.h"
#include "WFClientDLLInterface.h"
#include "BigDataTransClientInterface.h"
#include "DSRecord.h"
#include "TicketRelateExDataType.h"
#include "TicketRelateExInterface.h"
#include "RTDBYKDLLInterface.h"

#include "psm_intf.h"
#include "psm_file.h"

#include "config.h"


//void *wf = NULL;
//void *rt = NULL;
//void *bd = NULL;
//device_t *list = NULL;


#define print_function(function_name, call_result) \
    printf("%s = %d\n", #function_name, call_result);

#define print_function_int(function_name, call_result) \
    printf("%s = %d\n", #function_name, call_result); \
    if(0>call_result) return NUMBER_FALSE;


#define print_function_null(function_name, call_result, ret_val) \
    if(NULL==call_result) \
    { \
        printf(#function_name" = NULL\n"); \
        return ret_val; \
    }

typedef struct _psm_exec_item
{
    int                 enable;
    int                 station_id;
    DWORD32             device_id;
    DWORD32             exec_id;
    double              exec_time;
} psm_exec_t;

typedef struct _psm_exec_list
{
    int                 count;
    psm_exec_t          *item;
} psm_exec_list_t;

typedef struct _psm_kbq_item
{
    int                 enable;         //启用
    device_t            *kbq;           //KBQ设备
} psm_kbq_item_t;

typedef struct _psm_kbq
{
    psm_kbq_item_t      open;           //分,1->0
    psm_kbq_item_t      close;          //合,0->1
} psm_kbq_t;

typedef struct psm_yx_item
{
    int                 enable;         //启用
    int                 reverse;        //取反,共创网门设备状态需取反(与集控规约定义相反),小车设备状态取反(1不变,0和2互换)
    int                 has_3_state;    //三态,如小车
} psm_yx_item_t;

typedef struct _psm_yx
{
    psm_yx_item_t       recieve;        //接收
    psm_yx_item_t       send;           //发送
} psm_yx_t;

typedef struct _psm_yk_item
{
    int                 enable;
} psm_yk_item_t;

typedef struct _psm_yk
{
    psm_yk_item_t       recieve;
    psm_yk_item_t       send;
} psm_yk_t;

typedef struct _psm_device_extend
{
    psm_kbq_t           kbq;            //KBQ数据
    psm_yx_t            yx;             //遥信数据
    psm_yk_t            yk;             //遥控数据
    DWORD32             ticket_id;      //遥控票号
} psm_device_extend_t;

typedef struct _psm_station
{
    int                 station_no;
    int                 device_count;
    device_t            *device;
    TGCDataAreaID       *area;
    device_t            *lock_device;
} psm_station_t;

typedef struct _psm_sys
{
    void                *wf;
    void                *rt;
    void                *bd;
    void                *yk;
    void                *ticket_service;
    int                 station_count;
    TRTDBSvcCfgData     rt_cfg;
    TGCDataAreaID       *area;
    psm_station_t       *station;
    psm_config_t        config;
    psm_exec_list_t     exec_list;

    CRITICAL_SECTION guard_critical;    //临界区
} psm_sys_t;


static int inited = NUMBER_FALSE;
static psm_sys_t self;

psm_sys_t *psm_sys()
{
    if(!inited)
    {
        memset(&self, 0, sizeof(psm_sys_t));
        MUTEX_INIT(&self.guard_critical);
        inited = NUMBER_TRUE;
    }
    return &self;
}

void lock()
{
    MUTEX_LOCK(&psm_sys()->guard_critical);
}

void unlock()
{
    MUTEX_UNLOCK(&psm_sys()->guard_critical);
}

psm_station_t *find_station(int station_no)
{
    psm_station_t *station = NULL;

    lock();

    int station_count = psm_sys()->station_count;
    for(int i=0;i<station_count;i++)
    {
        psm_station_t *s = (psm_station_t *)(psm_sys()->station+i);
        if(station_no==s->station_no)
        {
            station = s;
            break;
        }
    }

    unlock();

    return station;
}

device_t *find_device_by_id(int station_no, int id)
{
    device_t *result = NULL;

    psm_station_t *station = find_station(station_no);
    if(NULL!=station)
    {
        device_t *n = (device_t *)(station->device);
        while(NULL!=n)
        {
            if(id==n->inner_id)
            {
                result = n;
                break;
            }

            n = (device_t *)n->next;
        }
    }

    return result;
}

/*!
 * \brief psm_device_jk_state       将集控状态转换成本地状态或将本地状态转换成集控状态
 * \param state                     当前状态
 * \param reverse                   是否反转状态
 * \param has_3_state               是否为三态
 * \return                          详见jk_device_t
 */
int psm_device_jk_state(int state, int reverse, int has_3_state)
{
    int result = state;
    //状态取反
    if(reverse)
    {
        if(has_3_state)
        {
            //三态,如小车
            switch (state) {
            case 0:
                result = 2;
                break;
            case 1:
                result = 1;
                break;
            case 2:
                result = 0;
                break;
            default:
                break;
            }

        }
        else
        {
            //二态,如网门
            switch (state) {
            case 0:
                result = 1;
                break;
            case 1:
                result = 0;
                break;
            default:
                break;
            }
        }
    }
    return result;
}

void psm_append_device_extend(device_t *device)
{
    if(NULL==device)
    {
        return;
    }
    if(NULL!=device->extend)
    {
        return;
    }
    psm_device_extend_t *ext = (psm_device_extend_t *)malloc(sizeof(psm_device_extend_t));
    if(NULL==ext)
    {
        return;
    }
    memset(ext, 0, sizeof(psm_device_extend_t));
    device->extend = ext;
}

int rt_callback(TGCDataAreaID * pAreaID,TGCContainer * pContainer,DWORD32 valType,DWORD32 index,TGCRltValue * pVal,void * pContext,DWORD32 breakFaceNo)
{
    if(NULL==pAreaID) return 0;
    if(NULL==pContainer) return 0;
    if(NULL==pVal) return 0;
    if(1!=valType) return 0;  //只处理遥信值
    if(breakFaceNo!=0) return 0;
    //index值定义：0唯一操作权，1状态
    int wf_id = pAreaID->NO;
    int id = pContainer->innerID;
    int type = index;
    int value = pVal->var.var32Low;

    device_t *d = find_device_by_id(wf_id, id);
    if(NULL!=d)
    {
        switch (type) {
        case 1:
            if(value!=d->value)
            {
                psm_append_device_extend(d);
                psm_device_extend_t *ext = (psm_device_extend_t *)d->extend;
                if(NULL==ext)
                {
                    return 0;
                }
                //不在遥信发表中
                if(!ext->yx.send.enable)
                {
                    return 0;
                }

                d->value = value;
                int real_state = psm_device_jk_state(value, ext->yx.send.reverse, ext->yx.send.has_3_state);
                if(NULL!=psm_intf_instance()->device_state_change)
                {
                    int len = strlen(d->name);
                    psm_intf_instance()->device_state_change(wf_id, len, d->name, real_state);
                }
            }
            break;
        case 0:
            if(value!=d->lock_value)
            {
                d->lock_value = value;
                if(NULL!=psm_intf_instance()->device_lock_state_change)
                {
                    int len = strlen(d->name);
                    psm_intf_instance()->device_lock_state_change(wf_id, len, d->name, d->lock_value);
                }
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

int wf_connect_callback(DWORD32 state)
{
    const char *connected = (state==0)?"disconnected":"connected";
    printf("wf state:%s\n", connected);
    return 0;
}

int bd_connect_callback(DWORD32 state)
{
    const char *connected = state==0?"disconnected":"connected";
    printf("bd state:%s\n", connected);
    return 0;
}

int psm_init()
{
    /* 1.GCInitEnvContext       初始化PSM
     */
    {
        int ret = GCInitEnvContext();
        Sleep(200);
        print_function(GCInitEnvContext, ret);
        return (0<=ret);
    }
}

void psm_uninit_s_bd()
{
    if(NULL==psm_sys()->bd)
    {
        return;
    }

    void *bd = psm_sys()->bd;

    psm_sys()->bd = NULL;
    GCBDShutdownSvc(bd);
    GCBDUninstantiateClientSvc(bd);
}

void psm_uninit_s_yk()
{
    if(NULL==psm_sys()->yk)
    {
        return;
    }
    void *yk = psm_sys()->yk;
    psm_sys()->yk = NULL;

    GCRTDBYKShutdownSvc(yk);
    GCRTDBYKUninstantiateClientSvc(yk);
}

void psm_uninit_s_rt()
{
    if(NULL==psm_sys()->rt)
    {
        return;
    }
    void *rt = psm_sys()->rt;
    psm_sys()->rt = NULL;
    GCShutdownSvc(rt);
    GCUninstantiateSvc(rt);
}

void psm_uninit_s_wf()
{
    if(NULL==psm_sys()->wf)
    {
        return;
    }
    void *wf = psm_sys()->wf;
    psm_sys()->wf = NULL;
    GCWFShutdownSvc(wf);					//关闭防务总服务
    GCWFUninstantiateClientSvc(wf);         //撤销防务总服务
}

void psm_uninit()
{
    psm_uninit_s_yk();
    psm_uninit_s_bd();
    psm_uninit_s_rt();
    psm_uninit_s_wf();
    {
        int ret = GCUninitEnvContext();
        print_function(GCUninitEnvContext, ret);
    }
}

int psm_init_s_wf(char *wf_ip, int wf_port)
{
    /* 2.GCWFInitClientSvc      初始化五防服务
     */
    {
        char ip[16];
        sprintf(ip, wf_ip);
        WORD remote = wf_port;
        void *wf = GCWFInitClientSvc(0, ip, remote);  //localID参数在GCWFInitClientSvc函数在未使用，此处使用0
        print_function(GCWFInitClientSvc, (int)wf);
        if(NULL==wf)
        {
            return NUMBER_FALSE;
        }
        psm_sys()->wf = wf;

        int ret = GCWFRegisterCallbackConnectState(wf_connect_callback, wf);
        print_function(GCWFRegisterCallbackConnectState, ret);
        if(ret<0)
        {
            return NUMBER_FALSE;
        }
    }

    /* 3.GCWFStartupClientSvc   启动五防服务
     */
    {
        void *wf = psm_sys()->wf;
        int ret = GCWFStartupClientSvc(wf);
        Sleep(200);
        print_function(GCWFStartupClientSvc, ret);
        if(ret<0)
        {
            return NUMBER_FALSE;
        }
    }
    /* 3.1.站点数据
     */
    {
        DWORD32 count = 0;
        {
            void *wf = psm_sys()->wf;
            int ret = GCWFGetStationCount(wf, count);
            print_function(GCWFGetStationCount, ret);
            printf("wf station count:%d\n", count);
            if(0<count)
            {
                TStationIDItem *station = (TStationIDItem *)malloc(count*sizeof(TStationIDItem));
                if(NULL!=station)
                {
                    memset(station, 0, count*sizeof(TStationIDItem));

                    DWORD32 get_count = 0;
                    void *wf = psm_sys()->wf;
                    int ret = GCWFGetStationIDList(wf, station, count, get_count);
                    print_function(GCWFGetStationIDList, ret);
                    printf("wf get station count:%d\n", get_count);
                    if(0<=ret&&get_count)
                    {
                        TGCDataAreaID *area = (TGCDataAreaID *)malloc(get_count*sizeof(TGCDataAreaID));

                        if(NULL!=area)
                        {
                            memset(area, 0, sizeof(get_count*sizeof(TGCDataAreaID)));
                            for(int i=0;i<get_count;i++)
                            {
                                TStationIDItem *item = (TStationIDItem *)(station+i);
                                TGCDataAreaID *a = (TGCDataAreaID *)(area+i);
                                a->NO = item->stationNo;
                                a->type = item->stationType;
                                printf("station %d:id=%d, type=%d\n", i+1, item->stationNo, item->stationType);
                            }
                            psm_sys()->station_count = get_count;
                            psm_sys()->area = area;
                        }
                    }
                    else
                    {
                        //nothing to do
                    }//ret

                    free(station);
                }
            }//0<count
        }//count
    }

    return NUMBER_TRUE;
}

int psm_init_s_rt(int rt_id)
{
    TRTDBSvcCfgData rt_cfg;
    /* 4.GCWFGetRTDBSvcCfg      获取实时库配置
     */
    {
        void *wf = psm_sys()->wf;
        int ret = GCWFGetRTDBSvcCfg(wf, &rt_cfg);
        print_function(GCWFGetRTDBSvcCfg, ret);
        if(0>ret)
        {
            return NUMBER_FALSE;
        }
    }
    /* 4.1.GCInstantiateSvc     初始化实时库
     */
    {
        DWORD32 id = rt_id;
        void *rt = GCInstantiateSvc(id, rt_cfg.remote_ip, rt_cfg.remote_port, rt_cfg.remote_pubport);
        Sleep(200);
        print_function(GCInstantiateSvc, (int)rt);
        if(NULL==rt)
        {
            return NUMBER_FALSE;
        }
        psm_sys()->rt = rt;
        TRTDBSvcCfgData *t = &psm_sys()->rt_cfg;
        memset(t, 0, sizeof(TRTDBSvcCfgData));
        memcpy(&t, &rt_cfg, sizeof(TRTDBSvcCfgData));
    }
    // 4.2.GCStartupSvc         启动实时库
    {
        void *rt = psm_sys()->rt;
        TGCDataAreaID station_out[1];
        memset(station_out, 0, sizeof(TGCDataAreaID));
        DWORD32 out_val = sizeof(station_out);
        DWORD32 size = psm_sys()->station_count;
        //权限 PRIVILEGE_USER_DATA_EX / PRIVILEGE_RECV_MSG_EX / PRIVILEGE_NODATA_UPDATEVAL_EX
        DWORD32 privilege = PRIVILEGE_USER_DATA_EX | PRIVILEGE_RECV_MSG_EX | PRIVILEGE_NODATA_UPDATEVAL_EX;
        int ret = GCStartupSvc(rt, psm_sys()->area, size, station_out, out_val, privilege, rt_callback, psm_sys(), NULL);
        print_function(GCStartupSvc, ret);
        if(0>ret)
        {
            return NUMBER_FALSE;
        }
    }

    return NUMBER_TRUE;
}

int psm_get_device_value(int station_no, int id, int type, int &value)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }
    TGCDataAreaID *area = station->area;
    DWORD32 device_count = station->device_count;
    if(1>device_count)
    {
        return NUMBER_FALSE;
    }
    DWORD32 get_count = 0;
    TGCContainer *devices = (TGCContainer *)malloc(device_count*sizeof(TGCContainer));
    if(NULL==devices)
    {
        return NUMBER_FALSE;
    }
    memset(devices, 0, device_count*sizeof(TGCContainer));

    void *rt = psm_sys()->rt;
    int ret = GCGetALLContainer(rt, area, devices, device_count, &get_count);
    print_function(GCGetALLContainer, ret);
    printf("device count:%d, get count:%d\n", device_count, get_count);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    TGCContainer *index = NULL;
    for(int i=0; i<get_count; i++)
    {
        if(devices[i].innerID==id)
        {
            index = &devices[i];
            break;
        }
    }
    if(NULL==index)
    {
        return NUMBER_FALSE;
    }
    TGCRltValue v;
    ret = GCGetRltValue(rt, area, index, 1, type, &v);
    print_function(GCGetRltValue, ret);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    value = v.var.var32Low;
    return NUMBER_TRUE;
}

int psm_set_device_value(int station_no, int id, int type, int value)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }
    TGCDataAreaID *area = station->area;
    TGCValue v;
    v.var32Low = value;
    void *rt = psm_sys()->rt;
    int ret = GCSetRltValueByInnerID(rt, area, id, 1, type, &v);
    print_function(GCSetRltValueByInnerID, ret);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    return NUMBER_TRUE;
}


void psm_init_station(psm_station_t *station)
{
    DWORD32 wf_id = station->station_no;
    char *file_path;
    //FIXME:目录不存在则需要先创建
    char dir[] = "data";
    char file_name[256] = {0};
    sprintf(file_name, "record_%d.dat", wf_id);

    TFileDetails fd;
    //取站点点表文件路径
    {
        void *wf = psm_sys()->wf;
        int ret = GCWFGetPointTablePath(wf, wf_id, &fd);
        print_function(GCWFGetPointTablePath, ret);
        if(0>ret)
        {
            return;
        }
        file_path = fd.filePath;
        printf("wf_id=%d,path=%s,type=%d\n", wf_id, fd.filePath, fd.type);
    }
    //下载站点点表
    {
        DWORD64 size = 0;
        void *bd = psm_sys()->bd;
        int ret = GCBDDownLoadFile(bd, file_path, size, dir, file_name);
        print_function(GCBDDownLoadFile, ret);
        if(0>ret)
        {
            return;
        }
    }
    /*
     * 点表文件解析
     */
    {
        device_t *list = NULL;

        {
            memset(file_name, 0, sizeof(file_name));
            sprintf(file_name, "%s/record_%d.dat", dir, wf_id);
            CDSRecord record;
            int ret = record.load(file_name);
            printf("load %s (ret=%d)\n", file_name, ret);
            if(0<=ret)
            {
                int record_count = record.count();
                printf("record count:%d\n", record_count);
                for(int i=0; i<record_count; i++)
                {
                    TRecord *r = (TRecord *)record.findByIndex(i);
                                        printf("record:station_id=%d,inner_id=%d,mac_id=%d,name=%s,point_id=%d,type=%d,vtype=%d,vindex=%d,value=%d\n",
                                               r->stationID,
                                               r->innerID,
                                               r->macID,
                                               (char *)r->macNum,
                                               r->pointID,
                                               r->type,
                                               r->vtype,
                                               r->vindex,
                                               r->value);
                    device_insert(&list, (char *)r->macNum, sizeof(r->macNum));
                    device_t *d = device_find(list, (char *)r->macNum, sizeof(r->macNum));
                    if(NULL!=d)
                    {
                        d->inner_id = r->innerID;
                    }
                }
            }
            if(NULL==list)
            {
                return;
            }
            int count = device_count(list);
            station->device = list;
            station->device_count = count;

            /*更新设备状态*/
            {
                device_t *next = list;
                while(next != NULL)
                {
                    int value;
                    if(psm_get_device_value(wf_id, next->inner_id, 1, value))
                    {
                        next->value = value;
                    }
                    if(psm_get_device_value(wf_id, next->inner_id, 0, value))
                    {
                        next->lock_value = value;
                    }

                    printf("name=%s,id=%d, value=%d, lock_value=%d\n",
                           next->name, next->inner_id, next->value, next->lock_value);
                    //                    if(0==(count%5))
                    //                    {
                    //                        printf("\n");
                    //                    }
                    next = (device_t *)next->next;
                }
            }
        }
    }

}

void psm_init_s_bd()
{
    if(NULL!=psm_sys()->bd)
    {
        return;
    }
    TBigDataCfgData bd_cfg;
    void *bd;
    /*
     * 从大数据服务取站点表文件，输出设备列表
     * FIXME:站点表文件更新(怎么知道?)，重新获得设备列表
     */
    /* 5.GCWFGetBigDataSvcCfg   从五防服务取大数据服务配置信息
     */
    {
        void *wf = psm_sys()->wf;
        int ret = GCWFGetBigDataSvcCfg(wf, &bd_cfg);
        print_function(GCWFGetBigDataSvcCfg, ret);
        if(0>ret)
        {
            return;
        }
    }
    /* 5.1.GCBDInitClientSvc
     * 初始化大数据服务
     */
    {
        int bd_id = 1;
        bd = GCBDInitClientSvc(bd_id, bd_cfg.remote_ip, bd_cfg.remote_port);
        Sleep(200);
        print_function(GCBDInitClientSvc, (int)bd);
        if(NULL==bd)
        {
            return;
        }

//        int ret = GCBDRegisterCallbackConnectState(bd_connect_callback, bd);
//        print_function_int(GCBDRegisterCallbackConnectState, ret);
    }
    /* 5.2.GCBDStartupClientSvc
     * 启动大数据服务，与初始化是一对
     */
    {
        int ret = GCBDStartupClientSvc(bd);
        print_function(GCBDStartupClientSvc, ret);
        if(0>ret)
        {
            GCBDUninstantiateClientSvc(bd);
            return;
        }
    }
    psm_sys()->bd = bd;
}



void psm_init_device()
{
    /* 5.3.
     * 初始化站数据，目前只取设备主编号
     */
    {
        int station_count = psm_sys()->station_count;
        if(0<station_count)
        {
            int size = station_count*sizeof(psm_station_t);
            psm_station_t *station = (psm_station_t *)malloc(size);
            if(NULL!=station)
            {
                memset(station, 0, size);
                psm_sys()->station = station;

                TGCDataAreaID *area = psm_sys()->area;
                for(int i=0; i<station_count; i++)
                {
                    TGCDataAreaID *a = (TGCDataAreaID *)(area+i);
                    psm_station_t *s = (psm_station_t *)(station+i);

                    int station_no = a->NO;
                    s->station_no = station_no;
                    s->area = a;

                    psm_init_station(s);
                }


            }
            else
            {
                psm_sys()->station_count = 0;
                free(psm_sys()->area);
                psm_sys()->area = NULL;
            }
        }
    }
}

/*!
 * \brief psm_handle_yx     处理站点遥信数据
 * \param station           站点
 * \param record            遥信数据
 * \param mode              数据表类型:0-接收,1-发送
 */
void psm_handle_yx(psm_station_t *station, psm_file_yx_t *record, int mode)
{
    int station_no = station->station_no;
    device_t *dev = find_device_by_id(station_no, record->record.innerID);
    psm_append_device_extend(dev);
    if(NULL==dev->extend)
    {
        return;
    }
    switch (mode) {
    case 0:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->yx.recieve.enable = NUMBER_TRUE;
        ext->yx.recieve.reverse = record->isOpposite;
        ext->yx.recieve.has_3_state = record->isThreeState;
    }
        break;
    case 1:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->yx.send.enable = NUMBER_TRUE;
        ext->yx.send.reverse = record->isOpposite;
        ext->yx.send.has_3_state = record->isThreeState;
    }
        break;
    default:
        break;
    }
}

/*!
 * \brief psm_load_yx       加载站点遥信点表
 * \param station           站点
 */
void psm_load_yx(psm_station_t *station)
{
    static int file_name_len = 255;
    char file_name[file_name_len];
    int station_no = station->station_no;
    //处理遥信接收表
    {
        memset(file_name, 0, file_name_len);
        sprintf(file_name, "data/%d_yxRecvTable.dat", station_no);

        psm_file_yx_t *r = NULL;
        int count = 0;
        int ret = psm_file_load_yx(file_name, &r, count);
        if(ret)
        {
            for(int i=0;i<count;i++)
            {
                psm_file_yx_t *yx = (psm_file_yx_t *)(r+i);
                psm_handle_yx(station, yx, 0);
            }
        }
    }
    //处理遥信发送表
    {
        memset(file_name, 0, file_name_len);
        sprintf(file_name, "data/%d_yxSendTable.dat", station_no);

        psm_file_yx_t *r = NULL;
        int count = 0;
        int ret = psm_file_load_yx(file_name, &r, count);
        if(ret)
        {
            for(int i=0;i<count;i++)
            {
                psm_file_yx_t *yx = (psm_file_yx_t *)(r+i);
                psm_handle_yx(station, yx, 1);
            }
        }
    }
}

void psm_init_yx()
{
    int station_count = psm_sys()->station_count;
    for(int i=0;i<station_count;i++)
    {
        psm_station_t *s = (psm_station_t *)(psm_sys()->station+i);
        if(NULL==s)
        {
            continue;
        }
        psm_load_yx(s);

    }

}

/*!
 * \brief psm_handle_yk     处理站点遥控数据
 * \param station           站点
 * \param record            遥控数据
 * \param mode              数据表类型:0-接收,1-发送
 */
void psm_handle_yk(psm_station_t *station, TYKRecord *record, int mode)
{
    int station_no = station->station_no;
    device_t *dev = find_device_by_id(station_no, record->record.innerID);
    psm_append_device_extend(dev);
    if(NULL==dev->extend)
    {
        return;
    }
    switch (mode) {
    case 0:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->yk.recieve.enable = NUMBER_TRUE;
    }
        break;
    case 1:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->yk.send.enable = NUMBER_TRUE;
    }
        break;
    default:
        break;
    }
}

/*!
 * \brief psm_load_yk       加载站点遥控点表
 * \param station           站点
 */
void psm_load_yk(psm_station_t *station)
{
    static int file_name_len = 255;
    char file_name[file_name_len];
    int station_no = station->station_no;
    //处理遥控接收表
    {
        memset(file_name, 0, file_name_len);
        sprintf(file_name, "data/%d_ykRecvTable.dat", station_no);

        TYKRecord *r = NULL;
        int count = 0;
        int ret = psm_file_load_yk(file_name, &r, count);
        if(ret)
        {
            for(int i=0;i<count;i++)
            {
                TYKRecord *yk = (TYKRecord *)(r+i);
                psm_handle_yk(station, yk, 0);
            }
        }
    }
    //处理遥控发送表
    {
        memset(file_name, 0, file_name_len);
        sprintf(file_name, "data/%d_ykSendTable.dat", station_no);

        TYKRecord *r = NULL;
        int count = 0;
        int ret = psm_file_load_yk(file_name, &r, count);
        if(ret)
        {
            for(int i=0;i<count;i++)
            {
                TYKRecord *yk = (TYKRecord *)(r+i);
                psm_handle_yk(station, yk, 1);
            }
        }
    }
}

void psm_init_yk()
{
    int station_count = psm_sys()->station_count;
    for(int i=0;i<station_count;i++)
    {
        psm_station_t *s = (psm_station_t *)(psm_sys()->station+i);
        if(NULL==s)
        {
            continue;
        }
        psm_load_yk(s);
    }

}

void psm_handle_kbq(psm_station_t *station, TKBQRecord *record)
{
    int station_no = station->station_no;
    device_t *dev = find_device_by_id(station_no, record->devRecord.innerID);
    device_t *kbq = find_device_by_id(station_no, record->kbqRecord.innerID);
    psm_append_device_extend(dev);
    if(NULL==dev->extend)
    {
        return;
    }
    int type = record->devAfterState;
//    if((1==record->devBeforeState)&&(0==record->devAfterState))
//    {
//        type = 0;
//    }
//    if((0==record->devBeforeState)&&(1==record->devAfterState))
//    {
//        type = 1;
//    }
    switch (type) {
    case 0:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->kbq.open.enable = NUMBER_TRUE;
        ext->kbq.open.kbq    = kbq;
    }
        break;
    case 1:
    {
        psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
        ext->kbq.close.enable = NUMBER_TRUE;
        ext->kbq.close.kbq    = kbq;
    }
        break;
    default:
        break;
    }
}

/*!
 * \brief psm_load_kbq      加载KBQ点表
 * \param station           站点
 */
void psm_load_kbq(psm_station_t *station)
{
    int station_no = station->station_no;

    static int file_name_len = 255;
    char file_name[file_name_len];
    memset(file_name, 0, file_name_len);
    sprintf(file_name, "data/kbq_%d.dat", station_no);
    //下载KBQ点表
    char *file_path=NULL;
    TFileDetails fd;
    DWORD32 wf_id = station->station_no;
    //获得服务器端KBQ点表文件路径
    {
        void *wf = psm_sys()->wf;
        int ret = GCWFGetKbqTablePath(wf, wf_id, &fd);
        print_function(GCWFGetKbqTablePath, ret);
        if(0>ret)
        {
            return;
        }
        file_path = fd.filePath;
        printf("wf_id=%d,path=%s,type=%d\n", wf_id, fd.filePath, fd.type);
    }
    //下载站点点表
    {
        DWORD64 size = 0;
        void *bd = psm_sys()->bd;
        int ret = GCBDDownLoadFile(bd, file_path, size, "./", file_name);
        print_function(GCBDDownLoadFile, ret);
        if(0>ret)
        {
            return;
        }
    }
    TKBQRecord *r = NULL;
    int count = 0;
    int ret = psm_file_load_kbq(file_name, &r, count);
    if(!ret)
    {
        return;
    }
    for(int i=0;i<count;i++)
    {
        TKBQRecord *kbq = (TKBQRecord *)(r+i);
        psm_handle_kbq(station, kbq);
    }
}

psm_exec_t *psm_find_exec(int station_no, DWORD32 device_id)
{
    psm_exec_t *found_item = NULL;
    lock();
    int count = psm_sys()->exec_list.count;
    psm_exec_t *item = psm_sys()->exec_list.item;
    for(int i=0;i<count;i++)
    {
        psm_exec_t *n = (psm_exec_t *)(item+i);
        if(!n->enable)
        {
            continue;
        }
        if(station_no==n->station_id&&device_id==n->device_id)
        {
            found_item = n;
            break;
        }
    }
    unlock();

    return found_item;
}

void psm_save_exec()
{
    lock();
    int count = psm_sys()->exec_list.count;
    psm_exec_t *item = psm_sys()->exec_list.item;

    FILE *fp;
    char file[] = "./data/kbq_exec.ini";
    fp = fopen(file, "w");
    if(NULL==fp)
    {
        unlock();
        return;
    }
    for(int i=0;i<count;i++)
    {
        psm_exec_t *n = (psm_exec_t *)(item+i);
        if(!n->enable)
        {
            continue;
        }
        fprintf(fp, "%d,%d,%d,%lf\n", n->station_id, n->device_id, n->exec_id, n->exec_time);
    }
    fclose(fp);

    unlock();
}

int kbq_callback(TTREXKBQOptResult * pKBQOptResult, void * pContext)
{
    UN_USE(pContext);

    int station_no = pKBQOptResult->stationNO;
    DWORD32 device_id = pKBQOptResult->devPointID;
    switch (pKBQOptResult->KBQDstState) {
    case 0:
    {
        //分
        psm_exec_t *n = psm_find_exec(station_no, device_id);
        if(NULL==n)
        {
            return NUMBER_FALSE;
        }
        device_t *dev = find_device_by_id(station_no, n->device_id);
        if(NULL==dev)
        {
            return NUMBER_FALSE;
        }
        n->enable = NUMBER_FALSE;
        psm_save_exec();

        jk_device_list_t list;
        jk_device_t device;
        memset(&list, 0, sizeof(jk_device_list_t));
        list.count = 1;
        list.device = &device;

        memset(&device, 0, sizeof(jk_device_t));
        memcpy(&device.device, dev->name, strlen(dev->name));
        device.state_1 = 1;  //合
        device.state_2 = 2;  //失败
        if(1==pKBQOptResult->decSrcState&&0==pKBQOptResult->devDstState)
        {
            device.state_1 = 0;
        }
        if(pKBQOptResult->KBQOptResult)
        {
            device.state_2 = 1;   //成功
        }

        psm_intf_instance()->client_send_device_unlock_result(station_no, &list);
    }
        break;
    case 1:
    {
        //合
        device_t *dev = find_device_by_id(station_no, device_id);
        if(NULL==dev)
        {
            return NUMBER_FALSE;
        }

        jk_device_list_t list;
        jk_device_t device;
        memset(&list, 0, sizeof(jk_device_list_t));
        list.count = 1;
        list.device = &device;

        memset(&device, 0, sizeof(jk_device_t));
        memcpy(&device.device, dev->name, strlen(dev->name));
        device.state_1 = 1;  //合
        device.state_2 = 2;  //失败
        if(1==pKBQOptResult->decSrcState&&0==pKBQOptResult->devDstState)
        {
            device.state_1 = 0;
        }
        if(pKBQOptResult->KBQOptResult)
        {
            device.state_2 = 1;   //成功
        }

        psm_intf_instance()->client_send_device_lock_result(station_no, &list);
    }
        break;
    default:
        break;
    }

    return NUMBER_TRUE;
}

void psm_init_kbq()
{
    psm_config_t *config = &psm_sys()->config;

    self.ticket_service = NULL;

    int rt_ticket = config_instance()->rt_ticket;
    int ret = 0;
    ret = GCTREXInitEnvContext();
    print_function(GCTREXInitEnvContext, ret);
    if(0>ret)
    {
        return;
    }
    void *s = GCTREXInstantiateSvc();
    if(NULL==s)
    {
        return;
    }
    ret = GCTREXStartupSvc(s, rt_ticket, config->wf_ip, config->wf_port);
    print_function(GCTREXStartupSvc, ret);
    if(0>ret)
    {
        return;
    }
    self.ticket_service = s;
    ret = GCTREXRegisterKBQOptResult(s, kbq_callback, NULL);
    print_function(GCTREXRegisterKBQOptResult, ret);
    if(0>ret)
    {
        GCTREXUninstantiateSvc(s);
        GCTREXUninitEnvContext();
        self.ticket_service = NULL;
        return;
    }

    return;

    int station_count = psm_sys()->station_count;
    for(int i=0;i<station_count;i++)
    {
        psm_station_t *s = (psm_station_t *)(psm_sys()->station+i);
        if(NULL==s)
        {
            continue;
        }
        psm_load_kbq(s);
    }
}

void psm_load_exec()
{
    int count = 0;
    psm_exec_t *item = NULL;

    FILE *fp;
    char file[] = "./data/kbq_exec.ini";
    char s[1024];
    fp = fopen(file, "r");
    if(NULL==fp)
    {
        return;
    }
    while(!feof(fp))
    {
        *s = 0;

        fscanf( fp, "%100[^\n#]s", s );
        fscanf( fp, "%*[^\n]s" );
        fgetc( fp );			// Skip newline

        int station_no = 0;
        DWORD32 device_id = 0;
        DWORD32 exec_id = 0;
        double exec_time = 0.0;
        if(-1==sscanf(s, "%d,%d,%d,%lf", &station_no, &device_id, &exec_id, &exec_time))
        {
            continue;
        }
        psm_exec_t *n = NULL;
        if(NULL==item)
        {
            item = (psm_exec_t *)malloc(sizeof(psm_exec_t));
            if(NULL==item)
            {
                count = 0;
                break;
            }
            n = (psm_exec_t *)(item+count);
        }
        else
        {
            int size = sizeof(psm_exec_t)*(count+1);
            item = (psm_exec_t *)realloc(item, size);
            if(NULL==item)
            {
                count = 0;
                break;
            }
            n = (psm_exec_t *)(item+count);
        }
        memset(n, 0, sizeof(psm_exec_t));
        n->enable = NUMBER_TRUE;
        n->station_id = station_no;
        n->device_id = device_id;
        n->exec_id = exec_id;
        n->exec_time = exec_time;
        count++;
    }

    fclose(fp);

    if(NULL!=item)
    {
        psm_exec_list_t *list = &psm_sys()->exec_list;
        list->count =  count;
        list->item = item;
    }
}
int yk_allow(TYKMsgData msgData,void *pContext)
{
    UN_USE(pContext);
    int station_no = msgData.stationID;
    int device_id = msgData.pointID;
    device_t *dev = find_device_by_id(station_no, device_id);
    if(NULL==dev)
    {
        return NUMBER_FALSE;
    }
    //不在遥控发表中不处理
    if(!psm_device_in_yk_table(station_no, dev->name, strlen(dev->name), 1))
    {
        return NUMBER_FALSE;
    }
    jk_device_t n;
    jk_device_list_t list;
    memset(&n, 0, sizeof(jk_device_t));
    list.count = 1;
    list.device = &n;
    n.device_len = strlen(dev->name);
    memcpy(&n.device, dev->name, n.device_len);
    n.state_1 = msgData.state;
    int mode = msgData.msgType;
    switch (mode) {
    case 3:
    {
        //遥控执行发设备解锁

        if(NULL!=psm_intf_instance()->server_send_unlock_device)
        {
            psm_intf_instance()->server_send_unlock_device(station_no, &list);
        }
    }
        break;
    case 4:
    {
        //遥控撤销发设备闭锁

        if(NULL!=psm_intf_instance()->server_send_lock_device)
        {
            psm_intf_instance()->server_send_lock_device(station_no, &list);
        }
    }
    default:
        break;
    }
    return NUMBER_TRUE;
}

int yk_forbid(TForbidAllForStationRqt msgData,void *pContext)
{
    UN_USE(pContext);
    int station_no = msgData.staionNo;
    //全站禁止闭锁所有设备

    if(NULL!=psm_intf_instance()->client_lock_all_device)
    {
        psm_intf_instance()->client_lock_all_device(station_no);
    }
    return NUMBER_TRUE;
}

void psm_init_s_yk()
{
    psm_sys()->yk = NULL;
    if(NULL==psm_sys()->rt)
    {
        return;
    }
    void *yk = GCRTDBYKInitClientSvc();
    print_function(GCRTDBYKInitClientSvc, (int)yk);
    if(NULL==yk)
    {
        return;
    }
    int ret = 0;
    ret = GCRTDBYKRegisterCallbackReceiveYK(yk, yk_allow, yk);
    print_function(GCRTDBYKRegisterCallbackReceiveYK, ret);
    if(0>ret)
    {
        GCRTDBYKUninstantiateClientSvc(yk);
        return;
    }
    ret = GCRTDBYKRegisterCallbackRecvForbidYK(yk, yk_forbid, yk);
    print_function(GCRTDBYKRegisterCallbackRecvForbidYK, ret);
    if(0>ret)
    {
        GCRTDBYKUninstantiateClientSvc(yk);
        return;
    }
    DWORD32 local_id = config_instance()->rt_yk;
    TRTDBSvcCfgData *cfg = &psm_sys()->rt_cfg;
    ret = GCRTDBYKStartupClientSvc(yk, local_id, cfg->remote_ip, cfg->remote_port, cfg->remote_pubport);
    print_function(GCRTDBYKStartupClientSvc, ret);
    if(0>ret)
    {
        GCRTDBYKUninstantiateClientSvc(yk);
        return;
    }
    psm_sys()->yk = yk;
}

int psm_start(psm_config_t *config)
{
    inited = NUMBER_FALSE;
    
    psm_sys();

    int len = strlen(config->wf_ip);
    char *ip = (char *)malloc(len);
    if(NULL==ip)
    {
        return NUMBER_FALSE;
    }
    memcpy(ip, config->wf_ip, len);
    memcpy(&self.config, config, sizeof(psm_config_t));
    self.config.wf_ip = ip;

    if(!psm_init())
    {
        return NUMBER_FALSE;
    }

    if(!psm_init_s_wf(config->wf_ip, config->wf_port))
    {
        psm_uninit();
        return NUMBER_FALSE;
    }
    if(!psm_init_s_rt(config->rt_id))
    {
        psm_uninit();
        return NUMBER_FALSE;
    }
    psm_init_s_bd();
    if(NULL==psm_sys()->bd)
    {
        psm_uninit();
        return NUMBER_FALSE;
    }
    psm_init_s_yk();
    if(NULL==psm_sys()->yk)
    {
        psm_uninit();
        return NUMBER_FALSE;
    }
    psm_init_device();
    psm_init_yx();
    psm_init_yk();
    psm_init_kbq();

    psm_uninit_s_bd();

    psm_load_exec();


    return NUMBER_TRUE;
}

jk_device_list_t *psm_get_device(int station_no)
{
    jk_device_list_t *result = (jk_device_list_t *)malloc(sizeof(jk_device_list_t));
    if(NULL!=result)
    {
        memset(result, 0, sizeof(jk_device_list_t));
        psm_station_t *station = find_station(station_no);
        if(NULL!=station)
        {
            int count = 0;
            jk_device_t *device = NULL;
            lock();
            device_t *n = station->device;
            while(NULL!=n)
            {
                psm_append_device_extend(n);
                psm_device_extend_t *ext = (psm_device_extend_t *)n->extend;
                //设备必须在遥信发表中
                if(NULL!=ext&&ext->yx.send.enable)
                {
                    if(NULL==device)
                    {
                        device = (jk_device_t *)malloc(sizeof(jk_device_t));
                    }
                    else
                    {
                        int size = sizeof(jk_device_t)*(count+1);
                        device = (jk_device_t *)realloc(device, size);
                    }
                    if(NULL==device)
                    {
                        count = 0;
                        break;
                    }
                    jk_device_t *d = (jk_device_t *)(device+count);
                    int len = strlen(n->name);
                    memcpy(d->device, n->name, len);
                    d->device_len = len;
                    d->state_1 = psm_device_jk_state(n->value, ext->yx.send.reverse, ext->yx.send.has_3_state);
                    count++;
                }

                n = (device_t *)n->next;
            }
            unlock();

            result->count = count;
            result->device = device;
        }
    }
    return result;
}


device_t *psm_device_get(int station_no)
{
    device_t *result = NULL;
    psm_station_t *station = find_station(station_no);
    if(NULL!=station)
    {
        lock();
        result = station->device;
        unlock();
    }
    return result;
}


int psm_device_count(int station_no)
{
    int result = 0;
    psm_station_t *station = find_station(station_no);
    if(NULL!=station)
    {
        lock();
        result = station->device_count;
        unlock();
    }
    return result;
}


int hello_psm_1()
{
    return 1;
}

device_t *find_device_by_name(int station_no, int device_len, char *device)
{
    device_t *result = NULL;
    char *str1 = (char *)malloc(device_len+1);
    if(NULL!=str1)
    {
        memset(str1, 0, device_len+1);
        memcpy(str1, device, device_len);

        psm_station_t *station = find_station(station_no);
        lock();
        if(NULL!=station)
        {
            device_t *n = station->device;
            while(NULL!=n)
            {
                if(0==stricmp(str1, n->name))  //strcasecmp只在Linux中提供，相当于windows平台的 stricmp
                {
                    result = n;
                    break;
                }

                n = (device_t *)n->next;
            }
        }
        unlock();
        free(str1);
    }
    return result;
}

void psm_device_update(int station, int device_len, char *device, int type, int state)
{
    if(!(0==type||1==type))
    {
        return;
    }
    device_t *d = find_device_by_name(station, device_len, device);
    if(NULL==d)
    {
        return;
    }
    int value = type==1?d->value:d->lock_value;
    if(value==state)
    {
        return;
    }
    switch (type) {
    case 1:
    {
        d->value = state;
    }
        break;
    case 0:
    {
        d->lock_value = state;
    }
        break;
    default:
        break;
    }
    psm_set_device_value(station, d->inner_id, type, state);
}


void psm_device_update_state(int station, int device_len, char *device, int state)
{    
    device_t *d = find_device_by_name(station, device_len, device);
    if(NULL==d)
    {
        return;
    }
    psm_append_device_extend(d);
    psm_device_extend_t *ext = (psm_device_extend_t *)d->extend;
    if(NULL==ext)
    {
        return;
    }
    if(!ext->yx.recieve.enable)
    {
        return;
    }
    int real_state = psm_device_jk_state(state, ext->yx.recieve.reverse, ext->yx.recieve.has_3_state);
    psm_device_update(station, device_len, device, 1, real_state);
}


void psm_device_update_lock_state(int station, int device_len, char *device, int state)
{
    psm_device_update(station, device_len, device, 0, state);
}

void psm_clear_lock_device(int station_no)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return;
    }
    device_t *next = station->lock_device;
    while(NULL!=next)
    {
        device_t *d = next;
        psm_device_update_lock_state(station_no, strlen(d->name), d->name, 0);
        next = (device_t *)next->next;
    }
    lock();
    device_free(&station->lock_device);
    unlock();
}


void psm_fix_lock_device(int station_no, jk_device_list_t list)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return;
    }
    for(int i=0;i<list.count;i++)
    {
        jk_device_t *device = (jk_device_t *)(list.device+i);
        if(1!=device->state_2)
        {
            device_insert(&station->lock_device, device->device, device->device_len);
        }
        else
        {
            device_remove(&station->lock_device, device->device, device->device_len);
        }
    }
}

/*!
 * \brief psm_device_kbq_type       设备KBQ类型
 * \param ext                       设备扩展信息
 * \return                          KBQ类型:0-分合独立,1-分合共用
 */
int psm_device_kbq_type(psm_device_extend_t *ext)
{
    int result = 0;

    if(NULL!=ext)
    {
        bool success = ext->kbq.close.enable&&ext->kbq.open.enable
                &&(ext->kbq.open.kbq->inner_id==ext->kbq.close.kbq->inner_id);
        if(success)
        {
            result = 1;
        }
    }
    return result;
}

int psm_add_kbq_item(jk_device_t **list, int &count, device_t *dev, int mode, int h_state, int l_state)
{
    if(NULL==dev)
    {
        return NUMBER_FALSE;
    }
    int len = strlen(dev->name);
    jk_device_t *t_list = *list;
    if(NULL==t_list)
    {
        t_list = (jk_device_t *)malloc(sizeof(jk_device_t));
        if(NULL==t_list)
        {
            count = 0;
            *list = NULL;
            return NUMBER_FALSE;
        }
    }
    else
    {
        int size = sizeof(jk_device_t)*(count+1);
        t_list = (jk_device_t *)realloc(t_list, size);
        if(NULL==t_list)
        {
            count = 0;
            *list = NULL;
            return NUMBER_FALSE;
        }
    }

    jk_device_t *one = (jk_device_t *)(t_list+count);
    memset(one, 0, sizeof(jk_device_t));

    count++;
    one->device_len = len;
    memcpy(one->device, dev->name, len);

    *list = t_list;

    if(mode)
    {
        one->state_1 = 2;
        one->state_2 = l_state;

        return NUMBER_TRUE;
    }
    else
    {
        one->state_1 = 0;
        one->state_2 = l_state;

        int size = sizeof(jk_device_t)*(count+1);
        t_list = (jk_device_t *)realloc(t_list, size);
        if(NULL==t_list)
        {
            count = 0;
            *list = NULL;
            return NUMBER_FALSE;
        }
        jk_device_t *other = (jk_device_t *)(t_list+count);
        memset(other, 0, sizeof(jk_device_t));

        count++;
        other->device_len = len;
        memcpy(other->device, dev->name, len);
        other->state_1 = 1;
        other->state_2 = h_state;

        *list = t_list;

        return NUMBER_TRUE;
    }
}

int psm_get_kbq(int station_no, jk_device_list_t *list)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }

    void *s = psm_sys()->ticket_service;
    if(NULL==s)
    {
        return NUMBER_FALSE;
    }
    DWORD32 count = 0;
    int ret = GCTREXGetDevKBQRelateCount(s, station_no, count);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }

    if(0==count)
    {
        return NUMBER_FALSE;
    }

    TTREXKBQStateReport *all = (TTREXKBQStateReport *)malloc(sizeof(TTREXKBQStateReport)*count);
    if(NULL==all)
    {
        return NUMBER_FALSE;
    }
    DWORD32 real_count = 0;
    ret = GCTREXGetDevKBQRelate(s, station_no, all, count, real_count);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    int sum = 0;
    jk_device_t *t = NULL;
    for(int i=0;i<real_count;i++)
    {
        TTREXKBQStateReport *n = (TTREXKBQStateReport *)(all+i);
        device_t *dev = find_device_by_id(station_no, n->devPointID);
        if(NULL==dev)
        {
            continue;
        }
        //不在遥控发表中不处理
        if(!psm_device_in_yk_table(station_no, dev->name, strlen(dev->name), 1))
        {
            continue;
        }
        if(!psm_add_kbq_item(&t, sum, dev, n->isKBQShare, n->HKBQState, n->LKBQState))
        {
            break;
        }
    }

    if(0==sum)
    {
        return NUMBER_FALSE;
    }

    list->count = sum;
    list->device = t;

    return NUMBER_TRUE;

//    jk_device_t *d = NULL;
//    lock();
//    device_t *n = station->device;
//    while(NULL!=n)
//    {
//        psm_device_extend_t *ext = (psm_device_extend_t *)(n->extend);
//        if(NULL!=ext)
//        {
//            int mode = psm_device_kbq_type(ext);
//            switch (mode) {
//            case 1:
//            {
//                jk_device_t t;
//                memset(&t, 0, sizeof(jk_device_t));
//                int len = strlen(n->name);
//                t.device_len = len;
//                memcpy(t.device, n->name, len);
//                t.state_1 = 2;
//                t.state_2 = ext->kbq.open.kbq->value;
//            }
//                break;
//            default:
//                break;
//            }//switch
//        }//if

//        n = (device_t *)n->next;
//    }//while
//    unlock();

}

/*!
 * \brief psm_device_in_yk_table        设备是否在遥控点表中
 * \param station_no                    站号
 * \param device                        设备主编码
 * \param device_len                    设备主编码长度
 * \param mode                          遥控点表类型:0-遥控收表,1-遥控发表
 * \return
 */
int psm_device_in_yk_table(int station_no, char *device, int device_len, int mode)
{
    device_t *d = find_device_by_name(station_no, device_len, device);
    if(NULL==d)
    {
        return NUMBER_FALSE;
    }
    if(NULL==d->extend)
    {
        return NUMBER_FALSE;
    }
    psm_device_extend_t *ext = (psm_device_extend_t *)(d->extend);
    switch (mode) {
    case 0:
        return ext->yk.recieve.enable;
        break;
    case 1:
        return ext->yk.send.enable;
        break;
    default:
        break;
    }

    return NUMBER_FALSE;
}

/*!
 * \brief psm_device_can_operate        设备是否允许操作
 * \param station_no                    站号
 * \param device                        设备主编码
 * \param device_len                    设备主编码长度
 * \param operate                       操作:0-分,1-合
 * \return
 */
int psm_device_can_operate(int station_no, char *device, int device_len, int operate)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }

    void *s = psm_sys()->ticket_service;
    if(NULL==s)
    {
        return NUMBER_FALSE;
    }
    device_t *dev = find_device_by_name(station_no, device_len, device);
    if(NULL==dev)
    {
        return NUMBER_FALSE;
    }
    TTREXDeviceOptAskRqt ask;
    TTREXDeviceOptAskRsp result;
    memset(&ask, 0, sizeof(TTREXDeviceOptAskRqt));
    memset(&result, 0, sizeof(TTREXDeviceOptAskRsp));
    ask.stationNO = station_no;
    ask.pointID = dev->inner_id;
    ask.deviceSrcState = 1;
    ask.deviceDstState = 0;
    if(operate)
    {
        ask.deviceSrcState = 0;
        ask.deviceDstState = 1;
    }
    int ret = GCTREXDeviceOptAsk(psm_sys()->ticket_service, &ask, &result);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    if(0==result.result)
    {
        return NUMBER_FALSE;
    }

    return NUMBER_TRUE;
}

psm_exec_t *psm_find_exec_item(int station_no, DWORD32 device_id, DWORD32 exec_id)
{
    psm_exec_t *found_item = NULL;
    lock();
    int count = psm_sys()->exec_list.count;
    psm_exec_t *item = psm_sys()->exec_list.item;
    for(int i=0;i<count;i++)
    {
        psm_exec_t *n = (psm_exec_t *)(item+i);
        if(!n->enable)
        {
            continue;
        }
        if(station_no==n->station_id&&device_id==n->device_id&&exec_id==n->exec_id)
        {
            found_item = n;
            break;
        }
    }
    unlock();

    return found_item;
}

void psm_add_exec_item(int station_no, DWORD32 device_id, DWORD32 exec_id)
{
    psm_exec_t *found_item = psm_find_exec_item(station_no, device_id, exec_id);
    if(NULL!=found_item)
    {
        return;
    }
    lock();
    int count = psm_sys()->exec_list.count;
    psm_exec_t *item = psm_sys()->exec_list.item;
    int size = sizeof(psm_exec_t)*(count+1);
    item = (psm_exec_t *)realloc(item, size);
    if(NULL==item)
    {
        count = 0;
    }
    else
    {
        psm_exec_t *n = (psm_exec_t *)(item+count);
        memset(n, 0, sizeof(psm_exec_t));
        n->enable = NUMBER_TRUE;
        n->station_id = station_no;
        n->device_id = device_id;
        n->exec_id = exec_id;
        double now;
        GET_TIME(now);
        n->exec_time = now;
        count++;
    }
    psm_sys()->exec_list.count = count;
    psm_sys()->exec_list.item = item;

    unlock();

    psm_save_exec();
}


int psm_device_operate(int station_no, char *device, int device_len, int operate)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }

    void *s = psm_sys()->ticket_service;
    if(NULL==s)
    {
        return NUMBER_FALSE;
    }
    device_t *dev = find_device_by_name(station_no, device_len, device);
    if(NULL==dev)
    {
        return NUMBER_FALSE;
    }
    psm_append_device_extend(dev);
    if(NULL==dev->extend)
    {
        return NUMBER_FALSE;
    }

    psm_device_extend_t *ext = (psm_device_extend_t *)dev->extend;
    switch (operate) {
    case 0:
    {
        TTREXDevOptOrder list;
        memset(&list, 0, sizeof(TTREXDevOptOrder));
        DWORD32 exec_id = 0;
        memcpy(list.macNum, dev->name, strlen(dev->name));
        int ret = GCTREXDeicevOpt(s, station_no, &list, 1, 1, exec_id);
        if(0>ret)
        {
            return NUMBER_FALSE;
        }
        ext->ticket_id = exec_id;
        psm_add_exec_item(station_no, dev->inner_id, exec_id);
        return NUMBER_TRUE;
    }
        break;
    case 1:
    {
        psm_exec_t *n = psm_find_exec(station_no, dev->inner_id);
        if(NULL==n)
        {
            return NUMBER_FALSE;
        }
        int ret = GCTREXDeicevOptCancle(s, station_no, n->exec_id);
        if(0>ret)
        {
            return NUMBER_FALSE;
        }
        return NUMBER_TRUE;
    }
        break;
    default:
        break;
    }
    return NUMBER_FALSE;
}


int psm_receive_tikcet(int station_no, jk_device_list_t *list)
{
    psm_station_t *station = find_station(station_no);
    if(NULL==station)
    {
        return NUMBER_FALSE;
    }

    void *s = psm_sys()->ticket_service;
    if(NULL==s)
    {
        return NUMBER_FALSE;
    }
    int count = list->count;
    if(1>count)
    {
        return NUMBER_FALSE;
    }
    int size = sizeof(TTREXDevOptOrder)*count;
    TTREXDevOptOrder *ticket = (TTREXDevOptOrder *)malloc(size);
    if(NULL==ticket)
    {
        return NUMBER_FALSE;
    }
    memset(ticket, 0, size);
    for(int i=0;i<count;i++)
    {
        jk_device_t *d = (jk_device_t *)(list->device+i);
        TTREXDevOptOrder *n = (TTREXDevOptOrder *)(ticket+i);
        memcpy(n->macNum, d->device, d->device_len);
        n->devSrcState = d->state_1;
        n->devDstState = d->state_2;
    }
    DWORD32 exec_id = 0;
    int ret = GCTREXDeicevOpt(s, station_no, ticket, count, 0, exec_id);
    if(0>ret)
    {
        return NUMBER_FALSE;
    }
    return NUMBER_TRUE;
}
