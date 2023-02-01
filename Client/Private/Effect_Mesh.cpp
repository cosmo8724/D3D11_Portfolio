#include "stdafx.h"
#include "..\Public\Effect_Mesh.h"
#include "GameInstance.h"

CEffect_Mesh::CEffect_Mesh(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Mesh::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	return S_OK;
}

void CEffect_Mesh::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CEffect_Mesh::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CEffect_Mesh::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CEffect_Mesh::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
