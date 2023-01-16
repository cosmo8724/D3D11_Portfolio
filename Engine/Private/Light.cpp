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
	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(pShaderCom->Set_RawValue(L"g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)), E_FAIL);

	pShaderCom->Begin(1);
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
