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

_float4 & CLightMgr::Get_LightDiffuse(_uint iIndex)
{
	if (iIndex >= m_vecLight.size())
		return _float4(0.f, 0.f, 0.f, 0.f);

	return m_vecLight[iIndex]->Get_Diffuse();
}

void CLightMgr::Set_LightState(_uint iIndex, _bool bState)
{
	if (iIndex >= m_vecLight.size())
		return;

	m_vecLight[iIndex]->Set_LightState(bState);
}

void CLightMgr::Set_LightPosition(_uint iIndex, _float4 vPos)
{
	if (iIndex >= m_vecLight.size())
		return;

	m_vecLight[iIndex]->Set_LightPosition(vPos);
}

void CLightMgr::Set_LightRange(_uint iIndex, _float fRange)
{
	if (iIndex >= m_vecLight.size())
		return;

	m_vecLight[iIndex]->Set_LightRange(fRange);
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
		if (nullptr != pLight && pLight	->Get_LightDesc()->bIsLightOn == true)
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
