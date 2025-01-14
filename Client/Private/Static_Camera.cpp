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
		CameraDesc.fFov = XMConvertToRadians(75.f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 3000.f;
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
	
	m_CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;

	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);

	if (CGameInstance::GetInstance()->Key_Down(DIK_F1))
		m_bMouseFix = !m_bMouseFix;

	if (m_bShopOpen != g_bReadySceneChange)
	{
		m_bNeedLerp = true;
		m_dLerpTime = 0.0;
		m_fCloseAngle = 25.f;
		m_bShopOpen = g_bReadySceneChange;
		m_vLerpStartEye = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
		m_vLerpStartAt = XMLoadFloat4(&m_vLerpStartEye) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}

	if (g_bReadySceneChange == false)
	{
		_vector	vTargetPos = m_pOwnerTransform->Get_State(CTransform::STATE_TRANS);
		vTargetPos = XMVectorSet(XMVectorGetX(vTargetPos), XMVectorGetY(vTargetPos) + 2.f, XMVectorGetZ(vTargetPos), 1.f);

		m_MouseMoveX = 0;
		m_MouseMoveY = 0;
		_float		fVerticalAngle = 0.f;

		fVerticalAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
		if (isnan(fVerticalAngle))
		{
			fVerticalAngle = 0.f;
		}

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

		if (m_bNeedLerp)
		{
			_float4	vLerpEndEye = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
			_float4	vLerpEndAt = XMLoadFloat4(&m_vLerpStartEye) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			_float4	vLerpEye = _float4::Lerp(m_vLerpStartEye, vLerpEndEye, (_float)m_dLerpTime);
			_float4	vLerpAt = _float4::Lerp(m_vLerpStartAt, vLerpEndAt, (_float)m_dLerpTime);

			m_pTransformCom->Set_State(CTransform::STATE_TRANS, vLerpEye);
			m_pTransformCom->LookAt(vLerpAt);

			m_dLerpTime += dTimeDelta * 0.5f;

			if (m_dLerpTime > 1.0)
				m_bNeedLerp = false;
		}
	}
	else
	{
		_vector	vTargetPos = m_pOwnerTransform->Get_State(CTransform::STATE_TRANS);
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.8f);
		_vector	vTargetLook = XMVector3Normalize(XMVectorSetY(m_pOwnerTransform->Get_State(CTransform::STATE_LOOK), 0.f));
		_vector	vTargetRight= XMVector3Normalize(XMVectorSetY(m_pOwnerTransform->Get_State(CTransform::STATE_RIGHT), 0.f));

		vTargetLook = XMVector3TransformNormal(vTargetLook, XMMatrixRotationY(XMConvertToRadians(m_fCloseAngle)));

		if (CGameInstance::GetInstance()->Key_Pressing(DIK_A))
			m_fCloseAngle -= 1.f;
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_D))
			m_fCloseAngle += 1.f;

		_vector	vEye = vTargetPos + vTargetLook * 1.3f;
		_vector	vAt = vTargetPos;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vEye);
		m_pTransformCom->LookAt(vAt);

		_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		vEye += vRight * 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vEye);

		if (m_bNeedLerp)
		{
			_float4	vLerpEndEye = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
			_float4	vLerpEye = _float4::Lerp(m_vLerpStartEye, vLerpEndEye, (_float)m_dLerpTime);
			
			m_pTransformCom->Set_State(CTransform::STATE_TRANS, vLerpEye);
			m_pTransformCom->LookAt(vTargetPos + vRight * 1.f);

			m_dLerpTime += dTimeDelta * 0.5f;
		
			if (m_dLerpTime > 1.0)
				m_bNeedLerp = false;
		}
	}

	__super::Tick(dTimeDelta);

	//Update_ReflectWorld();

	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, m_dTimeScale);
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

void CStatic_Camera::Update_ReflectWorld()
{
	_vector	vTargetPos = m_pOwnerTransform->Get_State(CTransform::STATE_TRANS);

	_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();
	_float3	vScale = m_pTransformCom->Get_Scale();

	_vector	vEye = matWorld.r[3];

	_vector	vLook = XMVector3Normalize(vTargetPos - vEye) * vScale.z;
	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	matWorld.r[0] = vRight;
	matWorld.r[1] = vUp;
	matWorld.r[2] = vLook;
	matWorld.r[3] = vEye;

	m_matReflectWorld = matWorld;

	CGameInstance::GetInstance()->Set_Transform(CPipeLine::D3DTS_CAMWORLD, m_matReflectWorld);
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
