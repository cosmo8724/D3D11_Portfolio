#include "stdafx.h"
#include "..\Public\LightMgr.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}

const LIGHTDESC * CLightMgr::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_vecLight.size())
		return nullptr;

	return m_vecLight[iIndex]->Get_LightDesc();
}

HRESULT CLightMgr::Add_Light(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);

	NULL_CHECK_RETURN(pLight, E_FAIL);

	m_vecLight.push_back(pLight);

	return S_OK;
}

void CLightMgr::Render_Light(CVIBuffer_Rect * pVIBufferCom, CShader * pShaderCom)
{
	for (auto& pLight : m_vecLight)
	{
		if (nullptr != pLight)
			pLight->Render(pVIBufferCom, pShaderCom);
	}
}

void CLightMgr::Clear()
{
	for (auto& pLight : m_vecLight)
		Safe_Release(pLight);

	m_vecLight.clear();
}

void CLightMgr::Free()
{
	for (auto& pLight : m_vecLight)
		Safe_Release(pLight);

	m_vecLight.clear();
}
