#include "..\Public\Level.h"

CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	m_pDevice->AddRef();
	m_pContext->AddRef();
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_double dTimeDelta)
{
}

void CLevel::Late_Tick(_double dTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}