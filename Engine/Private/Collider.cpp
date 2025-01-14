#include "stdafx.h"
#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "GameObject.h"

CCollider::CCollider(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_ColliderDesc(rhs.m_ColliderDesc)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pLayout(rhs.m_pLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pLayout);
#endif
}

_vector CCollider::Get_ColliderPos()
{
	_vector	vPos;

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		vPos = XMVectorSetW(XMLoadFloat3(&m_pSphere_Original->Center), 1.f);
		break;

	case COLLIDER_AABB:
		vPos = XMVectorSetW(XMLoadFloat3(&m_pAABB_Original->Center), 1.f);
		break;

	case COLLIDER_OBB:
		vPos = XMVectorSetW(XMLoadFloat3(&m_pOBB_Original->Center), 1.f);
		break;
	}

	return vPos;
}

void * CCollider::Get_Collider(COLLIDERTYPE eType)
{
	if (eType == CCollider::COLLIDER_SPHERE && m_pSphere != nullptr)
		return m_pSphere;

	else if (eType == CCollider::COLLIDER_AABB && m_pAABB != nullptr)
		return m_pAABB;

	else if (eType == CCollider::COLLIDER_OBB && m_pOBB != nullptr)
		return m_pOBB;

	return nullptr;
}

HRESULT CCollider::Initialize_Prototype(COLLIDERTYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	FAILED_CHECK_RETURN(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pLayout), E_FAIL);
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	memcpy(&m_ColliderDesc, pArg, sizeof(m_ColliderDesc));

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		m_pSphere_Original->Transform(*m_pSphere_Original,
			XMMatrixScaling(m_ColliderDesc.vSize.x, m_ColliderDesc.vSize.y, m_ColliderDesc.vSize.z) *
			XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
			XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(m_ColliderDesc.vRotation.z) *
			XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z));
		m_pSphere = new BoundingSphere(*m_pSphere_Original);
		break;

	case COLLIDER_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB_Original->Transform(*m_pAABB_Original,
			XMMatrixScaling(m_ColliderDesc.vSize.x, m_ColliderDesc.vSize.y, m_ColliderDesc.vSize.z) *
			XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z));
		m_pAABB = new BoundingBox(*m_pAABB_Original);
		break;

	case COLLIDER_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB_Original->Transform(*m_pOBB_Original,
			XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
			XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(m_ColliderDesc.vRotation.z) *
			XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z));
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix matTransform)
{
	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, matTransform);
		break;

	case COLLIDER_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(matTransform));
		break;

	case COLLIDER_OBB:
		m_pOBB_Original->Transform(*m_pOBB, matTransform);
		break;
	}
}

void CCollider::Resize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere_Original, XMMatrixScaling(m_ColliderDesc.vSize.x * fSizeX, m_ColliderDesc.vSize.y * fSizeY, m_ColliderDesc.vSize.z * fSizeZ)  *
			XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
			XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(m_ColliderDesc.vRotation.z) *
			XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z));
		break;

	case COLLIDER_AABB:
		m_pAABB_Original->Transform(*m_pAABB_Original, XMMatrixScaling(fSizeX, fSizeY, fSizeZ) *
			XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z));
		break;
	}
}

_bool CCollider::Collision(CCollider * pTargetCollider)
{
	m_bIsCollide = pTargetCollider->m_bIsCollide = false;;

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pSphere->Intersects(*pTargetCollider->m_pSphere);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pSphere->Intersects(*pTargetCollider->m_pAABB);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pSphere->Intersects(*pTargetCollider->m_pOBB);
		break;

	case COLLIDER_AABB:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pAABB->Intersects(*pTargetCollider->m_pSphere);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pAABB->Intersects(*pTargetCollider->m_pAABB);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pAABB->Intersects(*pTargetCollider->m_pOBB);
		break;

	case COLLIDER_OBB:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pOBB->Intersects(*pTargetCollider->m_pSphere);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pOBB->Intersects(*pTargetCollider->m_pAABB);
		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB)
			m_bIsCollide = pTargetCollider->m_bIsCollide = m_pOBB->Intersects(*pTargetCollider->m_pOBB);
		break;
	}

	return m_bIsCollide;
}

_bool CCollider::Collision_Ray(_fvector vRayPos, _fvector vRayDir, _float & fDist)
{
	m_bIsCollide = false;

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		m_bIsCollide = m_pSphere->Intersects(vRayPos, vRayDir, fDist);
		break;

	case COLLIDER_AABB:
		m_bIsCollide = m_pAABB->Intersects(vRayPos, vRayDir, fDist);
		break;

	case COLLIDER_OBB:
		m_bIsCollide = m_pOBB->Intersects(vRayPos, vRayDir, fDist);
		break;
	}

	return m_bIsCollide;
}

CGameObject * CCollider::CollisionReturnObj(CCollider * pTargetCollider)
{
	m_bIsCollide = false;
	CGameObject*		pTarget = nullptr;

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE &&
			m_pSphere->Intersects(*pTargetCollider->m_pSphere))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB &&
			m_pSphere->Intersects(*pTargetCollider->m_pAABB))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB &&
			m_pSphere->Intersects(*pTargetCollider->m_pOBB))
			pTarget = pTargetCollider->Get_Owner();
		break;

	case COLLIDER_AABB:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE &&
			m_pAABB->Intersects(*pTargetCollider->m_pSphere))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB &&
			m_pAABB->Intersects(*pTargetCollider->m_pAABB))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB &&
			m_pAABB->Intersects(*pTargetCollider->m_pOBB))
			pTarget = pTargetCollider->Get_Owner();
		break;

	case COLLIDER_OBB:
		if (pTargetCollider->Get_ColliderType() == COLLIDER_SPHERE &&
			m_pOBB->Intersects(*pTargetCollider->m_pSphere))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_AABB &&
			m_pOBB->Intersects(*pTargetCollider->m_pAABB))
			pTarget = pTargetCollider->Get_Owner();

		if (pTargetCollider->Get_ColliderType() == COLLIDER_OBB &&
			m_pOBB->Intersects(*pTargetCollider->m_pOBB))
			pTarget = pTargetCollider->Get_Owner();
		break;
	}

	if (pTarget != nullptr)
		m_bIsCollide = pTargetCollider->m_bIsCollide = true;

	return pTarget;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_vColor = m_bIsCollide == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*	pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pLayout);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case COLLIDER_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));
		break;

	case COLLIDER_AABB:
		DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
		break;

	case COLLIDER_OBB:
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif

_matrix CCollider::Remove_Rotation(_fmatrix matTransform)
{
	_matrix		matResult = matTransform;

	matResult.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(matTransform.r[0]));
	matResult.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(matTransform.r[1]));
	matResult.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(matTransform.r[2]));

	return matResult;
}

CCollider * CCollider::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, COLLIDERTYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Create : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(CGameObject * pOwner, void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);

#ifdef _DEBUG
	Safe_Release(m_pLayout);

	if (!m_bIsCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}
