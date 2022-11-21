#include "stdafx.h"
#include "..\Public\Dynamic_Camera.h"
#include "GameInstance.h"

CDynamic_Camera::CDynamic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CCamera(pDevice, pContext)
{
}


CDynamic_Camera::CDynamic_Camera(const CDynamic_Camera& rhs)
	: CCamera(rhs)
{
}

HRESULT CDynamic_Camera::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CDynamic_Camera::Initialize(void * pArg)
{
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

	FAILED_CHECK_RETURN(__super::Initialize(&CameraDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CDynamic_Camera::Tick(_double dTimeDelta)
{
	if (GetKeyState('W') & 0x8000)
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (GetKeyState('S') & 0x8000)
		m_pTransformCom->Go_BackWard(dTimeDelta);
	if (GetKeyState('A') & 0x8000)
		m_pTransformCom->Go_Left(dTimeDelta);
	if (GetKeyState('D') & 0x8000)
		m_pTransformCom->Go_Right(dTimeDelta);
	if (GetKeyState(VK_SPACE) & 0x8000)
		m_pTransformCom->Go_Up(dTimeDelta);
	if (GetKeyState(VK_LCONTROL) & 0x8000)
		m_pTransformCom->Go_Down(dTimeDelta);
	
	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long				MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * MouseMove * 0.1f);
	if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * MouseMove * 0.1f);

	Safe_Release(pGameInstance);

	__super::Tick(dTimeDelta);
}

void CDynamic_Camera::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CDynamic_Camera::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CDynamic_Camera::SetUp_Component()
{
	return S_OK;
}

CDynamic_Camera * CDynamic_Camera::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CDynamic_Camera*		pInstance = new CDynamic_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CDynamic_Camera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDynamic_Camera::Clone(void * pArg)
{
	CDynamic_Camera*		pInstance = new CDynamic_Camera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Camera::Free()
{
	__super::Free();
}