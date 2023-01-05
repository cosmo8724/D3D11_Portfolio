#include "stdafx.h"
#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "GameUtility.h"
#include "GameInstance.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CModel::CModel(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_matPivot(rhs.m_matPivot)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_vecMaterial(rhs.m_vecMaterial)
	, m_iNumEntireBone(rhs.m_iNumEntireBone)
	, m_bAnimFinished(false)
	, m_iLastAnimationIndex(0)
	, m_iCurAnimationIndex(0)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_dwBeginBoneData(rhs.m_dwBeginBoneData)
{
	for (auto& pMesh : rhs.m_vecMesh)
		m_vecMesh.push_back(dynamic_cast<CMesh*>(pMesh->Clone(nullptr)));

	for (auto& ModelMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(ModelMaterial.pTexture[i]);
	}
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

_matrix CModel::Get_BoneMatrix(const string & strBoneName)
{
	CBone*	pBone = Get_BoneFromEntireBone(strBoneName);
	NULL_CHECK_RETURN(pBone, XMMatrixIdentity());

	return pBone->Get_CombindMatrix();
}

_matrix CModel::Get_OffsetMatrix(const string & strBoneName)
{
	CBone*	pBone = Get_BoneFromEntireBone(strBoneName);
	NULL_CHECK_RETURN(pBone, XMMatrixIdentity());

	return pBone->Get_matOffset();
}

_bool CModel::Get_AnimationFinish()
{
	return m_vecAnimation[m_iCurAnimationIndex]->Get_AnimationFinish();
}

_float CModel::Get_AnimationProgress()
{
	return m_vecAnimation[m_iCurAnimationIndex]->Get_AnimationProgress();
}

void CModel::Set_CurAnimationIndex(_uint iAnimationIndex)
{
	if (iAnimationIndex < 0 || iAnimationIndex > m_iNumAnimations || iAnimationIndex == m_iCurAnimationIndex)
		return;

	m_iCurAnimationIndex = iAnimationIndex;
	m_vecAnimation[m_iCurAnimationIndex]->Reset_Animation();

	if (m_iLastAnimationIndex != m_iCurAnimationIndex)
		m_fCurAnimChangeTime = 0.f;
}

void CModel::Set_CurAnimationSpeed(_double dAnimSpeed)
{
	m_vecAnimation[m_iCurAnimationIndex]->Set_AnimationTickPerSecond(dAnimSpeed);
}

void CModel::Reset_Animation()
{
	m_vecAnimation[m_iCurAnimationIndex]->Reset_Animation();
}

HRESULT CModel::Check_MeshSize(const string & strMeshName, _float & Xmin, _float & Xmax, _float & Ymin, _float & Ymax, _float & Zmin, _float & Zmax)
{
	CMesh*	pMesh =Get_Mesh(strMeshName);
	NULL_CHECK_RETURN(pMesh, E_FAIL);

	pMesh->Check_MeshSize(Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eType, const char * pModelFilePath, _fmatrix matPivot)
{
	if (eType == MODELTYPE_END)
		return E_FAIL;

	m_eType = eType;
	_tchar		wszModelFilePath[MAX_PATH] = L"";
	CGameUtility::ctwc(pModelFilePath, wszModelFilePath);
	m_wstrFilePath = wstring(wszModelFilePath);
	XMStoreFloat4x4(&m_matPivot, matPivot);

	_tchar		wszExt[32] = L"";
	_wsplitpath_s(m_wstrFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, wszExt, 32);

	if (!lstrcmp(wszExt, L".fbx"))
	{
		_uint		iFlag = 0;

		if (m_eType == MODEL_NONANIM)
			iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
		else
			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

		m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
		NULL_CHECK_RETURN(m_pAIScene, E_FAIL);

		FAILED_CHECK_RETURN(Ready_MeshContainers(), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Materials(pModelFilePath), E_FAIL);
	}
	else if (!lstrcmp(wszExt, L".model"))
		FAILED_CHECK_RETURN(Load_MeshMaterial(m_wstrFilePath), E_FAIL);

	
	return S_OK;
}

HRESULT CModel::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	_tchar		wszExt[32] = L"";
	_wsplitpath_s(m_wstrFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, wszExt, 32);

	if (!lstrcmp(wszExt, L".fbx"))
	{
		FAILED_CHECK_RETURN(Ready_Bones(m_pAIScene->mRootNode, nullptr), E_FAIL);

		for (auto& pMesh : m_vecMesh)
			pMesh->SetUp_MeshBones(this);

		FAILED_CHECK_RETURN(Ready_Animations(), E_FAIL);
	}
	else if (!lstrcmp(wszExt, L".model"))
	{
		DWORD	dwByte = 0;
		HANDLE	hFile = CreateFileW(m_wstrFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		SetFilePointer(hFile, m_dwBeginBoneData, nullptr, FILE_BEGIN);
		
		DWORD	Temp = 0;
		ReadFile(hFile, &Temp, sizeof(DWORD), &dwByte, nullptr);

		FAILED_CHECK_RETURN(Load_BoneAnimation(hFile, dwByte), E_FAIL);

		CloseHandle(hFile);
	}

	if (m_eType == MODEL_ANIM && m_pOwner != nullptr)
		CGameInstance::GetInstance()->Add_AnimObject(m_pOwner);

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
				if (m_vecMaterial[i].pTexture[j] != nullptr)
				{
					ImGui::Image(m_vecMaterial[i].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
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
				if (m_vecMaterial[iMaterialIndex].pTexture[j] != nullptr)
				{
					ImGui::SameLine();
					ImGui::Image(m_vecMaterial[iMaterialIndex].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
				}
			}
		}
	}
}

void CModel::ImGui_RenderAnimation()
{
	static _int	iSelectAnimation = -1;
	CAnimation*	pAnimation = nullptr;
	char**			ppAnimationTag = new char*[m_iNumAnimations];
	
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		_uint	iTagLength = (_uint)m_vecAnimation[i]->Get_AnimationName().length() + 1;
		ppAnimationTag[i] = new char[iTagLength];
		sprintf_s(ppAnimationTag[i], sizeof(char) * iTagLength, m_vecAnimation[i]->Get_AnimationName().c_str());
	}

	ImGui::BulletText("Animation List");
	ImGui::ListBox("Animations", &iSelectAnimation, ppAnimationTag, (_int)m_iNumAnimations);

	if (iSelectAnimation != -1)
	{
		static _bool	bReName = false;
		static char	szNewName[MAX_PATH] = "";
		pAnimation = m_vecAnimation[iSelectAnimation];

		if (ImGui::Button("Play"))
		{
			Set_CurAnimationIndex(iSelectAnimation);
			m_vecAnimation[iSelectAnimation]->Reset_Animation();
		}
		ImGui::SameLine();
		if (ImGui::Button("ReName"))
		{
			bReName = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			for (_uint i = 0; i < m_iNumAnimations; ++i)
				Safe_Delete_Array(ppAnimationTag[i]);
			Safe_Delete_Array(ppAnimationTag);

			auto	iter = m_vecAnimation.begin();
			for (_int i = 0; i < iSelectAnimation; ++i)
				++iter;

			Safe_Release(m_vecAnimation[iSelectAnimation]);
			m_vecAnimation.erase(iter);			

			m_iNumAnimations--;
			iSelectAnimation = -1;

			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			iSelectAnimation = -1;
			bReName = false;
		}
		if (ImGui::Button("Save"))
		{
			string		strFilePath;
			strFilePath.assign(m_wstrFilePath.begin(), m_wstrFilePath.end());
			Save_Model(strFilePath.c_str());
		}

		if (bReName)
		{
			IMGUI_LEFT_LABEL(ImGui::InputText, "Input New Name", szNewName, MAX_PATH);
			if (ImGui::Button("Confirm"))
			{
				pAnimation->Get_AnimationName() = szNewName;
				sprintf_s(szNewName, "");
				bReName = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("UnDo"))
			{
				sprintf_s(szNewName, "");
				bReName = false;
			}
		}

		ImGui::Separator();
		ImGui::Text("Loop");
		if (ImGui::RadioButton("Allow", m_vecAnimation[iSelectAnimation]->Get_AnimationLoop()))
			m_vecAnimation[iSelectAnimation]->Get_AnimationLoop() = !m_vecAnimation[iSelectAnimation]->Get_AnimationLoop();
		ImGui::SameLine();
		if (ImGui::RadioButton("Disallow", !m_vecAnimation[iSelectAnimation]->Get_AnimationLoop()))
			m_vecAnimation[iSelectAnimation]->Get_AnimationLoop() = !m_vecAnimation[iSelectAnimation]->Get_AnimationLoop();

		ImGui::Text("Animation Speed");
		_double&	dTickPerSecond = m_vecAnimation[iSelectAnimation]->Get_AnimationTickPerSecond();
		IMGUI_LEFT_LABEL(ImGui::InputDouble, "Input", &dTickPerSecond, 0.5, 1.0);
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			dTickPerSecond = 25.0;
	}

	for (_uint i = 0; i < m_iNumAnimations; ++i)
		Safe_Delete_Array(ppAnimationTag[i]);
	Safe_Delete_Array(ppAnimationTag);	
}

void CModel::Play_Animation(_double dTimeDelta, LERPTYPE eType)
{
	if (m_eType == MODEL_NONANIM)
		return;

	if (m_fCurAnimChangeTime < m_fAnimChangeTime)
	{
		m_vecAnimation[m_iLastAnimationIndex]->Update_Bones(dTimeDelta);

		if (eType == LERP_BEGIN)
			m_vecAnimation[m_iCurAnimationIndex]->Update_Lerp(0.0, m_fCurAnimChangeTime / m_fAnimChangeTime);
		else if (eType == LERP_CONTINUE)
			m_vecAnimation[m_iCurAnimationIndex]->Update_Lerp((_double)m_vecAnimation[m_iLastAnimationIndex]->Get_AnimationProgress(), m_fCurAnimChangeTime / m_fAnimChangeTime);

		m_fCurAnimChangeTime += (_float)dTimeDelta;
	}
	else
	{
		m_vecAnimation[m_iCurAnimationIndex]->Update_Bones(dTimeDelta);
		m_iLastAnimationIndex = m_iCurAnimationIndex;
	}

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

	if (nullptr != m_vecMaterial[iMaterialIndex].pTexture[eType])
		m_vecMaterial[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShaderCom, wstrConstantName);
	else
		return S_FALSE;

	return S_OK;
}

HRESULT CModel::Render(CShader * pShaderCom, _uint iMeshIndex, const wstring & wstrBoneConstantName, _uint iPassIndex)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	if (nullptr != m_vecMesh[iMeshIndex])
	{
		if (wstrBoneConstantName != L"")
		{
			_float4x4		matBones[256];

			m_vecMesh[iMeshIndex]->SetUp_BoneMatrices(matBones, XMLoadFloat4x4(&m_matPivot));

			pShaderCom->Set_MatrixArray(wstrBoneConstantName, matBones, 256);
		}

		pShaderCom->Begin(iPassIndex);

		m_vecMesh[iMeshIndex]->Render();
	}

	return S_OK;
}

