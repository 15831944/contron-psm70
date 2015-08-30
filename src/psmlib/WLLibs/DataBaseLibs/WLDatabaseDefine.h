#ifndef __WL_DATABASE_DEFINE__
#define __WL_DATABASE_DEFINE__

#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC // Compile OTL 4/ODBC
#ifndef WL_OS_CORE_WIN
#define OTL_ODBC_UNIX
#endif
#include "otlv4.h" // include the OTL 4 header file


#endif