#include "stdafx.h"
#include "..\Public\NPC.h"


CNPC::CNPC(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPC::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CNPC::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	return S_OK;
}

void CNPC::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CNPC::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CNPC::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CNPC::Free()
{
	__super::Free();
}
