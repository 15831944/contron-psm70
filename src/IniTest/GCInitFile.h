/***********************************************************************
功能：Ini文件读写类
文件格式：1.以“#”开头表示注释行
         2.数据保存格式：Key=Value
举例：
    #这是注释行
    Name=lya
    ID=123
作者：刘远安
************************************************************************/
#ifndef GCINITFILE_H
#define GCINITFILE_H

#include "sys/platform.h"
//#include "WLLibs/platformLayer/WLDEF.h"
#include <ctype.h>
#include <stdio.h>

#ifndef CCHR
#define CCHR const char
#endif

#ifndef UCHR
#define UCHR unsigned char
#endif

#ifndef UCCHR
#define UCCHR const unsigned char
#endif

#ifndef UINT
#define UINT unsigned int
#endif

//节点类型
#define typeNULL       0    //
#define typeSECTION    1    //段落
#define typeKEYVALUE   2    //键值
#define typeCOMMENT    3    //注释

//注释字符
#define COMMENT_CHAR   ('#')

struct EntryNode
{
   char   Type;
   char  *pText;
   struct EntryNode *pPrev;
   struct EntryNode *pNext;
};

typedef struct
{
   struct EntryNode *pSec;
   struct EntryNode *pKey;
   char KeyText [128];
   char ValText [128];
   char Comment [255];
} RecordItem;

/* Macros */
#define AreaPtrValid(Key,Val) ((Key!=NULL)&&(Val!=NULL))

class CGCInitFile
{
public:
    CGCInitFile();
    ~CGCInitFile();

    UINT    GetVersion  (void);
    BOOL    OpenIniFile (CCHR *pFileName);
    BOOL    ReadBool    (CCHR *pKey, BOOL   Default);
    int     ReadInt     (CCHR *pKey, int    Default);
    double  ReadDouble  (CCHR *pKey, double Default);
    CCHR    *ReadString (CCHR *pKey, CCHR  *pDefault);

    void    WriteBool   (CCHR *pKey, bool   Value);
    void    WriteInt    (CCHR *pKey, int    Value);
    void    WriteDouble (CCHR *pKey, double Value);
    void    WriteString (CCHR *pKey, CCHR  *pValue);

    void    CloseIniFile ();
    bool    WriteIniFile (CCHR *pFileName);
    bool	DeleteKey (CCHR *pSection, CCHR *pKey);

protected:
    struct  EntryNode *m_pEntry;
    struct  EntryNode *m_pCurEntry;
    char    m_result [255];
    FILE    *m_pIniFile;
    void    AddKey     (struct EntryNode *pEntry, CCHR *pKey, CCHR *pValue);
    BOOL    AddItem    (char Type, CCHR *pText);
    bool    AddItemAt (struct EntryNode *pEntryAt, char Mode, CCHR *pText);
    void    FreeMem    (void *pPtr);
    void    FreeAllMem (void);
    bool    FindKey    (CCHR *pKey, RecordItem *pList);
    bool    AddSectionAndKey (CCHR *pKey, CCHR *pValue);
    struct  EntryNode *MakeNewEntry (void);
    struct  EntryNode *FindSection (CCHR *pSection);

private:

};

#endif // GCINITFILE_H
