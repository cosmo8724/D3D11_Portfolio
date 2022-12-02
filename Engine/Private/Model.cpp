#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMesh(rhs.m_vecMesh)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumEntireBone(rhs.m_iNumEntireBone)
	, m_vecEntireBone(rhs.m_vecEntireBone)
	, m_iCurAnimationIndex(rhs.m_iCurAnimationIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_vecAnimation(rhs.m_vecAnimation)
{
	for (auto& pMesh : m_vecMesh)
		Safe_AddRef(pMesh);

	for (auto& ModelMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(ModelMaterial.pTexture[i]);
	}

	for (auto& pBone : m_vecEntireBone)
		Safe_AddRef(pBone);

	for (auto& pAnimation : m_vecAnimation)
		Safe_AddRef(pAnimation);
}

CBone * CModel::Get_BoneFromEntireBone(const string & strBoneName)
{
	auto	iter = find_if(m_vecEntireBone.begin(), m_vecEntireBone.end(), [&](CBone* pBone)->bool {
		return strBoneName == pBone->Get_Name();
	});

	if (iter == m_vecEntireBone.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eType, const char * pModelFilePath)
{
	if (eType == MODELTYPE_END)
		return E_FAIL;

	m_eType = eType;

	_uint		iFlag = 0;

	if (m_eType == MODEL_NONANIM)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	NULL_CHECK_RETURN(m_pAIScene, E_FAIL);

	FAILED_CHECK_RETURN(Ready_Bones(m_pAIScene->mRootNode), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MeshContainers(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Materials(pModelFilePath), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Animations(), E_FAIL);

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

void CModel::ImGui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Materials"))
	{
		for (size_t i = 0; i < m_iNumMaterials; ++i)
		{
			ImGui::Text("%d", i);
			ImGui::Separator();
			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				if (m_Materials[i].pTexture[j] != nullptr)
				{
					ImGui::Image(m_Materials[i].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
					ImGui::SameLine();
				}
			}
			ImGui::NewLine();
		}
	}
	
	if (ImGui::CollapsingHeader("Meshes"))
	{
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			_uint	iMaterialIndex = m_vecMesh[i]->Get_MaterialIndex();
			ImGui::BulletText("%s", m_vecMesh[i]->Get_MeshName().c_str());
			ImGui::Separator();

			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				if (m_Materials[iMaterialIndex].pTexture[j] != nullptr)
				{
					ImGui::SameLine();
					ImGui::Image(m_Materials[iMaterialIndex].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
				}
			}
		}
	}
}

void CModel::Play_Animation(_double dTimeDelta)
{
	m_vecAnimation[m_iCurAnimationIndex]->Update_Bones(dTimeDelta);

	for (auto& pBone : m_vecEntireBone)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader * pShaderCom, _uint iMeshIndex, aiTextureType eType, const wstring & wstrConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint	iMaterialIndex = m_vecMesh[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShaderCom, wstrConstantName);
	else
		return S_FALSE;

	return S_OK;
}

HRESULT CModel::Render(CShader * pShaderCom, _uint iMeshIndex, const wstring & wstrBoneConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	if (nullptr != m_vecMesh[iMeshIndex])
	{
		if (wstrBoneConstantName != L"")
		{
			_float4x4		matBones[256];

			m_vecMesh[iMeshIndex]->SetUp_BoneMatrices(matBones);

			pShaderCom->Set_MatrixArray(wstrBoneConstantName, matBones, 256);
		}

		pShaderCom->Begin(0);

		m_vecMesh[iMeshIndex]->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode * pAINode)
{
	CBone*	pBone = CBone::Create(pAINode);
	NULL_CHECK_RETURN(pBone, E_FAIL);

	m_vecEntireBone.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
		Ready_Bones(pAINode->mChildren[i]);

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	NULL_CHECK_RETURN(m_pAIScene, E_FAIL);

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*	pAIMesh = m_pAIScene->mMeshes[i];

		CMesh*	pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, pAIMesh, this);
		NULL_CHECK_RETURN(pMesh, E_FAIL);

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFIlePath)
{
	char	szDirectory[MAX_PATH] = "";

	_splitpath_s(pModelFIlePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		char	szTexturePath[MAX_PATH] = "";
		char	szTextureFileName[MAX_PATH] = "";
		char	szExt[MAX_PATH] = "";

		MODELMATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			_tchar		szFullPath[MAX_PATH] = L"";

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, (_int)strlen(szTexturePath), szFullPath, MAX_PATH);

			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			NULL_CHECK_RETURN(ModelMaterial.pTexture[j], E_FAIL);
		}
		m_Materials.push_back(ModelMaterial);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*	pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*	pAnimation = CAnimation::Create(pAIAnimation, this);
		NULL_CHECK_RETURN(pAnimation, E_FAIL);

		m_vecAnimation.push_back(pAnimation);
	}

	return S_OK;
}

CModel * CModel::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MODELTYPE eType, const char * pModelFilePath)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_vecMesh)
		Safe_Release(pMesh);
	m_vecMesh.clear();

	for (auto& ModelMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(ModelMaterial.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pBone : m_vecEntireBone)
		Safe_Release(pBone);
	m_vecEntireBone.clear();

	for (auto& pAnimation : m_vecAnimation)
		Safe_Release(pAnimation);
	m_vecAnimation.clear();

	m_Importer.FreeScene();
}
