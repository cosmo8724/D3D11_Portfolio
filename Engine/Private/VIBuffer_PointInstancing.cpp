#include "stdafx.h"
#include "..\Public\VIBuffer_PointInstancing.h"


CVIBuffer_PointInstancing::CVIBuffer_PointInstancing(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_PointInstancing::CVIBuffer_PointInstancing(const CVIBuffer_PointInstancing & rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_PointInstancing::Initialize_Prototype(_uint iNumInstance)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumPrimitive = iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	/* Vertex Buffer */
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	VTXPOINT*			pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);
	pVertices->vPSize = _float2(0.2f, 0.2f);

	ZeroMemory(&m_tSubResourceData, sizeof m_tSubResourceData);
	m_tSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/* Index Buffer */
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.StructureByteStride = 0;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	ZeroMemory(&m_tSubResourceData, sizeof m_tSubResourceData);
	m_tSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Initialize(CGameObject * pOwner, void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Tick(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		//m_pInstanceBuffer
	};

	_uint					iStrides[] = {
		m_iStride,
		//m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		//0,
	};

	m_pContext->IASetVertexBuffers(0, 1, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

CVIBuffer_PointInstancing * CVIBuffer_PointInstancing::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iNumInstance)
{
	CVIBuffer_PointInstancing*		pInstance = new CVIBuffer_PointInstancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_PointInstancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_PointInstancing::Clone(CGameObject * pOwner, void * pArg)
{
	CVIBuffer_PointInstancing*		pInstance = new CVIBuffer_PointInstancing(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_PointInstancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_PointInstancing::Free()
{
	__super::Free();
}
