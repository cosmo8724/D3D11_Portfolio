#include "..\Public\Component.h"

CComponent::CComponent(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_bIsCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bIsCloned(true)
	, m_wstrFilePath(rhs.m_wstrFilePath)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
