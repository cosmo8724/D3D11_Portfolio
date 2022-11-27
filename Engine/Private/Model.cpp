#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"

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
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
{
	for (auto& pMesh : m_vecMesh)
		Safe_AddRef(pMesh);
	for (auto& ModelMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(ModelMaterial.pTexture[i]);
	}
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eType, const char * pModelFilePath)
{
	_uint		iFlag = 0;

	if (eType == MODEL_NONANIM)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	NULL_CHECK_RETURN(m_pAIScene, E_FAIL);

	FAILED_CHECK_RETURN(Ready_MeshContainers(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Materials(pModelFilePath), E_FAIL);

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

HRESULT CModel::Render(CShader * pShaderCom)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (nullptr != m_Materials[m_vecMesh[i]->Get_MaterialIndex()].pTexture[aiTextureType_DIFFUSE])
			m_Materials[m_vecMesh[i]->Get_MaterialIndex()].pTexture[aiTextureType_DIFFUSE]->Bind_ShaderResource(pShaderCom, L"g_DiffuseTexture");
		if (nullptr != m_Materials[m_vecMesh[i]->Get_MaterialIndex()].pTexture[aiTextureType_NORMALS])
			m_Materials[m_vecMesh[i]->Get_MaterialIndex()].pTexture[aiTextureType_NORMALS]->Bind_ShaderResource(pShaderCom, L"g_NormalTexture");

		if (nullptr != m_vecMesh[i])
			m_vecMesh[i]->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	NULL_CHECK_RETURN(m_pAIScene, E_FAIL);

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*	pAIMesh = m_pAIScene->mMeshes[i];

		CMesh*	pMesh = CMesh::Create(m_pDevice, m_pContext, pAIMesh);
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

	m_Importer.FreeScene();
}
