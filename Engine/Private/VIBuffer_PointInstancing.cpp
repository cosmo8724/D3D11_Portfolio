#include "stdafx.h"
#include "..\Public\VIBuffer_PointInstancing.h"
#include "Shader.h"

CVIBuffer_PointInstancing::CVIBuffer_PointInstancing(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_PointInstancing::CVIBuffer_PointInstancing(const CVIBuffer_PointInstancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iInitNumInstance(rhs.m_iInitNumInstance)
{
}

HRESULT CVIBuffer_PointInstancing::Initialize_Prototype(_uint iNumInstance)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_iInitNumInstance = iNumInstance;
	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 2;
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

	/* Instance Buffer */
	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
	m_tBufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iInstanceStride;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;

	VTXMATRIX*		pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	ZeroMemory(&m_tSubResourceData, sizeof m_tSubResourceData);
	m_tSubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_tBufferDesc, &m_tSubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);

	m_vecInstanceInfo.reserve(m_iInitNumInstance);

	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	/*m_pInstanceInfo = new VTXMATRIX[m_iMaxInstanceCount];
	ZeroMemory(m_pInstanceInfo, sizeof(VTXMATRIX) * m_iMaxInstanceCount);

	for (int i = 0; i < 300; ++i)
	{
		memcpy(&m_pInstanceInfo[i], &_float4x4::Identity, sizeof(_float4x4));
		m_pInstanceInfo[i].vPosition.w = 3.f;
	}*/

	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Tick(_double dTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		//((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= m_pSpeeds[i] * TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < 0.f)
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 3.f;
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Tick_Trail(_double dTimeDelta)
{
	for (VTXMATRIX& pInfo : m_vecInstanceInfo)
		pInfo.vPosition.w -= (_float)dTimeDelta;

	m_vecInstanceInfo.erase(remove_if(m_vecInstanceInfo.begin(), m_vecInstanceInfo.end(), [](const VTXMATRIX& Info) {
		return Info.vPosition.w <= 0.f;
	}), m_vecInstanceInfo.end());

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, m_vecInstanceInfo.data(), sizeof(VTXMATRIX) * m_vecInstanceInfo.size());
	m_pContext->Unmap(m_pInstanceBuffer, 0);

	m_iNumInstance = (_uint)m_vecInstanceInfo.size();

	/*if (m_dTimeDelta > 0.2)
	{
		m_iNumInstance++;
		m_dTimeDelta -= 0.2;
	}

	for (int i = 0; i < m_iNumInstance; ++i)
	{
		_float fCurLife = m_vecInstanceInfo[i].vPosition.w - dTimDelta;
		m_vecInstanceInfo[i].vPosition.w = 1.f;
		m_vecInstanceInfo[i].vPosition += XMVector3Normalize(vDir) * (_float)m_dMoveSpeed * dTimDelta;
		m_vecInstanceInfo[i].vPosition.w = fCurLife;
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < m_vecInstanceInfo.size(); ++i)
		memcpy(&((VTXMATRIX*)SubResource.pData)[i], &m_vecInstanceInfo[i], sizeof(VTXMATRIX));

	m_pContext->Unmap(m_pInstanceBuffer, 0);*/

	return S_OK;
}

HRESULT CVIBuffer_PointInstancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	//m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_PointInstancing::Add_Instance(_float4x4 matInfo)
{
	if (m_vecInstanceInfo.size() >= m_iInitNumInstance)
		return;

	VTXMATRIX	Info;
	memcpy(&Info, &matInfo, sizeof(_float4x4));
	m_vecInstanceInfo.push_back(Info);
}

HRESULT CVIBuffer_PointInstancing::Bind_ShaderResource(CShader * pShaderCom, const wstring & wstrConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	return pShaderCom->Set_MatrixArray(wstrConstantName, (_float4x4*)m_vecInstanceInfo.data(), (_uint)m_vecInstanceInfo.size());
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
