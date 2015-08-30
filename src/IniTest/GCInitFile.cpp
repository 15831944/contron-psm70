#include "GCInitFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

CGCInitFile::CGCInitFile()
{
    m_pEntry      = NULL;
    m_pCurEntry   = NULL;
    m_result [0]  = 0;
    m_pIniFile    = NULL;
}

CGCInitFile::~CGCInitFile()
{
    FreeAllMem ();
}

/*=========================================================================
   OpenIniFile
  -------------------------------------------------------------------------
   Job : Opens an ini file or creates a new one if the requested file
         doesnt exists.
/*========================================================================*/
BOOL CGCInitFile::OpenIniFile (CCHR * FileName)
{
    char   Str [255];
    char   *pStr;
    struct EntryNode *pEntry;

    FreeAllMem ();

    if (FileName == NULL)                             { return FALSE; }
    if ((m_pIniFile = fopen (FileName, "r")) == NULL) { return FALSE; }

    while (fgets (Str, 255, m_pIniFile) != NULL)
    {
        pStr = strchr (Str, '\n');
        if (pStr != NULL) { *pStr = 0; }
        pEntry = MakeNewEntry ();
        if (pEntry == NULL) { return FALSE; }

        #ifdef INI_REMOVE_CR
        int Len = strlen(Str);
        if ( Len > 0 )
        {
            if ( Str[Len-1] == '\r' )
            {
                Str[Len-1] = '\0';
            }
        }
        #endif

        pEntry->pText = (char *)malloc (strlen (Str)+1);
        if (pEntry->pText == NULL)
        {
            FreeAllMem ();
            return FALSE;
        }
        strcpy (pEntry->pText, Str);
        //注释行
//        pStr = strchr (Str,';');
        pStr = strchr (Str,COMMENT_CHAR);
        if (pStr != NULL) { *pStr = 0; } /* Cut all comments */
        // Is Section
        if ( (strstr (Str, "[") > 0) && (strstr (Str, "]") > 0) && ('[' == Str[0]) )
        {
            pEntry->Type = typeSECTION;
        }
        else
        {
            if (strstr (Str, "=") > 0)
            {
                pEntry->Type = typeKEYVALUE;
            }
            else
            {
                pEntry->Type = typeCOMMENT;
            }
        }
        m_pCurEntry = pEntry;
    }
    fclose (m_pIniFile);
    m_pIniFile = NULL;
    return TRUE;
}

/*=========================================================================
   CloseIniFile
  -------------------------------------------------------------------------
   Job : Closes the ini file without any modifications. If you want to
         write the file use WriteIniFile instead.
/*========================================================================*/
void CGCInitFile::CloseIniFile (void)
{
    FreeAllMem ();
    if (m_pIniFile != NULL)
    {
        fclose (m_pIniFile);
        m_pIniFile = NULL;
    }
}


/*=========================================================================
   WriteIniFile
  -------------------------------------------------------------------------
   Job : Writes the iniFile to the disk and close it. Frees all memory
         allocated by WriteIniFile;
/*========================================================================*/
bool CGCInitFile::WriteIniFile (const char *pFileName)
{
    struct EntryNode *pEntry = m_pEntry;
    if (m_pIniFile != NULL)
    {
        fclose (m_pIniFile);
    }
    if ((m_pIniFile = fopen (pFileName, "wb")) == NULL)
    {
        FreeAllMem ();
        return FALSE;
    }

    while (pEntry != NULL)
    {
        if (pEntry->Type != typeNULL)
        {
#			ifdef INI_REMOVE_CR
            fprintf (m_pIniFile, "%s\n", pEntry->pText);
#			else
            fprintf (m_pIniFile, "%s\r\n", pEntry->pText);
#			endif
        }
        pEntry = pEntry->pNext;
    }

    fclose (m_pIniFile);
    m_pIniFile = NULL;
    return TRUE;
}


