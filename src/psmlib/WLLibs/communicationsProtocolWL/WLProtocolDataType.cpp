// NetSvcDataType.cpp: implementation of the CNetSvcDataType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WLProtocolDataType.h"

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif

void debugProtocolWL(const char * str,BYTE * pStrem,int len,const char * ip,WORD port)
{
	static char cmdStr[2048];

	//ƴǰ׺�ַ�
	memset(cmdStr,0,sizeof(cmdStr));
	sprintf(cmdStr,"[%s:%d] %s ",ip,port,str);
	char * s = cmdStr + strlen(cmdStr);

	//ƴ����ͷ
	TAPDUHeader * pHeader = (TAPDUHeader *)pStrem;
	sprintf(s,"%.4X %.2X [%.2X] %.2X %.2X %.4X %.8X %.8X %.4X %.4X(%.4d) | ",
		              pHeader->syncheader,
					  pHeader->cla,
					  pHeader->ins,
					  pHeader->p1,
					  pHeader->p2,
					  pHeader->sn,
					  pHeader->id,
					  pHeader->remoteID,
					  pHeader->flag,
					  pHeader->lc,pHeader->lc
					  );

	if (pHeader->lc > 200)	pHeader->lc = 200;

	//ƴ������
	int headerLen = sizeof(TAPDUHeader);
	char * pBodyStr = s + strlen(s);
	for (int i =0;i<pHeader->lc && i < sizeof(cmdStr) - (headerLen*2 + 100) - sizeof(TAPDUTailer) ;i++)
	{
		sprintf(&pBodyStr[2*i],"%.2X",pStrem[headerLen + i]);
	}
	sprintf(&pBodyStr[pHeader->lc *2],"%s"," | ");

	//ƴ����β
	TAPDUTailer * pTailer = (TAPDUTailer *)(pStrem + headerLen + pHeader->lc);
	char * pTailerStr = s + strlen(s);
	sprintf(pTailerStr,"%.4X %.4X !",pTailer->crc,pTailer->synctailer);

	//���ֻ��200���ַ�
	cmdStr[200] = '\0';

	//��ӡ
	printf(cmdStr);
	printf("\n");

}

