#include "stdafx.h"
#include "..\Public\FontMgr.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{
}

HRESULT CFontMgr::Add_Font(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrFontTag, const wstring & wstrFontFilePath)
{
	if (Find_Font(wstrFontTag) != nullptr)
		return E_FAIL;

	CCustomFont*	pFont = CCustomFont::Create(pDevice, pContext, wstrFontFilePath);
	NULL_CHECK_RETURN(pFont, E_FAIL);

	m_mapFont.emplace(wstrFontTag, pFont);

	return S_OK;
}

HRESULT CFontMgr::Render_Font(const wstring & wstrFontTag, const wstring & wstrText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	CCustomFont*	pFont = Find_Font(wstrFontTag);
	NULL_CHECK_RETURN(pFont, E_FAIL);

	return pFont->Render(wstrText, vPos, fRadian, vScale, vColor);
}

CCustomFont * CFontMgr::Find_Font(const wstring & wstrFontTag)
{
	auto	iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(wstrFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontMgr::Free()
{
	for (auto& Pair : m_mapFont)
		Safe_Release(Pair.second);
	m_mapFont.clear();
}