/*=========================================================================
   WriteString : Writes a string to the ini file
*========================================================================*/
void CGCInitFile::WriteString (CCHR *pKey, CCHR *pValue)
{
    RecordItem List;
    char  Str [255];

    if (AreaPtrValid ( pKey, pValue) == FALSE) { return; }
    if (FindKey  (pKey, &List) == TRUE)
    {
        sprintf (Str, "%s=%s%s", List.KeyText, pValue, List.Comment);
        FreeMem (List.pKey->pText);
        List.pKey->pText = (char *)malloc (strlen (Str)+1);
        strcpy (List.pKey->pText, Str);
    }
    else
    {
        if ((List.pSec != NULL) && (List.pKey == NULL)) /* section exist, Key not */
        {
            AddKey (List.pSec, pKey, pValue);
        }
        else
        {
            AddSectionAndKey (pKey, pValue);
        }
    }
}

/*=========================================================================
   WriteBool : Writes a boolean to the ini file
*========================================================================*/
void CGCInitFile::WriteBool (CCHR *pKey, bool Value)
{
    char Val [2] = {'0',0};
    if (Value != 0) { Val [0] = '1'; }
    WriteString (pKey, Val);
}

/*=========================================================================
   WriteInt : Writes an integer to the ini file
*========================================================================*/
void CGCInitFile::WriteInt (CCHR *pKey, int Value)
{
    char Val [12]; /* 32bit maximum + sign + \0 */
    sprintf (Val, "%d", Value);
    WriteString (pKey, Val);
}

/*=========================================================================
   WriteDouble : Writes a double to the ini file
*========================================================================*/
void CGCInitFile::WriteDouble (CCHR *pKey, double Value)
{
    char Val [32]; /* DDDDDDDDDDDDDDD+E308\0 */
    sprintf (Val, "%1.10lE", Value);
    WriteString (pKey, Val);
}


/*=========================================================================
   ReadString : Reads a string from the ini file
*========================================================================*/
CCHR *CGCInitFile::ReadString (CCHR *pKey, CCHR *pDefault)
{
    RecordItem List;
    if (AreaPtrValid (pKey, pDefault) == FALSE) { return pDefault; }
    if (FindKey  (pKey, &List) == TRUE)
    {
        strcpy (m_result, List.ValText);
        return m_result;
    }
    return pDefault;
}

/*=========================================================================
   ReadBool : Reads a boolean from the ini file
*========================================================================*/
BOOL CGCInitFile::ReadBool (CCHR *pKey, BOOL Default)
{
    char Val [2] = {"0"};
    if (Default != 0) { Val [0] = '1'; }
    return (atoi (ReadString ( pKey, Val))?1:0); /* Only allow 0 or 1 */
}

/*=========================================================================
   ReadInt : Reads a integer from the ini file
*========================================================================*/
int CGCInitFile::ReadInt (CCHR *pKey, int Default)
{
    char Val [12];
    sprintf (Val,"%d", Default);
    return (atoi (ReadString ( pKey, Val)));
}

/*=========================================================================
   ReadDouble : Reads a double from the ini file
*========================================================================*/
double CGCInitFile::ReadDouble (CCHR *pKey, double Default)
{
    double Val;
    sprintf (m_result, "%1.10lE", Default);
    sscanf (ReadString ( pKey, m_result), "%lE", &Val);
    return Val;
}

/*=========================================================================
   DeleteKey : Deletes an entry from the ini file
*========================================================================*/
bool CGCInitFile::DeleteKey (CCHR *pSection, CCHR *pKey)
{
    RecordItem         List;
    struct EntryNode *pPrev;
    struct EntryNode *pNext;

    if (FindKey ( pKey, &List) == TRUE)
    {
        pPrev = List.pKey->pPrev;
        pNext = List.pKey->pNext;
        if (pPrev)
        {
            pPrev->pNext=pNext;
        }
        if (pNext)
        {
            pNext->pPrev=pPrev;
        }
        FreeMem (List.pKey->pText);
        FreeMem (List.pKey);
        return TRUE;
    }
    return FALSE;
}




/* Here we start with our helper functions */

void CGCInitFile::FreeMem (void *pPtr)
{
    if (pPtr != NULL) { free (pPtr); }
}

