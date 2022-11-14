#include "..\Public\Transform.h"

CTransform::CTransform(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
	//, m_TransformDesc(rhs.m_TransformDesc) // Initialize()에서 복사 해줌.
{
}

HRESULT CTransform::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Go_Straight(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPos += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_BackWard(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPos -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Left(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Right(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Turn(_fvector vAxis, _double dTimeDelta)
{
	_matrix	matRotation = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)dTimeDelta);

	_vector	vRight		= Get_State(CTransform::STATE_RIGHT);
	_vector	vUp		= Get_State(CTransform::STATE_UP);
	_vector	vLook		= Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, matRotation));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, matRotation));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, matRotation));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix	matRotation = XMMatrixRotationAxis(vAxis, fRadian);

	_vector	vRight		= XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector	vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector	vLook		= XMVectorSet(0.f, 0.f, 1.f, 0.f);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, matRotation));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, matRotation));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, matRotation));
}

CTransform * CTransform::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
