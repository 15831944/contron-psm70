#include <iostream>

using namespace std;

#include "psm_sys.h"

int main()
{    
    cout << "Hello World!" << endl;
//	test();
//	return 0;

    char ip[] = "127.0.0.1";
    psm_config_t config;
    config.wf_ip = ip;
    config.wf_port = 5555;

    psm_start(&config);

    getchar();

    return 0;
}

