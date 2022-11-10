#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	virtual HRESULT					Render();

protected:
	D3D11_BUFFER_DESC				m_tBufferDesc;
	D3D11_SUBRESOURCE_DATA	m_tSubResourceData;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	_uint								m_iStride = 0;			// 정점 하나의 바이트 크기
	_uint								m_iNumPrimitive = 0;	// 그려야 할 도형의 개수

protected:
	ID3D11Buffer*					m_pVB = nullptr;
	_uint								m_iNumVertices = 0;
	_uint								m_iNumVertexBuffers = 0;

protected:
	ID3D11Buffer*					m_pIB = nullptr;
	DXGI_FORMAT					m_eIndexFormat;
	_uint								m_iNumIndices = 0;
	_uint								m_iNumIndicesPerPrimitive = 0;
	_uint								m_iIndicesSizePerPrimitive = 0;

protected:
	HRESULT							Create_VertexBuffer();
	HRESULT							Create_IndexBuffer();

public:
	virtual CComponent*			Clone(void* pArg = nullptr) PURE;
	virtual void						Free() override;
};

END