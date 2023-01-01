#include "stdafx.h"
#include "Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "Transform.h"
#include "GameUtility.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CMesh::CMesh(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_strName(rhs.m_strName)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumMeshBone(rhs.m_iNumMeshBone)
	, m_pNonAnimVertices(rhs.m_pNonAnimVertices)
	, m_pAnimVertices(rhs.m_pAnimVertices)
	, m_pIndices(rhs.m_pIndices)
{
}

HRESULT CMesh::Save_Mesh(HANDLE& hFile, DWORD& dwByte)
{
	if (m_eType == CModel::MODELTYPE_END)
		return E_FAIL;

	_uint			iNameLength = (_uint)m_strName.length() + 1;
	const char*	pName = m_strName.c_str();
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	WriteFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumMeshBone, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iStride, sizeof(_uint), &dwByte, nullptr);

	if (m_eType == CModel::MODEL_NONANIM)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
			WriteFile(hFile, &m_pNonAnimVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);
	}
	else if (m_eType == CModel::MODEL_ANIM)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
			WriteFile(hFile, &m_pAnimVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);
	}

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
		WriteFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

	return S_OK;
}

HRESULT CMesh::Save_MeshBones(HANDLE & hFile, DWORD & dwByte)
{
	for (auto& pBone : m_vecMeshBone)
		pBone->Save_BoneName(hFile, dwByte);

	return S_OK;
}

HRESULT CMesh::Load_Mesh(HANDLE & hFile, DWORD & dwByte)
{
	if (m_eType == CModel::MODELTYPE_END)
		return E_FAIL;

	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	m_strName = pName;
	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumMeshBone, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iStride, sizeof(_uint), &dwByte, nullptr);

	m_iNumVertexBuffers = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	if (m_eType == CModel::MODEL_NONANIM)
	{
		m_pNonAnimVertices = new VTXMODEL[m_iNumVertices];

		for (_uint i = 0; i < m_iNumVertices; ++i)
			ReadFile(hFile, &m_pNonAnimVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);

		ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_tSubResourceData.pSysMem = m_pNonAnimVertices;
	}
	else if (m_eType == CModel::MODEL_ANIM)
	{
		m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];

		for (_uint i = 0; i < m_iNumVertices; ++i)
			ReadFile(hFile, &m_pAnimVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);

		ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_tSubResourceData.pSysMem = m_pAnimVertices;
	}

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.StructureByteStride = 0;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
		ReadFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

void CMesh::Check_MeshSize(_float & Xmin, _float & Xmax, _float & Ymin, _float & Ymax, _float & Zmin, _float & Zmax)
{
	if (m_eType == CModel::MODEL_NONANIM)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			Xmin = min(Xmin, m_pNonAnimVertices[i].vPosition.x);
			Xmax = max(Xmax, m_pNonAnimVertices[i].vPosition.x);

			Ymin = min(Ymin, m_pNonAnimVertices[i].vPosition.y);
			Ymax = max(Ymax, m_pNonAnimVertices[i].vPosition.y);

			Zmin = min(Zmin, m_pNonAnimVertices[i].vPosition.z);
			Zmax = max(Zmax, m_pNonAnimVertices[i].vPosition.z);
		}
	}
	else if (m_eType == CModel::MODEL_ANIM)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			Xmin = min(Xmin, m_pAnimVertices[i].vPosition.x);
			Xmax = max(Xmax, m_pAnimVertices[i].vPosition.x);

			Ymin = min(Ymin, m_pAnimVertices[i].vPosition.y);
			Ymax = max(Ymax, m_pAnimVertices[i].vPosition.y);

			Zmin = min(Zmin, m_pAnimVertices[i].vPosition.z);
			Zmax = max(Zmax, m_pAnimVertices[i].vPosition.z);
		}
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::MODELTYPE eType, aiMesh * pAIMesh, CModel* pModel)
{
	if (eType == CModel::MODELTYPE_END)
		return E_FAIL;

	m_eType = eType;

	if (pAIMesh == nullptr)
		return S_OK;

	m_pAIMesh = pAIMesh;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

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
		FAILED_CHECK_RETURN(Ready_VertexBuffer_NonAnimModel(pAIMesh, pModel), E_FAIL);
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

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		m_pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		m_pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		m_pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize(class CGameObject* pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

void CMesh::SetUp_MeshBones(CModel * pModel)
{
	for (_uint i = 0; i < m_iNumMeshBone; ++i)
	{
		aiBone*	pAIBone = m_pAIMesh->mBones[i];

		CBone*	pBone = pModel->Get_BoneFromEntireBone(pAIBone->mName.data);
		NULL_CHECK_RETURN(pBone, );

		_float4x4 matOffset;
		memcpy(&matOffset, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&matOffset, XMMatrixTranspose(XMLoadFloat4x4(&matOffset)));

		pBone->Set_matOffset(matOffset);

		m_vecMeshBone.push_back(pBone);
		Safe_AddRef(pBone);
	}

	if (m_iNumMeshBone == 0)
	{
		CBone*	pBone = pModel->Get_BoneFromEntireBone(m_strName);
		NULL_CHECK_RETURN(pBone, );

		m_iNumMeshBone = 1;

		m_vecMeshBone.push_back(pBone);
		Safe_AddRef(pBone);
	}
}

HRESULT CMesh::SetUp_MeshBones(HANDLE & hFile, DWORD & dwByte, CModel * pModel)
{
	for (_uint i = 0; i < m_iNumMeshBone; ++i)
	{
		_uint		iBoneNameLength = 0;
		ReadFile(hFile, &iBoneNameLength, sizeof(_uint), &dwByte, nullptr);
		char*		pBoneName = new char[iBoneNameLength];
		ReadFile(hFile, pBoneName, sizeof(char) * iBoneNameLength, &dwByte, nullptr);

		CBone*		pBone = pModel->Get_BoneFromEntireBone(string(pBoneName));

		Safe_Delete_Array(pBoneName);
		if (pBone == nullptr)
			continue;

		m_vecMeshBone.push_back(pBone);
		Safe_AddRef(pBone);
	}

	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix matPivot)
{
	if (m_iNumMeshBone == 0)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], matPivot);
		return;
	}

	_uint	iNumBones = 0;

	for (auto& pBone : m_vecMeshBone)
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++], pBone->Get_matOffset() * pBone->Get_CombindMatrix() * matPivot);
}

