#ifndef __TICKETRELATEEXDATATYPE_H__
#define __TICKETRELATEEXDATATYPE_H__

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
typedef struct _stTREXDeviceOptAskRqt
{
	DWORD32 stationNO;		//վ���(ע����վID����)
	DWORD32 pointID;        //�豸��ţ����ձ����豸��ţ�
	DWORD32 deviceSrcState; //�豸ԭ״̬
	DWORD32 deviceDstState; //�豸Ŀ��״̬
}TTREXDeviceOptAskRqt;

//�豸������ѯ�ظ�
typedef struct _stTREXDeviceOptAskRsp
{
	DWORD32 stationNO;		//վ���(ע����վID����)
	DWORD32 pointID;        //�豸��ţ����ձ����豸��ţ�
	DWORD32 deviceSrcState; //�豸ԭ״̬
	DWORD32 deviceDstState; //�豸Ŀ��״̬
	DWORD32 result;         //��ѯ�����0�����������1�������
}TTREXDeviceOptAskRsp;

//�豸��������
typedef struct _stTREXDevOptOrder
{
	char macNum[64];         //�豸�����
	DWORD32 devSrcState;     //�豸ԭ״̬
	DWORD32 devDstState;     //�豸Ŀ��״̬
}TTREXDevOptOrder;


typedef struct _stTREXKBQOptResult
{
	DWORD32 stationNO;       //վ���
	DWORD32 devPointID;      //�豸���
	DWORD32 decSrcState;     //�豸ԭ״̬
	DWORD32 devDstState;     //�豸Ŀ��״̬
    DWORD32 KBQDstState;     //KBQĿ��״̬��0�֣�1��
    DWORD32 KBQOptResult;    //KBQ���������0ʧ�ܣ�1�ɹ�
}TTREXKBQOptResult;


//Ʊ�������
typedef struct _stTREXTicketOptResult
{
	DWORD32 stationNO;      //վID
	DWORD32 operateResult;  //���������0ʧ�ܣ�1�ɹ�
	DWORD32 failedReason;   //ʧ��ԭ��(���������Ϊʧ��ʱ�ò�����Ч������ʧ��ԭ��)��0δ֪ԭ��1KBQ����ʧ�ܡ�
}TTREXTicketOptResult;


//KBQ״̬�㱨������KBQ״̬�ı�ʱ�����㱨���ⲿ
typedef struct _stTREXKBQStateReport
{
	DWORD32 stationNO;       //վ���
	DWORD32 devPointID;      //�豸���
	DWORD32 isKBQShare;      //KBQ�Ƿ���(�豸��ӦKBQ�Ƿ�ֺ�һ�壬���豸�ֺ�Ϊ��ͬKBQ����)
	DWORD32 HKBQState;       //�豸������ӦKBQ״̬
	DWORD32 LKBQState;       //�豸������ӦKBQ״̬
}TTREXKBQStateReport;


//KBQ�����������ص�����(Ʊִ��ʱ���KBQ���)
typedef int (* GCTREXCallbackKBQOptResult)(TTREXKBQOptResult * pKBQOptResult, void * pContext);
//Ʊ�����������ص�����(����KBQ״̬�ı�ʱ�����㱨���ⲿ)
typedef int (* GCTREXCallbackKBQStateReport)(TTREXKBQStateReport * pKBQStateReport, void * pContext);
//KBQ״̬�㱨����ص�����(��Ե��ز�����Ʊ����ִ�н��)
typedef int (* GCTREXCallbackTicketOptResult)(TTREXTicketOptResult * pTicketOptResult, void * pContext);


#endif
