// WLProtocolDataType.h: .
// WLͨ��Э�����������Ͷ���
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLPROTOCOLDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_)
#define AFX_WLPROTOCOLDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLPlatformLayer.h"

//����״̬�ֳ���
#define SW_LEN 2                             //״̬�ֳ���Ϊ sizeof(WORD)�������ֽ�

//Э��ͨ��״̬�ֶ��� 90XX��FFXX��6AXX��61XX ΪЭ����ͨ��״̬�룬ϵͳ����������Э�鲻��ռ��
#define SW_SUCCESS                    0x9000 //�ɹ�
#define SW_NO_FULL_SUCCESS            0x9001 //δ��ȫ�ɹ������ֳɹ�������ʧ��
#define SW_SUCCESSED_YOURSELF         0x9002 //�ɹ�
#define SW_NO_RSP                     0xFFFF //�Է�����Ӧ  ͨ�Ŵ��� ��ʱ
#define SW_INPUT_PARAM_ERR            0xFFFE //����������Ϸ�
#define SW_FAILED_FORWARD             0xFFF0 //ת�����ݰ�ʧ�� 
#define SW_UNKNOWN_ERR                0x6A01 //δ֪����  
#define SW_NO_INIT                    0x6A02 //δ��ʼ��
#define SW_INS_NOTSUOOORT             0x6A03 //�����ֲ�֧��
#define SW_CRC_ERR                    0x6A04 //���������
#define SW_CMD_ERR                    0x6A05 //��������(������ �� CRC ��ȷ�����Ǳ��Ļ�������������CLA��LC��) 
#define SW_BUSY                       0x6A06 //����æ��������
#define SW_BREAKFASE_ISUSE            0x6A07 //�����ѱ�ʹ��
#define SW_DB_CONNECT_ERR             0x6A08 //���ݿ�δ���Ӵ���
#define SW_DB_OPERATION_ERR           0x6A09 //���ݿ��������
#define SW_RQT_REDO                   0x6100 //0x61XX ����δ�����꣬������ִ��XX�ο���ɣ����ظ�����



//CLA����
#define CLA_ENCRYPTION 0x80//DATA������
#define CLA_NOENCRYPT  0x00//DATA��δ����
#define CLA_MAC        0x40//DATA����MAC
#define CLA_NOMAC      0x00//DATA����MAC
#define CLA_RQT 0x00       //����
#define CLA_RSP 0x08       //��Ӧ


//INS����(ע��00-0F�����ܱ������Զ���Э��ʱ������ռ��)
//��λ�� <--> ��λ��
#define INS_COMM_DETECT                0x01 //ͨ�ż�⣬��������
#define INS_GET_SVR_TIME               0X02 //ȡ����ʱ��

//Э���ʽ����
#define SYNC_HEADER 0xA5A5 //ͨ��ͬ��ͷ
#define SYNC_TAILER 0xA3A3 //ͨ��ͬ��β

//#pragma pack (1)

//Ӧ�����ݵ�Ԫͷ
typedef struct  _APDUHeader{
	WORD  syncheader;      //����ͬ��ͷ���̶�Ϊ0xA5A5
	BYTE  cla;             //Э����𣬼�עһ
	BYTE  ins;             //�����֣�ͬһ����������ӦINS��ͬ����CLA�ж����������Ӧ����עһ
	BYTE  p1;              //����һ��Ĭ����0����������վ�������
	BYTE  p2;              //ͬ��
	WORD  sn;              //��ˮ�ţ�Ĭ����0���������������������壬��������վ������
	DWORD id;              //����Ψһ��ʶ����վ��
	DWORD remoteID;        //���շ�ID,Ĭ�ϸ�0����ת��ʱ�������ã�����·�ɣ�
	WORD  flag;            //���ã���ʱֻ���ֽڶ���������ã������˴������������ü����㷨��ʶ����Ϣ��
	WORD  lc;              //����������:0--1000
}TAPDUHeader;

//Ӧ�����ݵ�Ԫβ
typedef struct _APDUTailer{
	WORD crc;              //�����ַ�����ʱ��crc8�����һ���ֽڷ������λ
	WORD synctailer;       //����ͬ��β���̶�Ϊ0xA3A3
}TAPDUTailer;

//���������붨��
typedef struct _RQTCMDIN{
	BYTE cla;
	BYTE ins;
	BYTE p1;
	BYTE p2;
	WORD sn;
	WORD lc;
	BYTE * pData;
}TRqtCmdIn;

//��Ӧ�������붨��
typedef struct _RSPCMDIN{
	BYTE cla;
	BYTE ins;
	BYTE p1;
	BYTE p2;
	WORD sn;
	WORD lc;
	WORD sw;
	BYTE * pData;
}TRspCmdIn;

void debugProtocolWL(const char * str,BYTE * pStrem,int len,const char * ip,WORD port);

#endif // !defined(AFX_NETSVCDATATYPE_H__E2D56B0B_9B4E_4C31_9B32_C91C78E558E2__INCLUDED_)
