#pragma once
#include "Base.h"

BEGIN(Engine)

class CFontMgr final : public CBase
{
	DECLARE_SINGLETON(CFontMgr)

private:
	CFontMgr();
	virtual ~CFontMgr() = default;

public:
	HRESULT					Add_Font(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFontTag, const wstring& wstrFontFilePath);
	HRESULT					Render_Font(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor);

private:
	map<const wstring, class CCustomFont*>		m_mapFont;

private:
	class CCustomFont*		Find_Font(const wstring& wstrFontTag);

public:
	virtual void				Free() override;
};

END