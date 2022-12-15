#include "stdafx.h"
#include "..\Public\Static_Camera.h"
#include "GameInstance.h"
#include "Bone.h"

CStatic_Camera::CStatic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CCamera(pDevice, pContext)
{
}

CStatic_Camera::CStatic_Camera(const CStatic_Camera & rhs)
	: CCamera(rhs)
{
}

HRESULT CStatic_Camera::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CStatic_Camera::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_bRender = true;

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 20.f, -30.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.TransformDesc.dSpeedPerSec = 15.0;
		CameraDesc.TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &CameraDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CStatic_Camera::Tick(_double dTimeDelta)
{
	if (!m_bRender)
		return;

	__super::Tick(dTimeDelta);
}

void CStatic_Camera::Late_Tick(_double dTimeDelta)
{
	if (!m_bRender)
		return;
	
	__super::Late_Tick(dTimeDelta);
}

HRESULT CStatic_Camera::Render()
{
	if (!m_bRender)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CStatic_Camera::Pitch(_float dTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);

	_matrix matRotation = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (_float)m_CameraDesc.TransformDesc.dRotationPerSec * (_float)dTimeDelta);
	XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_UP), matRotation);
	XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), matRotation);

	//_vector	vPlayerLookAt = m_pTransformCom->Get_State(CTransform::STATE_TRANS) + XMVector3Normalize(matOwnerWorld.r[2]);
	//_vector	vCamEye = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()).r[3];

	//m_vDir = vPlayerLookAt - vCamEye;
}

void CStatic_Camera::Yaw(_float dTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);
}

void CStatic_Camera::Roll(_float dTimeDelta)
{
	XMMatrixRotationRollPitchYaw(0.f, 0.f, _float(m_CameraDesc.TransformDesc.dRotationPerSec * dTimeDelta));
}

HRESULT CStatic_Camera::SetUp_Component()
{
	return S_OK;
}

CStatic_Camera * CStatic_Camera::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CStatic_Camera*	pInstance = new CStatic_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CStatic_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatic_Camera::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CStatic_Camera*	pInstance = new CStatic_Camera(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CStatic_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatic_Camera::Free()
{
	__super::Free();
}
