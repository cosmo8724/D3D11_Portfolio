#pragma once
#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT				Initialize(const wstring& wstrFontFilePath);
	HRESULT				Render(const wstring& wstrText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor);

private:
	DEVICE				m_pDevice = nullptr;
	DEVICE_CONTEXT	m_pContext = nullptr;

private:
	SpriteFont*			m_pFont = nullptr;
	SpriteBatch*			m_pBatch = nullptr;

public:
	static CCustomFont*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFontFilePath);
	virtual void			Free() override;
};

END