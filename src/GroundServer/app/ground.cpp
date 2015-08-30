#include "ground.h"

#include <iostream>
using namespace std;
#include <conio.h>


/*!
 * \brief self 全局地线程序
 * 在ground_new中初始化
 * 在ground_close中置NULL
 */
static ground_t *self = NULL;

/*!
 * \brief ground_app 获取全局地线程序
 * \return 地线程序
 * 外部单元调用
 */
ground_t *ground_app()
{
    return self;
}

/*!
 * \brief ground_new 创建地线程序
 * \param config 程序配置
 * \return 地线程序，创建失败则返回NULL
 */
ground_t *ground_new(config_t *config)
{
    ground_t *ground;
    ground = (ground_t *)malloc(sizeof(ground_t));
    if (NULL == ground)
    {
        return NULL;
    }
    memset(ground, 0, sizeof(ground_t));
    memcpy(ground->config, config, sizeof(config_t));
    MUTEX_INIT(&ground->ground_critical);
    if(!self)
    {
        self = ground;
    }
    return ground;
}

/*!
    \brief ground_start 启动地线程序
    \param ground 地线程序
    启动所有控制器通讯，创建定时汇报地线状态线程
 */
void ground_start(ground_t *ground)
{
    ground->ready = 1;
    config_t *config = ground->config;
    int count = config->controller_count;
    for(int i=0;i<count;i++)
    {
        controller_t *controller = &config->controller[i];
        controller->reconnect_interval = 15;
        ground_controller_start(controller);
    }
    int ret,success;
    THREAD_CREATE(&ground->state_thread[0], ground_state_thread, ground, ret);
    success = ret;
    THREAD_CREATE(&ground->console_thread[0], ground_console_thread, ground, ret);
    success = success & ret;
    THREAD_CREATE(&ground->guard_thread[0], ground_guard_thread, ground, ret);
    success = success & ret;


    if(success)
    {
        THREAD_RUN(ground->state_thread[0], false);
        THREAD_RUN(ground->console_thread[0], false);
        THREAD_RUN(ground->guard_thread[0], true);

//        Sleep(10);

//        ground_ready_set(ground, 0);  //关闭地线程序
    }
    else
    {
        ground_close(ground);
    }

}

THREAD_API ground_state_thread(void *param)
{
    ground_t *ground = (ground_t *)param;
    config_t *config = ground->config;
    int lastStateTime;
    GET_TIME(lastStateTime);
    int interval = config->state_interval;


    while (true) {
        int ready = ground_ready_get(ground);
        if(!ready)
        {
            break;
        }
        int now;
        GET_TIME(now);
        bool outOfTime = (now > (lastStateTime + interval));
        if(outOfTime)
        {
//            ground_log("interval: %d\nlast time: %d\ncurrent time: %d\nout of time: %d\n",
//                   interval, lastStateTime, now, outOfTime);
            lastStateTime = now;
            ground_state_report(ground);
        }
        else
        {
            Sleep(100);
        }
    }
    ground_log("leave state thread...\n");
    return NULL;
}

THREAD_API ground_console_thread(void *param)
{
    ground_t *ground = (ground_t *)param;
    int ready = 0;
    do
    {
        cout << "Enter supper password to exit:" << endl;
        char password[] = {49,50,51};
        int size = sizeof(password);
        char *buffer = (char *)malloc(size);
		memset(buffer, 0, size);
        int index = 0;
        do
        {
            Sleep(100);
            buffer[index] = getchar();
            index++;
            if((index==sizeof(password)))
            {
                if(0==memcmp(password, buffer, sizeof(password)))
                {
                    ground_ready_set(ground, 0);
                    break;
                }
                cout << endl;
                index = 0;
            }
        }while(index < size);
        ready = ground_ready_get(ground);

    }while(ready);
ground_log("leave console thread...\n");
    return NULL;
}


THREAD_API ground_guard_thread(void *param)
{
    ground_t *ground = (ground_t *)param;
    while(true)
    {
        int ready = ground_ready_get(ground);
        if(ready)
        {
            Sleep(10);
        }
        else
        {
            break;
        }
    }
    ground_close(ground);
    return NULL;
}


/*!
 * \brief ground_state_report  汇报地线状态
 * \param ground  地线程序
 * 向外部系统汇报地线状态
 */
void ground_state_report(ground_t *ground)
{
    if(!ground_intf()->statereport_interval)
    {
        return;
    }
    ground_enter(ground);
    config_t *config = ground->config;
    ground_leave(ground);
    int count = config->controller_count;
    for(int i=0;i<count;i++)
    {
        ground_controller_state_interval(&config->controller[i]);
    }
}

/*!
 * \brief ground_ready_get 取地线程序ready值
 * \param ground 地线程序
 * \return 地线程序ready值
 * 线程安全
 */
int ground_ready_get(ground_t *ground)
{
    int ready = 0;
    ground_enter(ground);
    ready = ground->ready;
    ground_leave(ground);
    return ready;
}

/*!
 * \brief ground_close 关闭地线程序
 * \param ground 地线程序
 */
void ground_close(ground_t *ground)
{
    ground_enter(ground);
    if(self==ground)
    {
        self = NULL;
    }
    config_t *config = ground->config;
    int count = config->controller_count;
    for(int i=0;i<count;i++)
    {
        controller_t *controller = &config->controller[i];
        ground_controller_close(controller);
    }
    ground_log("ground close...\n");
    THREAD_CLOSE(ground->state_thread[0]);
    THREAD_CLOSE(ground->console_thread[0]);
    THREAD_CLOSE(ground->guard_thread[0]);
    ground_leave(ground);
    MUTEX_FREE(&ground->ground_critical);
}

/*!
 * \brief ground_enter 进入临界区
 * \param ground 地线程序
 */
void ground_enter(ground_t *ground)
{
    MUTEX_LOCK(&ground->ground_critical);
}

/*!
 * \brief ground_leave 退出临界区
 * \param ground 地线程序
 */
void ground_leave(ground_t *ground)
{
    MUTEX_UNLOCK(&ground->ground_critical);
}

/*!
 * \brief ground_ready_set 设置地线程序运行状态
 * \param ground 地线程序
 * \param ready 0-程序关闭，1-程序运行中
 * 线程安全
 */
void ground_ready_set(ground_t *ground, int ready)
{
    ground_enter(ground);
    ground->ready = ready;
    ground_leave(ground);
}
