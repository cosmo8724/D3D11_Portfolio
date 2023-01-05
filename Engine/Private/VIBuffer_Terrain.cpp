#include "stdafx.h"
#include "..\Public\VIBuffer_Terrain.h"
#include "QuadTree.h"
#include "Frustum.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer(pDevice, pContext)
{
}


CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pPos(rhs.m_pPos)
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring wstrHeightMapFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	_ulong		dwByte = 0;
	HANDLE	hFile = CreateFile(wstrHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;
	_ulong*					pPixel = nullptr;
	
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	/* Initialize Vertex Buffer */
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_pPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pPos, sizeof(_float3) * m_iNumVertices);

	for (_uint z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX; ++x)
		{
			_uint		iIndex = z * m_iNumVerticesX + x;

			pVertices[iIndex].vPosition = m_pPos[iIndex] = _float3((_float)x, (pPixel[iIndex] & 0x000000ff) / 150.f, (_float)z);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(x / (m_iNumVerticesX - 1.f) * 3.f, z / (m_iNumVerticesZ - 1.f) * 3.f);
		}
	}
	Safe_Delete_Array(pPixel);
	
	/* Initialize Index Buffer */
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	_vector	vSour, vDest, vNormal;

	for (_uint z = 0; z < m_iNumVerticesZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; ++x)
		{
			_uint		iIndex = z * m_iNumVerticesX + x;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];
			
			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));
			++iNumFaces;
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
	m_tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.StructureByteStride = 0;
	m_tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tBufferDesc.MiscFlags = 0;

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_tSubResourceData.pSysMem = pIndices;

	FAILED_CHECK_RETURN(__super::Create_IndexBuffer(), E_FAIL);

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	m_pQuadTree = CQuadTree::Create(
		m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);
	
	m_pQuadTree->Make_Neighbor();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

void CVIBuffer_Terrain::Tick(_double dTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX; ++x)
		{
			_uint		iIndex = z * m_iNumVerticesX + x;

			if (iIndex + 1 != m_iNumVertices)
				((VTXNORTEX*)SubResource.pData)[iIndex].vTexUV = ((VTXNORTEX*)SubResource.pData)[iIndex + 1].vTexUV;//_float2(x / (m_iNumVerticesX - 1.f), z / (m_iNumVerticesZ - 1.f));
			else
				((VTXNORTEX*)SubResource.pData)[iIndex].vTexUV = ((VTXNORTEX*)SubResource.pData)[0].vTexUV;
		}
	}

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::Culling(_fmatrix matWorld)
{
	CFrustum*	pFrustum = CFrustum::GetInstance();

	pFrustum->TransformToLocalSpace(matWorld);

	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	_uint	iNumFaces = 0;

#ifdef USE_QUADTREE
	m_pQuadTree->Culling(pFrustum, m_pPos, m_pIndices, iNumFaces);

#else
	for (_uint z = 0; z < m_iNumVerticesZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; ++x)
		{
			_uint	iIndex = z * m_iNumVerticesX + x;

			_uint	iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool	IsIn[4] = {
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&m_pPos[iIndices[0]]), 0.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&m_pPos[iIndices[1]]), 0.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&m_pPos[iIndices[2]]), 0.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&m_pPos[iIndices[3]]), 0.f)
			};

			if (IsIn[0] == true || IsIn[1] == true || IsIn[2] == true)
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[1];
				m_pIndices[iNumFaces]._2 = iIndices[2];
				++iNumFaces;
			}

			if (IsIn[0] == true || IsIn[2] == true || IsIn[3] == true)
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[2];
				m_pIndices[iNumFaces]._2 = iIndices[3];
				++iNumFaces;
			}
		}
	}
#endif // USE_QUADTREE

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumFaces * 3;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring wstrHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrHeightMapFilePath)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(CGameObject * pOwner, void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (m_bIsCloned == false)
	{
		Safe_Delete_Array(m_pPos);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pQuadTree);
}