pair<_bool, _float> CMesh::Picking(HWND & hWnd, CTransform * pTransformCom, _float3 & vPickingPoint)
{
	pair<_bool, _float>	Result{ false, 0.f };

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	_uint	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewPort);

	Result = CGameUtility::Picking(hWnd, ViewPort.Width, ViewPort.Height, pTransformCom, m_pNonAnimVertices, m_pIndices, m_iNumPrimitive, vPickingPoint);

	return Result;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(aiMesh * pAIMesh, CModel * pModel)
{
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	m_pNonAnimVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(m_pNonAnimVertices, sizeof(VTXMODEL) * m_iNumVertices);

	_matrix		matPivot = pModel->Get_PivotMatrix();

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&m_pNonAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&m_pNonAnimVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pNonAnimVertices[i].vPosition), matPivot));

		memcpy(&m_pNonAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&m_pNonAnimVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&m_pNonAnimVertices[i].vNormal), matPivot));

		if (pAIMesh->mTextureCoords[0] != nullptr)
			memcpy(&m_pNonAnimVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		else
			m_pNonAnimVertices[i].vTexUV = _float2(0.f, 0.f);
		memcpy(&m_pNonAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = m_pNonAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(aiMesh * pAIMesh, CModel * pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.StructureByteStride = m_iStride;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;

	m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&m_pAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		if (pAIMesh->mTextureCoords[0] != nullptr)
			memcpy(&m_pAnimVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		else
			m_pAnimVertices[i].vTexUV = _float2(0.f, 0.f);
		memcpy(&m_pAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumMeshBone = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumMeshBone; ++i)
	{
		aiBone*	pAIBone = pAIMesh->mBones[i];
		_uint		iNumWeights = pAIBone->mNumWeights;

		for (_uint j = 0; j < iNumWeights; ++j)
		{
			_uint	iVertexIndex = pAIBone->mWeights[j].mVertexId;

			if (fabs(m_pAnimVertices[iVertexIndex].vBlendWeight.x - 0.f) < EPSILON)
			{
				m_pAnimVertices[iVertexIndex].vBlendIndex.x = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}
			else if (fabs(m_pAnimVertices[iVertexIndex].vBlendWeight.y - 0.f) < EPSILON)
			{
				m_pAnimVertices[iVertexIndex].vBlendIndex.y = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}
			else if (fabs(m_pAnimVertices[iVertexIndex].vBlendWeight.z - 0.f) < EPSILON)
			{
				m_pAnimVertices[iVertexIndex].vBlendIndex.z = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}
			else if (fabs(m_pAnimVertices[iVertexIndex].vBlendWeight.w - 0.f) < EPSILON)
			{
				m_pAnimVertices[iVertexIndex].vBlendIndex.w = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	ZeroMemory(&m_tSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tSubResourceData.pSysMem = m_pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

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

CComponent * CMesh::Clone(CGameObject * pOwner, void * pArg)
{
	CMesh*	pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	if (!m_bIsCloned)
	{
		Safe_Delete_Array(m_pNonAnimVertices);
		Safe_Delete_Array(m_pAnimVertices);
		Safe_Delete_Array(m_pIndices);
	}

	for (auto& pBone : m_vecMeshBone)
		Safe_Release(pBone);
	m_vecMeshBone.clear();
}