void CGCInitFile::FreeAllMem (void)
{
    struct EntryNode *pEntry;
    struct EntryNode *pNextEntry;
    pEntry = m_pEntry;
    while (1)
    {
        if (pEntry == NULL) { break; }
        pNextEntry = pEntry->pNext;
        FreeMem (pEntry->pText); /* Frees the pointer if not NULL */
        FreeMem (pEntry);
        pEntry = pNextEntry;
    }
    m_pEntry    = NULL;
    m_pCurEntry = NULL;
}

struct EntryNode *CGCInitFile::FindSection (CCHR *pSection)
{
    char Sec  [130];
    char iSec [130];
    struct EntryNode *pEntry;
    sprintf (Sec, "[%s]", pSection);
    strupr  (Sec);
    pEntry = m_pEntry; /* Get a pointer to the first Entry */
    while (pEntry != NULL)
    {
        if (pEntry->Type == typeSECTION)
        {
            strcpy  (iSec, pEntry->pText);
            strupr  (iSec);
            if (strcmp (Sec, iSec) == 0)
            {
                return pEntry;
            }
        }
        pEntry = pEntry->pNext;
    }
    return NULL;
}

bool CGCInitFile::FindKey  (CCHR *pKey, RecordItem *pList)
{
    char Search [130];
    char Found  [130];
    char Text   [255];
    char *pText;
    struct EntryNode *pEntry;
    pList->pSec        = NULL;
    pList->pKey        = NULL;
//    pEntry = FindSection (pSection);
    pEntry = m_pEntry;
    if (pEntry == NULL) { return FALSE; }
    pList->pSec        = pEntry;
    pList->KeyText[0] = 0;
    pList->ValText[0] = 0;
    pList->Comment[0] = 0;
//    pEntry = pEntry->pNext;
//    if (pEntry == NULL) { return FALSE; }
    sprintf (Search, "%s",pKey);
    strupr  (Search);
    while (pEntry != NULL)
    {
        if ((pEntry->Type == typeSECTION) || /* Stop after next section or EOF */
            (pEntry->Type == typeNULL   ))
        {
            return FALSE;
        }
        if (pEntry->Type == typeKEYVALUE)
        {
            strcpy (Text, pEntry->pText);
            pText = strchr (Text, ';');
            if (pText != NULL)
            {
                strcpy (pList->Comment, pText);
                *pText = 0;
            }
            pText = strchr (Text, '=');
            if (pText != NULL)
            {
                *pText = 0;
                strcpy (pList->KeyText, Text);
                strcpy (Found, Text);
                *pText = '=';
                strupr (Found);
                /*            printf ("%s,%s\n", Search, Found); */
                if (strcmp (Found,Search) == 0)
                {
                   strcpy (pList->ValText, pText+1);
                   pList->pKey = pEntry;
                   return TRUE;
                }
            }
        }
        pEntry = pEntry->pNext;
    }
    return NULL;
}

BOOL CGCInitFile::AddItem (char Type, CCHR *pText)
{
    struct EntryNode *pEntry = MakeNewEntry ();
    if (pEntry == NULL) { return FALSE; }
    pEntry->Type = Type;
    pEntry->pText = (char*)malloc (strlen (pText) +1);
    if (pEntry->pText == NULL)
    {
        free (pEntry);
        return FALSE;
    }
    strcpy (pEntry->pText, pText);
    pEntry->pNext   = NULL;
    if (m_pCurEntry != NULL) { m_pCurEntry->pNext = pEntry; }
    m_pCurEntry    = pEntry;
    return TRUE;
}

