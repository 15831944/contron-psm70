/********************************************************************************************************************
���ܣ����߷���ͨѶģ����ؽӿ�
˵������1�����밴һ��˳�����
���ߣ�Τ��Զ	2014-09-26
********************************************************************************************************************/
#ifndef	GROUNDSERVERINTERFACE_H
#define GROUNDSERVERINTERFACE_H

//#define GROUNDSERVER_EXPORTS

#ifdef _WIN32
#ifdef GROUNDSERVER_EXPORTS
#define GROUNDSERVERAPI extern "C" __declspec(dllexport)
#else
#define GROUNDSERVERAPI extern "C" __declspec(dllimport)
#endif
#else
#define GROUNDSERVERAPI
#endif

#ifdef _WIN32
//#define GROUNDCALL_TYPE _stdcall	//windowsϵͳ�õĻص�����һ�㶼��_stdcall��WIN32 Api������_stdcall���÷�ʽ
#define GROUNDCALL_TYPE __cdecl	//����C��C++Ĭ�ϵ��÷�ʽ��ʹ�����ַ�ʽQT creator�༭�����Ծ�̬/��ʽ����
#else
#define GROUNDCALL_TYPE
#endif

typedef struct _GroundServerConfig
{
	unsigned int stationNO;		//վ��
	unsigned int controllerNO;   //��������
	unsigned int stakeNO;        //׮��
	unsigned char label[16];     //���߱�ǩ
}TGroundServerPointConfig;

//���߿��������ò���
typedef struct _GroundServerControllerConfig
{
	unsigned int stationNO;		//վ��
	unsigned int controllerNO;  //��������
	unsigned char ipAddr[16];   //��ַ
	unsigned int  port;         //�˿ں� 
} TGroundServerControllerConfig;

/*********************************************************************
���ܣ�ʵ��������
��������
���أ�0,�ɹ�����0,ʧ�ܡ�
���ߣ�Τ��Զ 2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundServerInit();

/*********************************************************************
���ܣ�����ʵ��������
��������
���أ�<0,ʧ�ܡ�
      >=0���ɹ�
���ߣ�Τ��Զ 2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundServerUninit();

/*********************************************************************
���ܣ�����/���������ص�ָ��
������stationNO վ��
      controllerNO ��������
      stakeNO ׮��
���أ�0��ʾ�ɹ�����0��ʾ���ɹ�
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
typedef int (* GroundCallBack)(unsigned int stationNO, unsigned int controllerNO, unsigned int stakeNO);

/*********************************************************************
���ܣ�֪ͨ�ص�����ָ��
������stationNO վ��
���أ�0��ʾ�ɹ�����0��ʾ���ɹ�
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
typedef int (* SynCallBack)(unsigned int stationNO, TGroundServerPointConfig* config, unsigned int count);

/*********************************************************************
���ܣ�ע��ص�����
������Lock ���߱����ص�����ָ��
      UnLock ���߽����ص�����ָ��
	  Update ���µ���״̬�ص�����ָ��
	  Syn �Զ�ͬ���ص�����ָ��
���أ���
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI void GROUNDCALL_TYPE GCRoundServerRegedit(GroundCallBack Lock, GroundCallBack UnLock, GroundCallBack Update, SynCallBack Syn);

/*********************************************************************
���ܣ��ӵ���ж�ȡ���ĵ��ߵ�����
��������
���أ����ߵ�����
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetPointCount();

/*********************************************************************
���ܣ��ӵ���ж�ȡ���ĵ��ߵ���Ϣ
������config �ӵ���ж����ĵ�����Ϣ���飬�ڵ��ô˺���ǰ�����ڴ�ռ�
      count ��ʾconfig�Ĵ�С
���أ�> 0 ��ʾ�ɹ���ʵ�ʷ��ص����ø���
	  < 0 ��ʾ���ɹ�
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetPointConfig(TGroundServerPointConfig* config, unsigned int count);

/*********************************************************************
���ܣ���ȡ���߿��������ø���
��������
���أ����ߵ�����
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetControllerCount();

/*********************************************************************
���ܣ���ȡ���߿���������
������controller �ӵ���ж����ĵ�����Ϣ���飬�ڵ��ô˺���ǰ�����ڴ�ռ�
      count ��ʾcontroller�Ĵ�С
���أ�> 0 ��ʾ�ɹ���ʵ�ʷ��ص����ø���
      < 0 ��ʾ���ɹ�
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundGetControllerConfig(TGroundServerControllerConfig* controller, unsigned int count);

/*********************************************************************
���ܣ����õ���״̬
������controllerNO��������
      stakeNO׮��
���أ�0��ʾ�ɹ�����0��ʾ���ɹ�
���ߣ�Τ��Զ  2015-01-20
*********************************************************************/
GROUNDSERVERAPI int GROUNDCALL_TYPE GCRoundSetValue(unsigned int stationNO, unsigned int controllerNO, unsigned int stakeNO, unsigned int value);

#endif // GROUNDSERVERINTERFACE_H