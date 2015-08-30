// BusinessDataSet.cpp: implementation of the CFixedLenRecordFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FixedLenRecordFile.h"

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

/*********************************************************************
���ܣ����캯��������ʼ������
��������
���أ���
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
CFixedLenRecordFile::CFixedLenRecordFile(DWORD editerID)
{
	m_pCritical = new CWLCriticalSection();              //��ʼ���ٽ���

	memset(m_filename,0,sizeof(char)*MAX_PATH);          //�洢�ļ�����
	memset(&m_header,0,sizeof(TFileHeaderFixedLenRecord));     //�ļ�ͷ
	m_pBody       = NULL;                                //�ļ���
	m_maxRecCount = NULL;                                //����¼������������������ʱ����ֵ��

	m_editerID = editerID;

	m_reservedRecCountForBody = 0;                       //load�ɹ���ӦԤ����¼���������ռ䣩����������������ʱ����ֵ��
	m_recCountForBodyMem      = 0;                       //�����������ڴ���
    m_perRecLen = 0;                                     //ÿ����¼�ĳ���,�����ļ��Ϸ��Լ��
}

/*********************************************************************
���ܣ���������
��������
���أ���
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
CFixedLenRecordFile::~CFixedLenRecordFile()
{
	//�ͷ��ļ����ڴ�
	if (m_pBody != NULL)	delete [] m_pBody;

	//�ͷ��ٽ���
	delete m_pCritical;
}

/*********************************************************************
���ܣ������ļ������ݼ��У������洫���ļ�����¼Ϊ��ǰ�ļ���
������filename���ļ���
���أ�>0���ɹ�����¼����<0��ʧ��
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::load(const char * filename)
{
	if (filename == NULL)	return -1;

	m_pCritical->enter(); //���ٽ���

	int ret1 = 0;
	DWORD count = 0;
	DWORD readlen = 0;
	int len = 0;

	//�����ļ���
	saveFileName(filename);

	//���ļ�
	int res = -1;
	FILE * f = fopen(filename,"rb");
	if (f == NULL)	goto LAB_END_ERR_LOADFILE;

	//���ļ�ͷ
    res = -2;
	TFileHeaderFixedLenRecord fileHeader;
	ret1 = readFileHeader(&fileHeader,f);
	if (ret1 <= 0)	goto LAB_END_ERR_LOADFILE;

	//���ļ���
    res = -3;
	count   = (fileHeader.recoredCount < m_maxRecCount) ? fileHeader.recoredCount : m_maxRecCount; //�ɶ�ȡ��¼��
	fileHeader.recoredCount = count;
	readlen = fileHeader.perRecLen * count;
	
	if (m_pBody != NULL)	delete [] m_pBody;
	m_recCountForBodyMem = (count + m_reservedRecCountForBody < m_maxRecCount) ? count + m_reservedRecCountForBody : m_maxRecCount; //ʵ��Ϊ�ļ�body�������ڴ�
	m_pBody = NULL;
	m_pBody = new BYTE [m_recCountForBodyMem * fileHeader.perRecLen];

	len = fread(m_pBody,1,readlen,f);

	if (len <readlen)	goto LAB_END_ERR_READBODY;

	//�ر��ļ�
	fclose(f);

	//�����ļ�ͷ
	memcpy(&m_header,&fileHeader,sizeof(TFileHeaderFixedLenRecord));

	//������ָ���ظ�ֵ
	resetRecList();

	res = fileHeader.recoredCount;
	goto LAB_END;

LAB_END_ERR_READBODY:                                                //�����ļ���ռ�󣬶�ȡ����
	delete [] m_pBody;
	m_pBody = NULL;

LAB_END_ERR_LOADFILE:                                                //δ�ܳɹ������ļ�
	if (f != NULL)	fclose(f);                                       //�ر��ļ�
	//����ռ䣨ֻ������СԤ���ڴ棩
	if (m_pBody == NULL)
	{
		initEmpFileHeader();
		m_recCountForBodyMem = m_reservedRecCountForBody;
		if (m_recCountForBodyMem>0)
		{
			
			m_pBody = new BYTE [m_recCountForBodyMem * m_header.perRecLen];
			WLOutDebug(">> m_pBody = %.8X,len = %d",m_pBody,m_recCountForBodyMem * m_header.perRecLen);
		}
	}

LAB_END:                                                             //���
	m_pCritical->leave(); //���ٽ���

	return res;
}

/*********************************************************************
���ܣ��������ݼ����ļ���
������filename���ļ���
���أ�>0���ɹ���<0��ʧ��
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::saveas(const char * filename)
{
	m_pCritical->enter();					//���ٽ���

	int len = 0;
	DWORD writelen = 0;
	//���ļ�
	int ret = -1;
	FILE * f = fopen(filename,"wb");
	if (f == NULL) goto LAB_END;

	//д�ļ�ͷ
	ret--;
	len = fwrite(&m_header,sizeof(TFileHeaderFixedLenRecord),1,f);
	if (len <0)	goto LAB_END;

	//д�ļ���
	ret--;
	//WLOutDebug("begin write fileBody");
	writelen = m_header.perRecLen * m_header.recoredCount;
	len = fwrite(m_pBody,1,writelen,f);
	if (len <writelen)	goto LAB_END;
	//WLOutDebug("end write fileBody writeLen=%d",writelen);

	ret = 1;
	
LAB_END:  

	if (f != NULL)	fclose(f);                //�ر��ļ�
	m_pCritical->leave();                     //���ٽ���
	return ret;
}

/*********************************************************************
���ܣ����������ļ������ݼ��С�
��������
���أ�>0���ɹ�����¼����<0��ʧ��
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::reload()
{
	return load(m_filename);
}

/*********************************************************************
���ܣ��������ݼ����ļ���
��������
���أ�>0���ɹ���<0��ʧ��
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::save()
{
	//WLOutDebug("m_filename=%s",m_filename);
	return saveas(m_filename);
}

/*********************************************************************
���ܣ�ȡ���ݼ���¼����
��������
���أ�>=0���ɹ���Ϊ��¼����<0��ʧ��
���ߣ�Ҷ����	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::getRecordCount()
{
	DWORD count = 0;

	m_pCritical->enter(); //���ٽ���
	count = m_header.recoredCount;
	m_pCritical->leave(); //���ٽ���
	
	return count;
}

/*********************************************************************
���ܣ���ռ�¼
��������
���أ���
���ߣ�Ҷ����	2013-08-19
*********************************************************************/
void CFixedLenRecordFile::clear()
{
	m_pCritical->enter(); //���ٽ���

	if (m_pBody != NULL)	
	{
		delete [] m_pBody;
		m_pBody = NULL;
	}
	m_header.recoredCount = 0;
	m_recCountForBodyMem = 0;

	m_pCritical->leave(); //���ٽ���
}

