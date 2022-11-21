#include "..\Public\VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer(pDevice, pContext)
{
}


CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_iSegments = 20;
	m_iSlices = m_iSegments / 2;
	m_iNumVertices = (m_iSlices + 1) * (m_iSegments + 1) + 1;
	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumPrimitive = m_iSlices * m_iSegments * 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iSlices * m_iSegments * 3 * 2;

	/* Initialize Vertex Buffer */
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));

	_uint		iIndex = 0;

	for (_uint Slice = 0; Slice <= m_iSlices; ++Slice)
	{
		_float		fAngle1 = (_float)Slice / (_float)m_iSlices * XM_PI;
		_float		z = cosf(fAngle1);
		_float		r = sinf(fAngle1);
		
		for (_uint Segment = 0; Segment <= m_iSegments; ++Segment, ++iIndex)
		{
			_float		fAngle2 = (_float)Segment / (_float)m_iSegments * XM_2PI;

			pVertices[iIndex].vPosition = _float3(r * cosf(fAngle2), r * sinf(fAngle2), z);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);//pVertices[iIndex].vPosition;
			pVertices[iIndex].vTexUV = _float2((1.f - z) / 2.f, (_float)Segment / (_float)m_iSegments);
		}
	}
	
	/* Initialize Index Buffer */
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	iIndex = 0;
	_vector	vSour, vDest, vNormal;

	for (_uint Slice = 0; Slice < m_iSlices; ++Slice)
	{
		_uint		p1 = Slice * (m_iSegments + 1);
		_uint		p2 = (Slice + 1) * (m_iSegments + 1);

		for (_uint Segment = 0; Segment < m_iSegments; ++Segment)
		{
			if (Slice < m_iSlices - 1)
			{
				pIndices[iIndex]._0 = Segment + p1;
				pIndices[iIndex]._1 = Segment + p2;
				pIndices[iIndex]._2 = Segment + p2 + 1;

				vSour = XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iIndex]._0].vPosition);
				vDest = XMLoadFloat3(&pVertices[pIndices[iIndex]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

				XMStoreFloat3(&pVertices[pIndices[iIndex]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._0].vNormal) + vNormal));
				XMStoreFloat3(&pVertices[pIndices[iIndex]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vNormal) + vNormal));
				XMStoreFloat3(&pVertices[pIndices[iIndex]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._2].vNormal) + vNormal));

				++iIndex;
			}
			if (Slice > 0)
			{
				pIndices[iIndex]._0 = Segment + p1;
				pIndices[iIndex]._1 = Segment + p2 + 1;
				pIndices[iIndex]._2 = Segment + p1 + 1;

				vSour = XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iIndex]._0].vPosition);
				vDest = XMLoadFloat3(&pVertices[pIndices[iIndex]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

				XMStoreFloat3(&pVertices[pIndices[iIndex]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._0].vNormal) + vNormal));
				XMStoreFloat3(&pVertices[pIndices[iIndex]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._1].vNormal) + vNormal));
				XMStoreFloat3(&pVertices[pIndices[iIndex]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iIndex]._2].vNormal) + vNormal));

				++iIndex;
			}
		}
	}

	/* Create Vertex Buffer */
	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = pVertices;

	FAILED_CHECK_RETURN(__super::Create_VertexBuffer(), E_FAIL);

	/* Create Index Buffer */
	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_tBufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.StructureByteStride = 0;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_tSubResourceData.pSysMem = pIndices;

	FAILED_CHECK_RETURN(__super::Create_IndexBuffer(), E_FAIL);

	Safe_Delete_Array(pVertices);

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
}

CVIBuffer_Sphere * CVIBuffer_Sphere::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CVIBuffer_Sphere*		pInstance = new CVIBuffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Sphere::Clone(void * pArg)
{
	CVIBuffer_Sphere*		pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();
}
