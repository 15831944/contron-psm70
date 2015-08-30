#include <stdio.h>
#include "sys/platform.h"

THREAD_API thread_method1(void *param)
{
    while(true)
    {

        Sleep(222);
        printf("i'm in thread_method1...\n");
    }
    return NULL;
}
THREAD_API thread_method2(void *param)
{
    while(true)
    {

        Sleep(333);
        printf("i'm in thread_method2...\n");
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    int ret;
    THREAD_CREATE(&t1, thread_method1, NULL, ret);
    if(ret)
    {
//        pthread_join(t1, NULL);
        THREAD_RUN(t1, false);
    }
    THREAD_CREATE(&t2, thread_method2, NULL, ret);
    if(ret)
    {
                THREAD_RUN(t2, true);
//        pthread_detach(t2);
    }
    printf("Hello World!\n");
    return 0;
}

