#include "..\Public\Transform.h"
#include "Shader.h"

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

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	if (eState == STATE_TRANS)
		return;

	_vector	vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scale(_float3 vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANS)
		return;

	Set_State(eState, Get_State(eState) * fScale);
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

	vPos += XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_BackWard(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPos -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Left(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos -= XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Right(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos += XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Up(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPos += XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Down(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPos -= XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Turn(_fvector vAxis, _double dTimeDelta)
{
	_matrix	matRotation = XMMatrixRotationAxis(vAxis, (_float)m_TransformDesc.dRotationPerSec * (_float)dTimeDelta);

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

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3	vScale = Get_Scale();

	_vector	vLook = XMVector3Normalize(vTargetPos - Get_State(STATE_TRANS)) * vScale.z;
	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Chase(_fvector vTargetPos, _double dTimeDelta, _float fLimit)
{
	_vector	vPos = Get_State(STATE_TRANS);
	_vector	vDir = vTargetPos - vPos;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance > fLimit)
	{
		vPos += XMVector3Normalize(vDir) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;
		Set_State(STATE_TRANS, vPos);
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const wstring wstrConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	return pShaderCom->Set_Matrix(wstrConstantName, &m_matWorld);
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
