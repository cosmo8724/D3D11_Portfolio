#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_eTopology(rhs.m_eTopology)
	, m_iStride(rhs.m_iStride)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_iIndicesSizePerPrimitive(rhs.m_iIndicesSizePerPrimitive)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	NULL_CHECK_RETURN(m_pContext, E_FAIL);

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB, };
	_uint				iStrides[] = { m_iStride, };
	_uint				iOffsets[] = { 0, };

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr != m_pVB)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_tBufferDesc, &m_tSubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr != m_pIB)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_tBufferDesc, &m_tSubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
