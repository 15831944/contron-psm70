#ifndef GLOBAL_H
#define GLOBAL_H

#include "sys/platform.h"
#include "log.h"

/*!
  系统支持的最大地线控制器数量
*/
#define MAX_CONTROLLER 512
/*!
  单个地线控制器最大桩数量
*/
#define MAX_DEVICE 84
/*!
  单次TCP通讯最大处理字节数量
*/
#define MAXTCPSIZE 1024

#endif // GLOBAL_H

