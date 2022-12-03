#include "Mesh.h"
#include "Model.h"
#include "Bone.h"

CMesh::CMesh(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumMeshBone(rhs.m_iNumMeshBone)
	, m_vecMeshBone(rhs.m_vecMeshBone)
{
	for (auto& pBone : m_vecMeshBone)
		Safe_AddRef(pBone);
}

HRESULT CMesh::Initialize_Prototype(CModel::MODELTYPE eType, aiMesh * pAIMesh, CModel* pModel)
{
	if (eType == CModel::MODELTYPE_END)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_eType = eType;
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_strName = pAIMesh->mName.C_Str();
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	/* Create Vertex Buffer */
	if (m_eType == CModel::MODEL_NONANIM)
	{
		FAILED_CHECK_RETURN(Ready_VertexBuffer_NonAnimModel(pAIMesh), E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(Ready_VertexBuffer_AnimModel(pAIMesh, pModel), E_FAIL);
	}

	/* Create Index Buffer */
	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.StructureByteStride = 0;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices)
{
	_uint	iNumBones = 0;

	for (auto& pBone : m_vecMeshBone)
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++], pBone->Get_matOffset() * pBone->Get_CombindMatrix());
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(aiMesh * pAIMesh)
{
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	VTXMODEL*	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(aiMesh * pAIMesh, CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumMeshBone = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumMeshBone; ++i)
	{
		aiBone*	pAIBone = pAIMesh->mBones[i];

		CBone*	pBone = pModel->Get_BoneFromEntireBone(pAIMesh->mName.data);
		NULL_CHECK_RETURN(pBone, E_FAIL);

		_float4x4 matOffset;
		memcpy(&matOffset, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&matOffset, XMMatrixTranspose(XMLoadFloat4x4(&matOffset)));

		pBone->Set_matOffset(matOffset);

		m_vecMeshBone.push_back(pBone);
		Safe_AddRef(pBone);

		_uint		iNumWeights = pAIBone->mNumWeights;

		for (_uint j = 0; j < iNumWeights; ++j)
		{
			_uint	iVertexIndex = pAIBone->mWeights[j].mVertexId;

			if (fabs(pVertices[iVertexIndex].vBlendWeight.x - 0.f) < EPSILON)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}
			if (fabs(pVertices[iVertexIndex].vBlendWeight.y - 0.f) < EPSILON)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}
			if (fabs(pVertices[iVertexIndex].vBlendWeight.z - 0.f) < EPSILON)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}
			if (fabs(pVertices[iVertexIndex].vBlendWeight.w - 0.f) < EPSILON)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh * CMesh::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CModel::MODELTYPE eType, aiMesh * pAIMesh, CModel* pModel)
{
	CMesh*	pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, pModel)))
	{
		MSG_BOX("Failed to Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*	pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_vecMeshBone)
		Safe_Release(pBone);
	m_vecMeshBone.clear();
}
