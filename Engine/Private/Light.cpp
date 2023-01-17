#include "stdafx.h"
#include "..\Public\Light.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CLight::CLight(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBufferCom, CShader * pShaderCom)
{
	_int	iPassIndex = -1;

	if (m_LightDesc.eType == LIGHTDESC::LIGHT_DIRECTIONAL)
	{
		FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4)), E_FAIL);

		iPassIndex = 1;
	}
	else if (m_LightDesc.eType == LIGHTDESC::LIGHT_POINT)
	{
		FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4)), E_FAIL);
		FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_fLightRange", &m_LightDesc.fRange, sizeof(_float)), E_FAIL);

		iPassIndex = 2;
	}

	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)), E_FAIL);

	if (iPassIndex == -1)
		return E_FAIL;

	pShaderCom->Begin(iPassIndex);
	pVIBufferCom->Render();

	return S_OK;
}

CLight * CLight::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Faield to Create : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
