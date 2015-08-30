#include "psm.h"


static gapp s_app = 0;
int ground_psm_start(gapp app)
{
    s_app = app;
    return NUMBER_SUCCESS;
}

/*!
 * \brief ground_psm_init PSM初始化
 * \param config 程序配置
 * \return 成功返回NUMBER_SUCCESS；失败返回NUMBER_ERROR
 * 从PSM读取地线控制器配置，写入程序配置中；并向PSM注册解/闭锁、查询回调函数
 */
int ground_psm_init(config_t *config)
{
    int ret = GCRoundServerInit();
    ground_log("psm interface init:%d\n", ret);
    if(0!=ret)
    {
        return NUMBER_ERROR;
    }
    //获取地线控制器配置
    {
        int count = GCRoundGetControllerCount();
        ground_log("psm get controller count:%d\n", count);
        if(1>count)
        {
            return NUMBER_SUCCESS;
        }
        int len = count*sizeof(TGroundServerControllerConfig);
        TGroundServerControllerConfig *cfg =(TGroundServerControllerConfig *)malloc(len);
        memset(cfg, 0, len);
        ret = GCRoundGetControllerConfig(cfg, count);
        ground_log("psm get controller config:%d\n", ret);
        if(0!=ret||MAX_CONTROLLER<count)
        {
            return NUMBER_ERROR;
        }
        config->controller_count = count;
		{
			TGroundServerControllerConfig *p = cfg;
			for(int i=0;i<count;i++)
			{
				int station = p->stationNO;
				int controller = p->controllerNO;
				int port = p->port;
				char *address = (char *)&p->ipAddr[0];
				ground_log("psm controller config:station=%d,controller=%d,ip=%s,port=%d\n",
					station, controller, address, port);
				memset(&config->controller[i], 0, sizeof(controller_t));
				config->controller[i].station_no = station;
				config->controller[i].contronller_no = controller;
				tcp_t *tcp = &config->controller[i].conn[0].tcp[0];
				int minlen = min(sizeof(tcp->hostname), sizeof(p->ipAddr));
				memcpy(tcp->hostname, address, minlen);
				tcp->port = port;
				p++;
			}
		}
        free(cfg);
    }
    //获取点表
    {
        int count = GCRoundGetPointCount();
        ground_log("psm get ground count:%d\n", count);
        int len = count*sizeof(TGroundServerPointConfig);
        TGroundServerPointConfig *cfg = (TGroundServerPointConfig *)malloc(len);
        memset(cfg, 0, len);
        ret = GCRoundGetPointConfig(cfg, count);
        ground_log("psm get ground config:%d\n", ret);
        if(count==ret)  //分配的数量与返回数据不一致，则出错
        {
			TGroundServerPointConfig *p = cfg;
            for(int i=0;i<count;i++)
            {
                int stationNo = p->stationNO;
                int controllerNo = p->controllerNO;
                int no = p->stakeNO;
                ground_log("psm ground config:station=%d,controller=%d,id=%d\n",
                           stationNo, controllerNo, no);
                p++;
            }
        }
		free(cfg);
    }
    ground_log("psm set lock/unlock/query function\n");
    GCRoundServerRegedit(ground_psm_lock,
                         ground_psm_unlock,
                         ground_psm_query,
                         NULL);
    return NUMBER_SUCCESS;
}


void ground_psm_free()
{
    GCRoundServerUninit();
}


int ground_psm_unlock(unsigned int station, unsigned int controller, unsigned int id)
{
    ground_log("ground psm unlock:station=%d,controller=%d,id=%d\n", station, controller, id);
    if(!s_app)
    {
        return 0;
    }
    ground_t *ground = s_app();
    if(!ground)
    {
        return 0;
    }
    controller_t *target = NULL;
    ground_enter(ground);
    int count = ground->config[0].controller_count;
    for(int i=0;i<count;i++)
    {
        controller_t *c = &ground->config[0].controller[i];
        bool found = false;
        ground_controller_enter(c);
        if((station==(unsigned int)c->station_no)
                &&(controller==(unsigned int)c->contronller_no))
        {
            target = c;
            found = true;
        }
        ground_controller_leave(c);
        if(found)
        {
            break;
        }

    }
    ground_leave(ground);
    if(!target)
    {
        return 0;
    }
    int ret = ground_controller_unlock(target, id);
    return ret;
}


int ground_psm_lock(unsigned int station, unsigned int controller, unsigned int id)
{
    ground_log("ground psm lock:station=%d,controller=%d,id=%d\n", station, controller, id);
    if(!s_app)
    {
        return 0;
    }
    ground_t *ground = s_app();
    if(!ground)
    {
        return 0;
    }
    controller_t *target = NULL;
    ground_enter(ground);
    int count = ground->config[0].controller_count;
    for(int i=0;i<count;i++)
    {
        controller_t *c = &ground->config[0].controller[i];
        bool found = false;
        ground_controller_enter(c);
        if((station==(unsigned int)c->station_no)
                &&(controller==(unsigned int)c->contronller_no))
        {
            target = c;
            found = true;
        }
        ground_controller_leave(c);
        if(found)
        {
            break;
        }

    }
    ground_leave(ground);
    if(!target)
    {
        return 0;
    }
    int ret = ground_controller_lock(target, id);
    return ret;
}


int ground_psm_query(unsigned int station, unsigned int controller, unsigned int id)
{
    ground_log("ground psm query:station=%d,controller=%d,id=%d\n", station, controller, id);
    if(!s_app)
    {
        return 0;
    }
    ground_t *ground = s_app();
    if(!ground)
    {
        return 0;
    }
    controller_t *target = NULL;
    ground_enter(ground);
    int count = ground->config[0].controller_count;
    for(int i=0;i<count;i++)
    {
        controller_t *c = &ground->config[0].controller[i];
        bool found = false;
        ground_controller_enter(c);
        if((station==(unsigned int)c->station_no)
                &&(controller==(unsigned int)c->contronller_no))
        {
            target = c;
            found = true;
        }
        ground_controller_leave(c);
        if(found)
        {
            break;
        }

    }
    ground_leave(ground);
    if(!target)
    {
        return 0;
    }
    int ret = ground_controller_query(target, id);
    return ret;
}


int ground_psm_statereport(int station, int controller, int id, int state)
{
    int ret = GCRoundSetValue((unsigned int)station, (unsigned int)controller,
                              (unsigned int)id, (unsigned int)(state+1));
    ground_log("ground psm report state:station=%d,controller=%d,id=%d,state=%d\n", station, controller, id, state+1);
    return ret;
}
