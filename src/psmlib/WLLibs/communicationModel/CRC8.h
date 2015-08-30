// CRC8.h: interface for the CCRC8 class.
// CRC8 �����װģ��
// Ҷ����	2012-02-20
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRC8_H__9E15B18B_794C_440D_894E_DEE89E467E04__INCLUDED_)
#define AFX_CRC8_H__9E15B18B_794C_440D_894E_DEE89E467E04__INCLUDED_

#include "../platformLayer/WLDEF.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BYTE CalcCRC8(const BYTE* pData, int iLength);

WORD CRC16(BYTE *pBuffer, int length);

WORD CalcCRC16_wfs(const BYTE* pData, int iLengthh);

#endif // !defined(AFX_CRC8_H__9E15B18B_794C_440D_894E_DEE89E467E04__INCLUDED_)
