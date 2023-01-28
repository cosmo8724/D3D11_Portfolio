#include "stdafx.h"
#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const wstring & wstrFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, wstrFontFilePath.c_str());

	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

HRESULT CCustomFont::Render(const wstring & wstrText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, wstrText.c_str(), vPos, vColor, fRadian, _float2(0.f, 0.f), vScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrFontFilePath)
{
	CCustomFont*		pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(wstrFontFilePath)))
	{
		MSG_BOX("Failed to Create : CCustomFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pFont);
	Safe_Delete(m_pBatch);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
