#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "Player.h"
#include "Transform.h"
#include "Bone.h"

CWeapon::CWeapon(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CWeapon::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	ZeroMemory(&m_tWeaponDesc, sizeof(WEAPONDESC));

	if (pArg != nullptr)
		memcpy(&m_tWeaponDesc, pArg, sizeof(WEAPONDESC));

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &m_tWeaponDesc), E_FAIL);

	return S_OK;
}

void CWeapon::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CWeapon::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CWeapon::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CWeapon::Free()
{
	__super::Free();
}
