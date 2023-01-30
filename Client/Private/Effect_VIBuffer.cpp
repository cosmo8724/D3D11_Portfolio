#include "stdafx.h"
#include "..\Public\Effect_VIBuffer.h"
#include "GameInstance.h"

CEffect_VIBuffer::CEffect_VIBuffer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_VIBuffer::CEffect_VIBuffer(const CEffect_VIBuffer & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_VIBuffer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_VIBuffer::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	return S_OK;
}

void CEffect_VIBuffer::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CEffect_VIBuffer::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CEffect_VIBuffer::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CEffect_VIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