/*********************************************************************
���ܣ��������ļ��������ʵ������ m_filename ��
������filename,�ļ�����
���أ���
���ߣ�Ҷ����	2012-08-22
ע�⣺����Զ���������reload�����е��ñ�����������fileName����m_filename�����Բ���ֱ�ӽ�m_filename���
*********************************************************************/
void CFixedLenRecordFile::saveFileName(const char * fileName)
{
	char temp[MAX_PATH] = {0};
	strcpy(temp,fileName);
	memset(m_filename,0,sizeof(char) * MAX_PATH);
    sprintf(m_filename,"%s",temp);
}

/*********************************************************************
���ܣ��������ռ�洢����,��������ӵĶ���
���أ�0�ɹ���<0ʧ�ܣ�>0ʵ�ʷ�������
���ߣ���С��	2012-08-24 �ܼ���
*********************************************************************/
int CFixedLenRecordFile::getMoreMemory()
{
	if (m_recCountForBodyMem >= m_maxRecCount) return -1;		// ��ǰ�������ڴ����,���������·���

	if (m_reservedRecCountForBody <= 0)
		m_recCountForBodyMem = ((m_recCountForBodyMem + 1) < m_maxRecCount) ? m_recCountForBodyMem + 1: m_maxRecCount; //ʵ��Ϊ�ļ�body�������ڴ�
	else
		m_recCountForBodyMem = ((m_recCountForBodyMem + m_reservedRecCountForBody) < m_maxRecCount) ? m_recCountForBodyMem + m_reservedRecCountForBody: m_maxRecCount; //ʵ��Ϊ�ļ�body�������ڴ�

	//�·���ռ�
	BYTE * pNewMem = NULL;
	pNewMem = new BYTE [m_recCountForBodyMem * m_header.perRecLen];
	if (NULL == pNewMem)  return -1;                   //�ڴ�����ʧ��

	//�������ͷž��ڴ�
	memcpy(pNewMem, m_pBody, m_header.recoredCount*m_header.perRecLen);
	delete m_pBody;
	m_pBody = pNewMem;

	//������ָ���ظ�ֵ
	resetRecList();

	return m_recCountForBodyMem;
}

