#include "stdafx.h"
#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	m_vPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _float3(1.f, 1.f, 0.f);
	m_vPoint[2] = _float3(1.f, -1.f, 0.f);
	m_vPoint[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _float3(1.f, 1.f, 1.f);
	m_vPoint[6] = _float3(1.f, -1.f, 1.f);
	m_vPoint[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::TransformToWorldSpace()
{
	CPipeLine*	pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_matrix		matViewInv = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix		matProjInv = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);

	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&m_vPointInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), matProjInv));
		XMStoreFloat3(&m_vPointInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointInWorld[i]), matViewInv));
	}

	Build_Planes(m_vPlaneInWorld, m_vPointInWorld);

	Safe_Release(pPipeLine);
}

void CFrustum::TransformToLocalSpace(_fmatrix matWorld)
{
	_matrix		matWorldInv = XMMatrixInverse(nullptr, matWorld);

	_float3		vPointInLocal[8];

	for (_uint i = 0; i < 8; ++i)
		XMStoreFloat3(&vPointInLocal[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointInWorld[i]), matWorldInv));

	Build_Planes(m_vPlaneInLocal, vPointInLocal);
}

_bool CFrustum::IsInFrustum_World(_fvector vWorldPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInWorld[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool CFrustum::IsInFrustum_Local(_fvector vLocalPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInLocal[i]), vLocalPos)))
			return false;
	}
	
	return true;
}

void CFrustum::Build_Planes(_float4 * pPlanes, const _float3 * pPoints)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{
}