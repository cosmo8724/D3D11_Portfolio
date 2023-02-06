#include "stdafx.h"
#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pInstanceBuffer(rhs.m_pInstanceBuffer)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
	Safe_AddRef(m_pInstanceBuffer);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(CGameObject * pOwner, void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Tick(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	NULL_CHECK_RETURN(m_pContext, E_FAIL);

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB, m_pInstanceBuffer };
	_uint				iStrides[] = { m_iStride, m_iInstanceStride };
	_uint				iOffsets[] = { 0, 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pInstanceBuffer);
}