int CFixedLenRecordFile::resetRecList()
{
    return m_header.recoredCount;
}

/*********************************************************************
���ܣ������ļ�����ʱ������ÿ����¼�ĳ���
��������
���أ���
���ߣ���Զ��
ע�⣺����������������ʵ�֡����������������ڼ����ļ�ʱ��������֤�ļ��ĺϷ���
*********************************************************************/
void CFixedLenRecordFile::initPerRecLen()
{
    m_perRecLen = 0;
}

/*********************************************************************
���ܣ����ļ�ͷ����
������pHeader,�������������ļ�ͷ���ݣ�f���Ѵ򿪵��ļ�
���أ�<0,ʧ�ܣ�>0���ɹ�
���ߣ�Ҷ����	2012-08-22
�޸ģ���Զ�� ���ӺϷ��Լ��
*********************************************************************/
int CFixedLenRecordFile::readFileHeader(TFileHeaderFixedLenRecord * pHeader,FILE * f)
{
    int ret = -1;
    if (pHeader == NULL)	return ret;

	//�����ļ�ͷ
    ret--;
	memset(pHeader,0,sizeof(TFileHeaderFixedLenRecord));
	int len = fread(pHeader,sizeof(TFileHeaderFixedLenRecord),1,f);	
	if (len <= 0)	
	{
		fclose(f);
        return ret;
    }

	//�ж��ļ�ͷ�ĺϷ���
    initPerRecLen();//������븲�Ǹú���
    if(m_perRecLen != 0)//���ںܶ���ʹ�øù����⺯�����þ���Ϊ�˼��������˵ĳ���
    {
        ret--;
        if(pHeader->perRecLen != m_perRecLen)
        {
            fclose(f);
            return ret;
        }
    }

	return 1;
}

void CFixedLenRecordFile::initEmpFileHeader()
{
	memset(&m_header,0,sizeof(TFileHeaderFixedLenRecord));
	m_header.createrID = m_editerID;
	m_header.updaterID = m_editerID;

	//ȡʱ��
	m_header.recoredCount = 0;
}

/*********************************************************************
���ܣ��麯������ӡ�����ļ�����
      �ú�����Ҫ��������������࣬�ɵ�����ͨ����������Ԥ��ֵ
������count������
���أ���������ǰԤ����¼��
���ߣ���С��	2013-08-24
*********************************************************************/
int CFixedLenRecordFile::setReservedRecCountForBody(int count)
{
	int countTemp = m_reservedRecCountForBody;
	m_reservedRecCountForBody = count;
	return countTemp;
}

/*********************************************************************
���ܣ��麯������ӡ�����ļ�����
��������
���أ���
���ߣ���Զ��	2013-08-23
*********************************************************************/
void CFixedLenRecordFile::dump()
{
	dumpHeader();
	//��ӡ�ļ��壬������ʵ��
}

/*********************************************************************
���ܣ��麯������ӡ�ļ�ͷ����
��������
���أ���
���ߣ���Զ��	2013-08-23
*********************************************************************/
void CFixedLenRecordFile::dumpHeader()
{
	//��ӡ�ļ�ͷ
	WLOutDebug("��ӡ�ļ�ͷ");
	char strHeader[1000] = {0};
	sprintf(strHeader,"�汾��=%.4X , ��¼����=%4d , ������¼����=%d\n�����߱�ʶ=%d\n�������ڣ�%.4d��%.2d��%.2d�� %.2d:%.2d:%.2d \n�޸��߱�ʶ=%4d\n����޸����ڣ�%.4d��%.2d��%.2d�� %.2d:%.2d:%.2d \n",
		m_header.version,m_header.recoredCount,m_header.perRecLen,m_header.createrID,
		m_header.fileCreatTime.year,m_header.fileCreatTime.month,m_header.fileCreatTime.day,m_header.fileCreatTime.hour,m_header.fileCreatTime.minute,m_header.fileCreatTime.second,
		m_header.updaterID,
		m_header.fileUpdateTime.year,m_header.fileUpdateTime.month,m_header.fileUpdateTime.day,m_header.fileUpdateTime.hour,m_header.fileUpdateTime.minute,m_header.fileUpdateTime.second
		);
	WLOutDebug(strHeader);
}
