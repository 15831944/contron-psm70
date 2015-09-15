#include "switchertestapp.h"
#include "log.h"
#include "config.h"
#include "tester.h"
#include "testclient.h"
#include "testserver.h"
#include "sys/platform.h"
#include "conn/tcp.h"

SwitcherTestApp::SwitcherTestApp()
{

}

SwitcherTestApp::~SwitcherTestApp()
{

}

int SwitcherTestApp::run()
{
    log_init();
    tcp_init();

    config_t *cfg = loadConfig();
    Tester *test = NULL;
    if(cfg->IsServer)
    {
        test = new TestServer();
        TestServer *p = (TestServer *)test;
        p->setPort(cfg->ServerPort);
    }
    else
    {
        test = new TestClient();

        TestClient *p = (TestClient *)test;
        p->setIp(cfg->ServerIP);
        p->setPort(cfg->ServerPort);
    }
    test->start();

    while(true)
    {
        Sleep(100);
    }

    return 0;
}