pair<_bool, _float3> CModel::Picking(HWND & hWnd, CTransform * pTransformCom)
{
	_bool		bIsPicked = false;
	_float3	vPickingPoint = { 0.f, 0.f, 0.f };
	_float3	vReturnPoint = { 0.f, 0.f, 0.f };
	pair<_bool, _float>	PickInfo;
	_float		fMinDist = 1000.f;

	for (auto& pMesh : m_vecMesh)
	{
		PickInfo = pMesh->Picking(hWnd, pTransformCom, vPickingPoint);
		
		if (PickInfo.first == true)
		{
			bIsPicked = true;

			if (PickInfo.second < fMinDist)
			{
				vReturnPoint = vPickingPoint;
				fMinDist = PickInfo.second;
			}
		}
	}

	return pair<_bool, _float3>{ bIsPicked, vReturnPoint };
}

_bool CModel::IsInFrustum(_uint iMeshIndex, _fmatrix matWorld)
{
	return m_vecMesh[iMeshIndex]->IsInFrustum(m_matPivot, matWorld);
}

HRESULT CModel::Ready_Bones(aiNode * pAINode, CBone * pParent)
{
	CBone*	pBone = CBone::Create(pAINode, pParent, pAINode->mNumChildren);
	NULL_CHECK_RETURN(pBone, E_FAIL);

	m_vecEntireBone.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
		Ready_Bones(pAINode->mChildren[i], pBone);

	return S_OK;
}

