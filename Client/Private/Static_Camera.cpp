#include "stdafx.h"
#include "..\Public\Static_Camera.h"
#include "GameInstance.h"
#include "Bone.h"
#include "GameUtility.h"

CStatic_Camera::CStatic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CCamera(pDevice, pContext)
{
}

CStatic_Camera::CStatic_Camera(const CStatic_Camera & rhs)
	: CCamera(rhs)
{
}

void CStatic_Camera::Init_Position()
{
	_vector	vTargetPos = m_pOwnerTransform->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);

	vTargetPos = XMVectorSet(XMVectorGetX(vTargetPos), XMVectorGetY(vTargetPos) + 2.f, XMVectorGetZ(vTargetPos), 1.f);

	m_CameraDesc.vEye = vTargetPos + XMVector3Normalize(vTargetLook) * -m_fDistanceFromTarget;
	m_CameraDesc.vAt = vTargetPos;

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, m_CameraDesc.vEye);
	m_pTransformCom->LookAt(m_CameraDesc.vAt);
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

	if (CGameInstance::GetInstance()->Key_Down(DIK_F1))
		m_bMouseFix = !m_bMouseFix;

	_vector	vTargetPos = m_pOwnerTransform->Get_State(CTransform::STATE_TRANS);
	vTargetPos = XMVectorSet(XMVectorGetX(vTargetPos), XMVectorGetY(vTargetPos) + 2.f, XMVectorGetZ(vTargetPos), 1.f);

	_vector	vEye = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	m_MouseMoveX = 0;
	m_MouseMoveY = 0;
	_float		fVerticalAngle = 0.f;

	fVerticalAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVectorSet(0.f, 1.f, 0.f, 0.f))));

	if (m_MouseMoveX = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_X))
		m_pTransformCom->Orbit(vTargetPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fDistanceFromTarget, dTimeDelta * m_MouseMoveX * 0.1f);
	if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		_bool	bPossible = true;

		if (fVerticalAngle < XMConvertToRadians(10.f) && m_MouseMoveY < 0)
			bPossible = false;
		if (fVerticalAngle > XMConvertToRadians(170.f) && m_MouseMoveY > 0)
			bPossible = false;

		if (bPossible == true)
			m_pTransformCom->Orbit(vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fDistanceFromTarget, dTimeDelta * m_MouseMoveY * 0.1f);
	}
	if (m_MouseMoveX == 0 && m_MouseMoveY == 0)
		m_pTransformCom->Orbit(vTargetPos, XMVectorSet(0.f, 0.f, 0.f, 0.f), m_fDistanceFromTarget, 0.0);

	__super::Tick(dTimeDelta);
}

void CStatic_Camera::Late_Tick(_double dTimeDelta)
{
	if (!m_bRender)
		return;

	if (m_bMouseFix)
	{
		POINT		pt{ (LONG)g_iWinSizeX >> 1, (LONG)g_iWinSizeY >> 1 };

		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
	}

	__super::Late_Tick(dTimeDelta);
}

HRESULT CStatic_Camera::Render()
{
	if (!m_bRender)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
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
