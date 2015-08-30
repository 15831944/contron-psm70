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
功能：构造函数，作初始化工作
参数：无
返回：无
作者：叶文林	2012-08-22
*********************************************************************/
CFixedLenRecordFile::CFixedLenRecordFile(DWORD editerID)
{
	m_pCritical = new CWLCriticalSection();              //初始化临界区

	memset(m_filename,0,sizeof(char)*MAX_PATH);          //存储文件名称
	memset(&m_header,0,sizeof(TFileHeaderFixedLenRecord));     //文件头
	m_pBody       = NULL;                                //文件体
	m_maxRecCount = NULL;                                //最大记录数（子类根据情况构造时赋初值）

	m_editerID = editerID;

	m_reservedRecCountForBody = 0;                       //load成功后应预留记录数（多分配空间）（子类根据情况构造时赋初值）
	m_recCountForBodyMem      = 0;                       //数据区分配内存数
    m_perRecLen = 0;                                     //每条记录的长度,用于文件合法性检查
}

/*********************************************************************
功能：析构函数
参数：无
返回：无
作者：叶文林	2012-08-22
*********************************************************************/
CFixedLenRecordFile::~CFixedLenRecordFile()
{
	//释放文件体内存
	if (m_pBody != NULL)	delete [] m_pBody;

	//释放临界区
	delete m_pCritical;
}

/*********************************************************************
功能：载入文件到数据集中，并保存传入文件名记录为当前文件名
参数：filename，文件名
返回：>0，成功，记录数；<0，失败
作者：叶文林	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::load(const char * filename)
{
	if (filename == NULL)	return -1;

	m_pCritical->enter(); //进临界区

	int ret1 = 0;
	DWORD count = 0;
	DWORD readlen = 0;
	int len = 0;

	//保存文件名
	saveFileName(filename);

	//打开文件
	int res = -1;
	FILE * f = fopen(filename,"rb");
	if (f == NULL)	goto LAB_END_ERR_LOADFILE;

	//读文件头
    res = -2;
	TFileHeaderFixedLenRecord fileHeader;
	ret1 = readFileHeader(&fileHeader,f);
	if (ret1 <= 0)	goto LAB_END_ERR_LOADFILE;

	//读文件体
    res = -3;
	count   = (fileHeader.recoredCount < m_maxRecCount) ? fileHeader.recoredCount : m_maxRecCount; //可读取记录数
	fileHeader.recoredCount = count;
	readlen = fileHeader.perRecLen * count;
	
	if (m_pBody != NULL)	delete [] m_pBody;
	m_recCountForBodyMem = (count + m_reservedRecCountForBody < m_maxRecCount) ? count + m_reservedRecCountForBody : m_maxRecCount; //实际为文件body区分配内存
	m_pBody = NULL;
	m_pBody = new BYTE [m_recCountForBodyMem * fileHeader.perRecLen];

	len = fread(m_pBody,1,readlen,f);

	if (len <readlen)	goto LAB_END_ERR_READBODY;

	//关闭文件
	fclose(f);

	//保存文件头
	memcpy(&m_header,&fileHeader,sizeof(TFileHeaderFixedLenRecord));

	//给索引指针重赋值
	resetRecList();

	res = fileHeader.recoredCount;
	goto LAB_END;

LAB_END_ERR_READBODY:                                                //分配文件体空间后，读取出错
	delete [] m_pBody;
	m_pBody = NULL;

LAB_END_ERR_LOADFILE:                                                //未能成功载入文件
	if (f != NULL)	fclose(f);                                       //关闭文件
	//分配空间（只分配最小预留内存）
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

LAB_END:                                                             //完成
	m_pCritical->leave(); //出临界区

	return res;
}

/*********************************************************************
功能：保存数据集到文件。
参数：filename，文件名
返回：>0，成功；<0，失败
作者：叶文林	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::saveas(const char * filename)
{
	m_pCritical->enter();					//进临界区

	int len = 0;
	DWORD writelen = 0;
	//打开文件
	int ret = -1;
	FILE * f = fopen(filename,"wb");
	if (f == NULL) goto LAB_END;

	//写文件头
	ret--;
	len = fwrite(&m_header,sizeof(TFileHeaderFixedLenRecord),1,f);
	if (len <0)	goto LAB_END;

	//写文件体
	ret--;
	//WLOutDebug("begin write fileBody");
	writelen = m_header.perRecLen * m_header.recoredCount;
	len = fwrite(m_pBody,1,writelen,f);
	if (len <writelen)	goto LAB_END;
	//WLOutDebug("end write fileBody writeLen=%d",writelen);

	ret = 1;
	
LAB_END:  

	if (f != NULL)	fclose(f);                //关闭文件
	m_pCritical->leave();                     //出临界区
	return ret;
}

/*********************************************************************
功能：重新载入文件到数据集中。
参数：无
返回：>0，成功，记录数；<0，失败
作者：叶文林	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::reload()
{
	return load(m_filename);
}

/*********************************************************************
功能：保存数据集到文件。
参数：无
返回：>0，成功；<0，失败
作者：叶文林	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::save()
{
	//WLOutDebug("m_filename=%s",m_filename);
	return saveas(m_filename);
}

/*********************************************************************
功能：取数据集记录数。
参数：无
返回：>=0，成功，为记录数；<0，失败
作者：叶文林	2012-08-22
*********************************************************************/
int CFixedLenRecordFile::getRecordCount()
{
	DWORD count = 0;

	m_pCritical->enter(); //进临界区
	count = m_header.recoredCount;
	m_pCritical->leave(); //出临界区
	
	return count;
}

