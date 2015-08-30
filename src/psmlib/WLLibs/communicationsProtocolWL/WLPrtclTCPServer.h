// WLPrtclTCPServer.h: interface for the CWLPrtclTCPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLPRTCLTCPSERVER_H__40C2C3E4_AB61_40FB_AA3B_7638EE02401F__INCLUDED_)
#define AFX_WLPRTCLTCPSERVER_H__40C2C3E4_AB61_40FB_AA3B_7638EE02401F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../communicationModel/TCPServer.h"
#include "WLProtocolDataType.h"

class CWLPrtclTCPServer : public CTCPServer
{
public:
	CWLPrtclTCPServer(CCommunicationProtocol * pProtocol);
	virtual ~CWLPrtclTCPServer();

};

#endif // !defined(AFX_WLPRTCLTCPSERVER_H__40C2C3E4_AB61_40FB_AA3B_7638EE02401F__INCLUDED_)
