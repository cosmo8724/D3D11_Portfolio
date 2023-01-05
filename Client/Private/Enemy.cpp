#include "stdafx.h"
#include "..\Public\Enemy.h"

CEnemy::CEnemy(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEnemy::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	return S_OK;
}

void CEnemy::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CEnemy::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CEnemy::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CEnemy::Free()
{
	__super::Free();
}
