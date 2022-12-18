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

void CWeapon::Set_SocketMatrix(_float4x4 matSocket)
{
	m_matSocket = m_tWeaponDesc.pSocket->Get_CombindMatrix()
		* XMLoadFloat4x4(&m_tWeaponDesc.matSocketPivot)
		* XMLoadFloat4x4(&matSocket);

	_float3	fScale;
	fScale.x = XMVectorGetX(XMVector3Length(XMVectorSet(m_matPivot._11, m_matPivot._12, m_matPivot._13, 0.f)));
	fScale.y = XMVectorGetX(XMVector3Length(XMVectorSet(m_matPivot._21, m_matPivot._22, m_matPivot._23, 0.f)));
	fScale.z = XMVectorGetX(XMVector3Length(XMVectorSet(m_matPivot._31, m_matPivot._32, m_matPivot._33, 0.f)));

	m_matSocket = XMMatrixScaling(fScale.x, fScale.y, fScale.z) * XMLoadFloat4x4(&m_matSocket);
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
