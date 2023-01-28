#include "stdafx.h"
#include "..\Public\MonsterDrink.h"
#include "Sigrid.h"
#include "GameInstance.h"

CMonsterDrink::CMonsterDrink(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterDrink::CMonsterDrink(const CMonsterDrink & rhs)
	: CGameObject(rhs)
	, m_iValue(rhs.m_iValue)
	, m_dMoveTime(rhs.m_dMoveTime)
{
}

HRESULT CMonsterDrink::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CMonsterDrink::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CMonsterDrink::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_dMoveTime += dTimeDelta;

	if (m_dMoveTime > 2.0)
		m_dMoveTime = 0.0;

	_vector	vPlayerPos = XMLoadFloat4x4(&m_pPlayer->Get_WorldMatrix()).r[3];

	//m_pTransformCom->LookAt_NoUpDown(vPlayerPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f * (_float)m_dMoveTime));
	m_pTransformCom->RotationFromNow(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMConvertToRadians(15.f));

	if (m_dMoveTime <= 1.0)
		m_pTransformCom->Go_Up((1.0 - m_dMoveTime) / dTimeDelta * 0.000001);
	else
		m_pTransformCom->Go_Down((2.0 - m_dMoveTime) / dTimeDelta * 0.000001);

	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonsterDrink::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_pPlayer->Collision_Range(m_pSphereCol))
	{
		_vector	vTargetPos = XMLoadFloat4x4(&m_pPlayer->Get_WorldMatrix()).r[3];
		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

		_vector	vDir = vTargetPos - vPos;
		_float		fDist = XMVectorGetX(XMVector3Length(vDir));

		m_pTransformCom->Chase(vTargetPos, dTimeDelta / (_double)fDist);
	}

	if (m_pPlayer->Collision_Body(m_pSphereCol))
	{
		m_pPlayer->Get_Status().iCoin += m_iValue;
		m_bDead = true;
	}
}

HRESULT CMonsterDrink::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CMonsterDrink::SetUp_Component()
{
	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.3f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

void CMonsterDrink::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCol);
}
