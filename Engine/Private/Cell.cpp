#include "stdafx.h"
#include "..\Public\Cell.h"
#include "GameInstance.h"

CCell::CCell(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	memcpy(m_vPoint, pPoints, sizeof(_float3) * POINT_END);
	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoint);
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
#endif // _DEBUG


	return S_OK;
}

_bool CCell::Compare_Point(const _float3 & SourPoint, const _float3 & DestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	return false;
}

_bool CCell::IsIn(_fvector vTargetPos, _int & iNeighborIndex)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoint[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoint[i]);
		_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
		_vector		vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoint[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			iNeighborIndex = m_iNeighborIndex[i];
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(CShader * pShader)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	_float4x4		matWorld;
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	
	pShader->Set_Matrix(L"g_matWorld", &matWorld);
	pShader->Set_Matrix(L"g_matView", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	pShader->Set_Matrix(L"g_matProj", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	pShader->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}
#endif

CCell * CCell::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Create : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG
}