HRESULT CModel::Ready_Bones(HANDLE & hFile, DWORD & dwByte, CBone * pParent)
{
	NULL_CHECK_RETURN(pParent, E_FAIL);

	_uint iNumChild = pParent->Get_NumChiled();
	while (iNumChild--)
	{
		CBone*	pBone = CBone::Create(nullptr, pParent, 0);
		NULL_CHECK_RETURN(pBone, E_FAIL);

		pBone->Load_Bone(hFile, dwByte);
		m_vecEntireBone.push_back(pBone);

		FAILED_CHECK_RETURN(Ready_Bones(hFile, dwByte, pBone), E_FAIL);
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
		m_vecMaterial.push_back(ModelMaterial);
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

HRESULT CModel::Load_MeshMaterial(const wstring & wstrModelFilePath)
{
	DWORD	dwByte = 0;
	HANDLE	hFile = CreateFileW(wstrModelFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* Meshes */
	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	m_vecMesh.reserve(m_iNumMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, nullptr, this);
		NULL_CHECK_RETURN(pMesh, E_FAIL);

		pMesh->Load_Mesh(hFile, dwByte);

		m_vecMesh.push_back(pMesh);
	}

	/* Materials */
	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	m_vecMaterial.reserve(m_iNumMaterials);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODELMATERIAL		tMaterial;
		ZeroMemory(&tMaterial, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint		iTemp = AI_TEXTURE_TYPE_MAX;
			ReadFile(hFile, &iTemp, sizeof(_uint), &dwByte, nullptr);

			if (iTemp == AI_TEXTURE_TYPE_MAX)
				continue;

			_uint		iFilePathLength = 0;
			ReadFile(hFile, &iFilePathLength, sizeof(_uint), &dwByte, nullptr);
			
			_tchar	*	pFilePath = new _tchar[iFilePathLength];
			ReadFile(hFile, pFilePath, sizeof(_tchar) * iFilePathLength, &dwByte, nullptr);

			tMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, wstring(pFilePath));
			NULL_CHECK_RETURN(tMaterial.pTexture[j], E_FAIL);

			Safe_Delete_Array(pFilePath);
		}

		m_vecMaterial.push_back(tMaterial);
	}

	ReadFile(hFile, &m_dwBeginBoneData, sizeof(DWORD), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Load_BoneAnimation(HANDLE & hFile, DWORD & dwByte)
{
	/* Bones */
	ReadFile(hFile, &m_iNumEntireBone, sizeof(_uint), &dwByte, nullptr);
	m_vecEntireBone.reserve(m_iNumEntireBone);

	CBone*		pRootBone = CBone::Create(nullptr, nullptr, 0);
	pRootBone->Load_Bone(hFile, dwByte);
	m_vecEntireBone.push_back(pRootBone);

	FAILED_CHECK_RETURN(Ready_Bones(hFile, dwByte, pRootBone), E_FAIL);

	/* SetUp Mesh Bones */
	for (auto& pMesh : m_vecMesh)
		pMesh->SetUp_MeshBones(hFile, dwByte, this);

	/* Animations */
	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	m_vecAnimation.reserve(m_iNumAnimations);

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*	pAnimation = CAnimation::Create(nullptr, this);
		NULL_CHECK_RETURN(pAnimation, E_FAIL);
		FAILED_CHECK_RETURN(pAnimation->Load_Animation(hFile, dwByte), E_FAIL);

		m_vecAnimation.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Save_Model(const char* pSaveFileDirectory)
{
	_tchar		wszSaveFileDirectory[MAX_PATH] = L"";
	CGameUtility::ctwc(pSaveFileDirectory, wszSaveFileDirectory);

	DWORD	dwByte = 0;
	HANDLE	hFile = CreateFileW(wszSaveFileDirectory, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* Meshes */
	WriteFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (auto& pMesh : m_vecMesh)
	{
		if (FAILED(pMesh->Save_Mesh(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Mesh");
			CloseHandle(hFile);
			return E_FAIL;
		}
	}
	
	/* Materials*/
	if (m_iNumMaterials == 0)
		return E_FAIL;

	WriteFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (auto& tMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			if (tMaterial.pTexture[i] == nullptr)
			{
				_uint		iTemp = AI_TEXTURE_TYPE_MAX;
				WriteFile(hFile, &iTemp, sizeof(_uint), &dwByte, nullptr);
			}
			else
			{
				WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);

				const _tchar*	wszFilePath = tMaterial.pTexture[i]->Get_FilePath().c_str();
				_uint				iFilePathLength = (_uint)tMaterial.pTexture[i]->Get_FilePath().length() + 1;

				WriteFile(hFile, &iFilePathLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, wszFilePath, sizeof(_tchar) * iFilePathLength, &dwByte, nullptr);
			}
		}
	}

	/* 파일 위치 포인터 받아서 저장하기 */
	m_dwBeginBoneData = SetFilePointer(hFile, 0, nullptr, FILE_CURRENT);

	/* Bones */
	WriteFile(hFile, &m_dwBeginBoneData, sizeof(DWORD), &dwByte, nullptr);

	WriteFile(hFile, &m_iNumEntireBone, sizeof(_uint), &dwByte, nullptr);
	for (auto& pBone : m_vecEntireBone)
	{
		if (FAILED(pBone->Save_Bone(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Bone");
			CloseHandle(hFile);
			return E_FAIL;
		}
	}

	/* Mesh Bones */
	for (auto& pMesh : m_vecMesh)
		pMesh->Save_MeshBones(hFile, dwByte);

	/* Animations */
	WriteFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (auto& pAnimation : m_vecAnimation)
	{
		if (FAILED(pAnimation->Save_Animation(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Animation");
			CloseHandle(hFile);
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

CMesh * CModel::Get_Mesh(const string & strMeshName)
{
	CMesh*		pFindMesh = nullptr;

	for (auto& pMesh : m_vecMesh)
	{
		if (pMesh->Get_MeshName() == strMeshName)
		{
			pFindMesh = pMesh;
			break;
		}
	}

	return pFindMesh;
}

CModel * CModel::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MODELTYPE eType, const char * pModelFilePath, _fmatrix matPivot)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, matPivot)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(CGameObject * pOwner, void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
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

	for (auto& ModelMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(ModelMaterial.pTexture[i]);
	}
	m_vecMaterial.clear();

	for (auto& pBone : m_vecEntireBone)
		Safe_Release(pBone);
	m_vecEntireBone.clear();

	for (auto& pAnimation : m_vecAnimation)
		Safe_Release(pAnimation);
	m_vecAnimation.clear();
	
	m_Importer.FreeScene();
}