/*********************************************************************
功能：请空记录
参数：无
返回：无
作者：叶文林	2013-08-19
*********************************************************************/
void CFixedLenRecordFile::clear()
{
	m_pCritical->enter(); //进临界区

	if (m_pBody != NULL)	
	{
		delete [] m_pBody;
		m_pBody = NULL;
	}
	m_header.recoredCount = 0;
	m_recCountForBodyMem = 0;

	m_pCritical->leave(); //出临界区
}

/*********************************************************************
功能：存名称文件到对象的实例变量 m_filename 中
参数：filename,文件名；
返回：无
作者：叶文林	2012-08-22
注意：（刘远安）如果在reload函数中调用本函数，参数fileName就是m_filename，所以不能直接将m_filename清空
*********************************************************************/
void CFixedLenRecordFile::saveFileName(const char * fileName)
{
	char temp[MAX_PATH] = {0};
	strcpy(temp,fileName);
	memset(m_filename,0,sizeof(char) * MAX_PATH);
    sprintf(m_filename,"%s",temp);
}

/*********************************************************************
功能：申请更大空间存储数据,针对有增加的对象
返回：0成功，<0失败，>0实际分配条数
作者：唐小灿	2012-08-24 佘佳明
*********************************************************************/
int CFixedLenRecordFile::getMoreMemory()
{
	if (m_recCountForBodyMem >= m_maxRecCount) return -1;		// 当前已最大的内存分配,不能再重新分配

	if (m_reservedRecCountForBody <= 0)
		m_recCountForBodyMem = ((m_recCountForBodyMem + 1) < m_maxRecCount) ? m_recCountForBodyMem + 1: m_maxRecCount; //实际为文件body区分配内存
	else
		m_recCountForBodyMem = ((m_recCountForBodyMem + m_reservedRecCountForBody) < m_maxRecCount) ? m_recCountForBodyMem + m_reservedRecCountForBody: m_maxRecCount; //实际为文件body区分配内存

	//新分配空间
	BYTE * pNewMem = NULL;
	pNewMem = new BYTE [m_recCountForBodyMem * m_header.perRecLen];
	if (NULL == pNewMem)  return -1;                   //内存申请失败

	//拷贝并释放旧内存
	memcpy(pNewMem, m_pBody, m_header.recoredCount*m_header.perRecLen);
	delete m_pBody;
	m_pBody = pNewMem;

	//给索引指针重赋值
	resetRecList();

	return m_recCountForBodyMem;
}

int CFixedLenRecordFile::resetRecList()
{
    return m_header.recoredCount;
}

/*********************************************************************
功能：加载文件内容时，设置每条记录的长度
参数：无
返回：无
作者：刘远安
注意：本函数必须由子类实现。本函数的作用是在加载文件时，用于验证文件的合法性
*********************************************************************/
void CFixedLenRecordFile::initPerRecLen()
{
    m_perRecLen = 0;
}

/*********************************************************************
功能：对文件头数据
参数：pHeader,输出参数，存放文件头数据；f，已打开的文件
返回：<0,失败；>0，成功
作者：叶文林	2012-08-22
修改：刘远安 增加合法性检查
*********************************************************************/
int CFixedLenRecordFile::readFileHeader(TFileHeaderFixedLenRecord * pHeader,FILE * f)
{
    int ret = -1;
    if (pHeader == NULL)	return ret;

	//载入文件头
    ret--;
	memset(pHeader,0,sizeof(TFileHeaderFixedLenRecord));
	int len = fread(pHeader,sizeof(TFileHeaderFixedLenRecord),1,f);	
	if (len <= 0)	
	{
		fclose(f);
        return ret;
    }

	//判断文件头的合法性
    initPerRecLen();//子类必须覆盖该函数
    if(m_perRecLen != 0)//由于很多人使用该公共库函数，该句是为了兼容其他人的程序
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

	//取时间
	m_header.recoredCount = 0;
}

/*********************************************************************
功能：虚函数：打印整个文件内容
      该函数主要针对有增操作的类，由调用者通过子类设置预留值
参数：count：设置
返回：返回设置前预留记录数
作者：唐小灿	2013-08-24
*********************************************************************/
int CFixedLenRecordFile::setReservedRecCountForBody(int count)
{
	int countTemp = m_reservedRecCountForBody;
	m_reservedRecCountForBody = count;
	return countTemp;
}

/*********************************************************************
功能：虚函数：打印整个文件内容
参数：无
返回：无
作者：刘远安	2013-08-23
*********************************************************************/
void CFixedLenRecordFile::dump()
{
	dumpHeader();
	//打印文件体，在子类实现
}

/*********************************************************************
功能：虚函数：打印文件头内容
参数：无
返回：无
作者：刘远安	2013-08-23
*********************************************************************/
void CFixedLenRecordFile::dumpHeader()
{
	//打印文件头
	WLOutDebug("打印文件头");
	char strHeader[1000] = {0};
	sprintf(strHeader,"版本号=%.4X , 记录数量=%4d , 单条记录长度=%d\n创建者标识=%d\n创建日期：%.4d年%.2d月%.2d日 %.2d:%.2d:%.2d \n修改者标识=%4d\n最后修改日期：%.4d年%.2d月%.2d日 %.2d:%.2d:%.2d \n",
		m_header.version,m_header.recoredCount,m_header.perRecLen,m_header.createrID,
		m_header.fileCreatTime.year,m_header.fileCreatTime.month,m_header.fileCreatTime.day,m_header.fileCreatTime.hour,m_header.fileCreatTime.minute,m_header.fileCreatTime.second,
		m_header.updaterID,
		m_header.fileUpdateTime.year,m_header.fileUpdateTime.month,m_header.fileUpdateTime.day,m_header.fileUpdateTime.hour,m_header.fileUpdateTime.minute,m_header.fileUpdateTime.second
		);
	WLOutDebug(strHeader);
}
