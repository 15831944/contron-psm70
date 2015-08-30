#ifndef __TICKETRELATEDATATYPE_H__
#define __TICKETRELATEDATATYPE_H__

typedef unsigned int        DWORD32;   //�޷������ֽ�
typedef unsigned long long  DWORD64;   //�޷��Ű��ֽڣ�long long�ͣ�32λ��64λ�ж�Ϊ���ֽ�

#ifdef _WIN32
typedef unsigned long       DWORD;
#else
typedef unsigned int        DWORD;
#endif
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;


//�豸������ѯ����
typedef struct _stDeviceOptAskRqt
{
	DWORD32 stationNO;		//վ���(ע����վID����)
	DWORD32 pointID;        //�豸��ţ����ձ����豸��ţ�
	DWORD32 deviceSrcState; //�豸ԭ״̬
	DWORD32 deviceDstState; //�豸Ŀ��״̬
}TDeviceOptAskRqt;

//�豸������ѯ�ظ�
typedef struct _stDeviceOptAskRsp
{
	DWORD32 stationNO;		//վ���(ע����վID����)
	DWORD32 pointID;        //�豸��ţ����ձ����豸��ţ�
	DWORD32 deviceSrcState; //�豸ԭ״̬
	DWORD32 deviceDstState; //�豸Ŀ��״̬
	DWORD32 result;         //��ѯ�����0�����������1�������
}TDeviceOptAskRsp;

//�豸��������
typedef struct _stTRDevOptOrder
{
	char macNum[64];         //�豸�����
	DWORD32 devSrcState;     //�豸ԭ״̬
	DWORD32 devDstState;     //�豸Ŀ��״̬
}TTRDevOptOrder;


typedef struct _stTRKBQOptResult
{
	DWORD32 stationNO;       //վ���
	DWORD32 ticketID;        //ƱID
	DWORD32 devPointID;      //�豸���
	DWORD32 decSrcState;     //�豸ԭ״̬
	DWORD32 devDstState;     //�豸Ŀ��״̬
	DWORD32 KBQDstState;     //KBQĿ��״̬
	DWORD32 KBQOptResult;    //KBQ���������0ʧ�ܣ�1�ɹ�
}TTRKBQOptResult;

//KBQ״̬�㱨������KBQ״̬�ı�ʱ�����㱨���ⲿ
typedef struct _stTRKBQStateReport
{
	DWORD32 stationNO;       //վ���
	DWORD32 devPointID;      //�豸���
	DWORD32 isKBQShare;      //KBQ�Ƿ���(�豸��ӦKBQ�Ƿ�ֺ�һ�壬���豸�ֺ�Ϊ��ͬKBQ����)
	DWORD32 HKBQState;       //�豸������ӦKBQ״̬
	DWORD32 LKBQState;       //�豸������ӦKBQ״̬
}TTRKBQStateReport;

//Ʊ�������
typedef struct _stTicketOptResult
{
	DWORD32 stationNO;      //վID
	DWORD32 ticketID;       //ƱID
	DWORD32 operateResult;  //���������0ʧ�ܣ�1�ɹ�
	DWORD32 failedReason;   //ʧ��ԭ��(���������Ϊʧ��ʱ�ò�����Ч������ʧ��ԭ��)��0δ֪ԭ��1KBQ����ʧ�ܡ�
}TTicketOptResult;


//KBQ�����������ص�����(Ʊִ��ʱ���KBQ���)
typedef int (* GCTRCallbackKBQOptResult)(TTRKBQOptResult * pKBQOptResult, void * pContext);
//Ʊ�����������ص�����(��Ե��ز�����Ʊ����ִ�н��)
typedef int (* GCTRCallbackTicketOptResult)(TTicketOptResult msgData, void *pContext);
//KBQ״̬�㱨����ص�����(����KBQ״̬�ı�ʱ�����㱨���ⲿ)
typedef int (* GCTRCallbackKBQStateReport)(TTRKBQStateReport * pMsgData, void *pContext);


#endif