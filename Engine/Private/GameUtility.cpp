#include "..\Public\GameUtility.h"

void CGameUtility::wctc(const wchar_t* szWchar, char* szChar)
{
	_int iLength;
	_int slength = lstrlen(szWchar) + 1;
	iLength = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, iLength, 0, 0);
}