//#ifdef WIN32
//    #include <Winsock2.h>
//#else
//    #include <sys/types.h>
//    #include <sys/socket.h>

//    #define closesocket close
//#endif
//#include <stdio.h>

#include "sys/platform.h"
#include "conn/tcp.h"

#define D_PORT      4344
#define D_HOST      "localhost"
#define D_QUEUE     32
#define D_SOCKETS   16
#define D_INFO      256



void tcp_error(void *c)
{
    tcp_t *tcp = (tcp_t *)c;
#if WIN32
    TerminateThread(tcp->receive_thread, 0);
    CloseHandle(tcp->receive_thread);
#else
    pthread_cancel(tcp->receive_thread);
#endif
    printf("tcp error...");


}

THREAD_API setup_thread_cb( void *c )
{
    tcp_t *tcp = (tcp_t *)c;
    static int count = 0;
    fd_set fds;
    while(count <1)
    {
        FD_ZERO(&fds);
        FD_SET(tcp->fd, &fds);
        int hifd = 0;
        hifd = MAX(hifd, (int)tcp->fd);
        if(0==hifd)
        {
            break;
        }

        //设置等待时间100ms
        struct timeval tv;
        tv.tv_sec =0;
        tv.tv_usec = 100000;

        int ret = select(hifd+1, &fds, NULL, NULL, &tv);
        printf("ret=%d\n", ret);
        if(-1==ret)
        {
            continue;
        }
        if(0<ret)
        {
            if(FD_ISSET(tcp->fd, &fds))
            {
                char buffer[1024] = {0};
                int len = recv(tcp->fd, buffer, sizeof(buffer), 0);
                printf("receive len=%d\n", len);
                if(0<len)
                {
                    for(int i=0;i<len;i++)
                    {
                        printf("%02X ", (unsigned char)buffer[i]);
                    }
                    printf("\n");
                }
                else
                {
                    tcp_error(tcp);
                    break;
                }
                UCHAR d[] = {0x55, 0xaa, 0x03, 0x03, 0xc9};
                tcp_send(tcp, (char *)d, sizeof(d));
            }
        }

    }

    return NULL;
}

int main(int argc, char **argv) {
    char s[MAX_STRING];
//    sprintf(s, "%s", "192.168.8.101");
    sprintf(s, "%s", "localhost");
    tcp_t tcp[1];
//    = {{s, //hostname
//                    4001,//port
//                    0,//socket_handle
//                    0//receive_thread
//                   }};
    tcp_config(&tcp[0], s, 4001);
    printf("server=%s, port=%d\n", tcp[0].hostname, tcp[0].port);
    tcp_connect(tcp);
    if(!tcp_isvalid(tcp))
    {
        printf("cann't connect to server..\n");
        return 1;
    }
    printf("connected to server...\n");
//    char d[] = {0xA5, 0x5A, 0x0, 0x12, 0x34};
    UCHAR d[] = {0x55,0xaa,0x05,0x02,0x00,0x00,0x67};
    if(!tcp_send(tcp, (char *)d, sizeof(d)))
    {
        printf("cann't send data to server...\n");
    }
    else
    {
        printf("sended...\n");
    }
    int ret;
    THREAD_CREATE(&tcp->receive_thread[0], setup_thread_cb, &tcp, ret)
//#if WIN32
//    tcp->receive_thread = CreateThread(NULL, 0, setup_thread_cb, &tcp, 0, NULL);
//    ret = (NULL != tcp->receive_thread);
//#else
//    ret = (0 == pthread_create(socket->receive_thread, NULL, setup_thread_cb, socket));
//#endif
    if(ret)
    {
        THREAD_CLOSE(tcp->receive_thread[0]);
//#if WIN32
//        WaitForSingleObject(tcp->receive_thread, INFINITE);
//        CloseHandle(tcp->receive_thread);
//#else
//        pthread_join(*(socket->receive_thread), NULL);
//#endif
    }
    return 0;


    struct timeval tv;
    struct sockaddr_in addr;
    struct hostent *host;
    unsigned int descriptor;
    int result;
    int index;
    int cycle = 0;
    int delay = 0;
    unsigned int sockets[D_SOCKETS];
    int sockets_index = 0;
    unsigned int maximun;
    char buffer[D_INFO];
    fd_set input;

    /*  read the delay if any  */
    if (argc > 1)
        delay = atol(argv[1]);
    else
        delay = 0;

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif  /*  WIN32  */

    /*  create a socket  */
    descriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (descriptor == -1) {
        perror("socket");
        return (1);
    }

    /*  get information about the host  */
    memset(&addr, 0, sizeof(addr));
    host = gethostbyname(D_HOST);
    if (host == NULL) {
        perror("gethostbyname");
        closesocket(descriptor);
#ifdef WIN32
        WSACleanup();
#endif
        return (1);
    }

    /*  bind the socket to an address and port  */
    memcpy(&addr.sin_addr, host->h_addr_list[0], sizeof(host->h_addr_list[0]));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(D_PORT);
    result = bind(descriptor, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1) {
        perror("bind");
        closesocket(descriptor);
#ifdef WIN32
        WSACleanup();
#endif
        return (1);
    }

    /*  listen for connections  */
    result = listen(descriptor, D_QUEUE);
    if (result == -1) {
        perror("listen");
        closesocket(descriptor);
#ifdef WIN32
        WSACleanup();
#endif
        return (1);
    }

    memset(sockets, 0, sizeof(sockets));
    maximun = descriptor;

    result = 0;
    while (result != -1) {
        FD_ZERO(&input);
        FD_SET(descriptor, &input);
        for (result = 0; result < sockets_index; result++)
            FD_SET(sockets[result], &input);

        tv.tv_sec  = delay;
        tv.tv_usec = 0;
        if (delay == -1)
            result = select(maximun + 1, &input, NULL, NULL, NULL);
        else
            result = select(maximun + 1, &input, NULL, NULL, &tv);
        switch (result) {
            /*  error in select  */
            case -1:
                perror("select");
                break;

            /*  nothing to process  */
            case 0:
                break;

            /*  a number of sockets are ready for reading  */
            default:
                /*  check if the descriptor set is our listening one  */
                if (FD_ISSET(descriptor , &input)) {
                    sockets[sockets_index] = accept(descriptor, NULL, NULL);
                    if (sockets[sockets_index] == -1) {
                        perror("accept");
                    }
                    else {
                        if (sockets[sockets_index] > maximun)
                            maximun = sockets[sockets_index];

                        sockets_index++;
                    }
                }
                /*  one of the sockets is sending data. Find it  */
                else {
                    for (index = 0; index < sockets_index; index++) {
                        if (FD_ISSET(sockets[index], &input)) {
                            memset(buffer, 0, sizeof(buffer));

                            /*  read information from socket  */
                            result = recv(sockets[index], buffer, sizeof(buffer), 0);
                            if (result == -1)
                                perror("recv");
                            else
                                printf("Received %d bytes from descriptor %d: %s/n", result, sockets[index], buffer);
                        }
                    }
                }
        }

        printf("%d/r", cycle++);
    }

    for (result = 0; result < sockets_index; result++) {
        closesocket(sockets[sockets_index]);
    }

    closesocket(descriptor);
#ifdef WIN32
    WSACleanup();
#endif

    return (0);
}
