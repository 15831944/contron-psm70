// WLPrtclTCPServer.cpp: implementation of the CWLPrtclTCPServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WLPrtclTCPServer.h"

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWLPrtclTCPServer::CWLPrtclTCPServer(CCommunicationProtocol * pProtocol) : CTCPServer(pProtocol)
{

}

CWLPrtclTCPServer::~CWLPrtclTCPServer()
{

}