bool CGCInitFile::AddItemAt (struct EntryNode *pEntryAt, char Mode, CCHR *pText)
{
    struct EntryNode *pNewEntry;
    if (pEntryAt == NULL)  { return FALSE; }
    pNewEntry = (struct EntryNode*) malloc (sizeof (EntryNode));
    if (pNewEntry == NULL) { return FALSE; }
    pNewEntry->pText = (char *) malloc (strlen (pText)+1);
    if (pNewEntry->pText == NULL)
    {
        free (pNewEntry);
        return FALSE;
    }
    strcpy (pNewEntry->pText, pText);
    if (pEntryAt->pNext == NULL) /* No following nodes. */
    {
        pEntryAt->pNext   = pNewEntry;
        pNewEntry->pNext  = NULL;
    }
    else
    {
        pNewEntry->pNext = pEntryAt->pNext;
        pEntryAt->pNext  = pNewEntry;
    }
    pNewEntry->pPrev = pEntryAt;
    pNewEntry->Type  = Mode;
    return TRUE;
}

bool CGCInitFile::AddSectionAndKey (CCHR *pKey, CCHR *pValue)
{
    char Text [255];
    sprintf (Text, "%s=%s", pKey, pValue);
    return AddItem (typeKEYVALUE, Text)? 1 : 0;
}

void CGCInitFile::AddKey (struct EntryNode *pSecEntry, CCHR *pKey, CCHR *pValue)
{
    char Text [255];
    sprintf (Text, "%s=%s", pKey, pValue);
    AddItemAt (pSecEntry, typeKEYVALUE, Text);
}

struct EntryNode *CGCInitFile::MakeNewEntry (void)
{
    struct EntryNode *pEntry;
    pEntry = (struct EntryNode *)malloc (sizeof (EntryNode));
    if (pEntry == NULL)
    {
        FreeAllMem ();
        return NULL;
    }
    if (m_pEntry == NULL)
    {
        m_pEntry = pEntry;
    }
    pEntry->Type  = typeNULL;
    pEntry->pPrev = m_pCurEntry;
    pEntry->pNext = NULL;
    pEntry->pText = NULL;
    if (m_pCurEntry != NULL)
    {
        m_pCurEntry->pNext = pEntry;
    }
    return pEntry;
}

#ifdef INIFILE_TEST_THIS_FILE
int main (void)
{
    CGCInitFile iFile;
    printf ("Hello World\n");
    iFile.OpenIniFile  ("Test.Ini");
    iFile.WriteString  ("Test", "Name", "Value");
    iFile.WriteString  ("Test", "Name", "OverWrittenValue");
    iFile.WriteString  ("Test", "Port", "COM1");
    iFile.WriteString  ("Test", "User", "James Brown jr.");
    iFile.WriteString  ("Configuration", "eDriver", "MBM2.VXD");
    iFile.WriteString  ("Configuration", "Wrap", "LPT.VXD");
    iFile.WriteInt     ("IO-Port", "Com", 2);
    iFile.WriteBool    ("IO-Port", "IsValid", 0);
    iFile.WriteDouble  ("TheMoney", "TheMoney", 67892.00241);
    iFile.WriteInt     ("Test"    , "ToDelete", 1234);
    iFile.WriteIniFile ("Test.Ini");
    printf ("Key ToDelete created. Check ini file. Any key to continue");
//	while (!kbhit());
    iFile.OpenIniFile  ("Test.Ini");
    iFile.DeleteKey    ("Test"	  , "ToDelete");
    iFile.WriteIniFile ("Test.Ini");
    printf ("[Test] Name = %s\n", iFile.ReadString ("Test", "Name", "NotFound"));
    printf ("[Test] Port = %s\n", iFile.ReadString ("Test", "Port", "NotFound"));
    printf ("[Test] User = %s\n", iFile.ReadString ("Test", "User", "NotFound"));
    printf ("[Configuration] eDriver = %s\n", iFile.ReadString ("Configuration", "eDriver", "NotFound"));
    printf ("[Configuration] Wrap = %s\n", iFile.ReadString ("Configuration", "Wrap", "NotFound"));
    printf ("[IO-Port] Com = %d\n", iFile.ReadInt ("IO-Port", "Com", 0));
    printf ("[IO-Port] IsValid = %d\n", iFile.ReadBool ("IO-Port", "IsValid", 0));
    printf ("[TheMoney] TheMoney = %1.10lf\n", iFile.ReadDouble ("TheMoney", "TheMoney", 111));
    iFile.CloseIniFile ();
    return 0;
}
#endif
