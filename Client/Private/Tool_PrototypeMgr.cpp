#include "stdafx.h"
#include "..\Public\Tool_PrototypeMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "GameUtility.h"

#define	LEVEL_PUBLIC	3

CTool_PrototypeMgr::CTool_PrototypeMgr(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTool_PrototypeMgr::Initialize(void * pArg)
{
	m_szWIndowName = "Prototype Manager";

	m_iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();


	m_mapProtoComponenets = CGameInstance::GetInstance()->Get_PrototypeComponents();
	m_iProtoComponentCnt = (_uint)m_mapProtoComponenets->size();

	m_mapProtoObjects = CGameInstance::GetInstance()->Get_Prototypes();
	m_iProtoObjCnt = (_uint)m_mapProtoObjects->size();

	return S_OK;
}

void CTool_PrototypeMgr::ImGui_RenderWindow()
{
	ImGui::BeginTabBar("Prototype Manager");
	Component_Editor();
	GameObject_Editor();
	ImGui::EndTabBar();
}

void CTool_PrototypeMgr::Component_Editor()
{
	static _int	iSelectLevel = LEVEL_PUBLIC;
	static _int	iSelectComponent = -1;
	static _bool	bAddPrototype = false;

	//char**			ppLevelTags = new char*[m_iProtoComponentCnt];
	
	if (ImGui::BeginTabItem("Component"))
	{
		ImGui::BulletText("Prototype Component List");
		IMGUI_LEFT_LABEL(ImGui::Combo, "Select Level", &iSelectLevel, m_pLevelName, LEVEL_PUBLIC + 1);

		char**			ppProtoComponentTag = new char*[m_mapProtoComponenets[iSelectLevel].size()];
		_uint i = 0;
		for (auto Pair : m_mapProtoComponenets[iSelectLevel])
		{
			ppProtoComponentTag[i] = new char[Pair.first.length() + 1];
			CGameUtility::wctc(Pair.first.c_str(), ppProtoComponentTag[i]);
			++i;
		}

		ImGui::ListBox(" ", &iSelectComponent, ppProtoComponentTag, (_int)m_mapProtoComponenets[iSelectLevel].size());

		if (ImGui::Button("Add"))
			bAddPrototype = true;

		if (bAddPrototype)
		{
			ImGui::Separator();

			iSelectComponent = -1;
			static	COMPONENTTYPE	eType = COM_RENDERER;
			char*	szTypeName[COMPONENTTYPE_END] = { "Renderer", "VIBuffer", "Shader", "Transform", "Texture", "Model" };
			static _uint	iTargetLevel = 0;

			IMGUI_LEFT_LABEL(ImGui::Combo, "Select Type ", (_int*)&eType, szTypeName, COMPONENTTYPE_END);
			IMGUI_LEFT_LABEL(ImGui::Combo, "Target Level", (_int*)&iTargetLevel, m_pLevelName, LEVEL_END + 1);

			ImGui::NewLine();
			ImGui::BulletText("Options");

			static char	szPrototypeTag[MAX_PATH] = "";
			static char	szFilePath[MAX_PATH] = "";
			static _int	iSelectDeclaration = 0;
			char*			szDeclarationType[4] = { "VTXTEX", "VTXNORTEX", "VTXMODEL", "VTXANIMMODEL" };
			static _int	iNumElements = 0;

			switch (eType)
			{
			case COM_RENDERER:
				ImGui::Text("There is no Options for this Comopnent Type.");
				break;

			case COM_VIBUFFER:
				ImGui::Text("There is no Options for this Comopnent Type.");
				break;

			case COM_SHADER:
				/* Input Prototype Tag */
				IMGUI_LEFT_LABEL(ImGui::InputText, "Component Tag", szPrototypeTag, sizeof(char) * MAX_PATH);

				/* Input File Path */
				IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "File Path", "Input Path here or Choose from FildDialog.", szFilePath, sizeof(char) * MAX_PATH);
				ImGui::SameLine();
				if (ImGui::SmallButton("Open"))
					ImGuiFileDialog::Instance()->OpenDialog("Choose HLSL", "Choose .hlsl", ".hlsl", "../Bin/Shader", ".hlsl", 1, nullptr, ImGuiFileDialogFlags_Modal);
				
				/* Select Declaration Type */
				IMGUI_LEFT_LABEL(ImGui::Combo, "Select Declaration", &iSelectDeclaration, szDeclarationType, 4);

				if (iSelectDeclaration == 0)
					iNumElements = 2;
				else if (iSelectDeclaration == 1)
					iNumElements = 3;
				else if (iSelectDeclaration == 2)
					iNumElements = 4;
				else if (iSelectDeclaration == 3)
					iNumElements = 6;

				IMGUI_LEFT_LABEL(ImGui::InputInt, "Emelents Count", &iNumElements, 0, 0, ImGuiInputTextFlags_ReadOnly);

				if (ImGuiFileDialog::Instance()->Display("Choose HLSL"))
				{
					if (ImGuiFileDialog::Instance()->IsOk())
					{
						char	szRelativePath[MAX_PATH] = "";
						char	szDirectoryPath[MAX_PATH] = "";
						GetCurrentDirectoryA(MAX_PATH, szDirectoryPath);
						PathRelativePathToA(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY, ImGuiFileDialog::Instance()->GetFilePathName().c_str(), FILE_ATTRIBUTE_DIRECTORY);
						strcpy_s(szFilePath, sizeof(char) * MAX_PATH, szRelativePath);

						ImGuiFileDialog::Instance()->Close();
					}
					if (!ImGuiFileDialog::Instance()->IsOk())
						ImGuiFileDialog::Instance()->Close();
				}
				break;

			case COM_TRANSFORM:
				ImGui::Text("I am %d", COM_TRANSFORM);
				break;

			case COM_TEXTURE:
				ImGui::Text("I am %d", COM_TEXTURE);
				break;

			case COM_MODEL:
				ImGui::Text("I am %d", COM_MODEL);
				break;
			}
			
			if (ImGui::Button("Create"))
			{
				for (size_t i = 0; i < m_mapProtoComponenets[iSelectLevel].size(); ++i)
					Safe_Delete_Array(ppProtoComponentTag[i]);
				Safe_Delete_Array(ppProtoComponentTag);

				switch (eType)
				{
				case COM_RENDERER:
					break;

				case COM_VIBUFFER:
					break;

				case COM_SHADER:
				{
					_tchar		wszPrototypeTag[MAX_PATH] = L"";
					_tchar		wszFilePath[MAX_PATH] = L"";
					CGameUtility::ctwc(szPrototypeTag, wszPrototypeTag);
					CGameUtility::ctwc(szFilePath, wszFilePath);

					if (iSelectDeclaration == 0)
						CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CShader::Create(m_pDevice, m_pContext, wstring(wszFilePath), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements));
					else if (iSelectDeclaration == 1)
						CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CShader::Create(m_pDevice, m_pContext, wstring(wszFilePath), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements));
					else if (iSelectDeclaration == 2)
						CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CShader::Create(m_pDevice, m_pContext, wstring(wszFilePath), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements));
					else if (iSelectDeclaration == 3)
						CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CShader::Create(m_pDevice, m_pContext, wstring(wszFilePath), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements));

					break;
				}
				case COM_TRANSFORM:
					ImGui::Text("I am %d", COM_TRANSFORM);
					break;

				case COM_TEXTURE:
					ImGui::Text("I am %d", COM_TEXTURE);
					break;

				case COM_MODEL:
					ImGui::Text("I am %d", COM_MODEL);
					break;
				}
				ImGui::EndTabItem();
				bAddPrototype = false;
				return;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				bAddPrototype = false;
			}
			ImGui::Separator();
		}

		if (iSelectComponent != -1)
		{
			/*CComponent*		pComponent = nullptr;

			for (auto Pair : m_mapProtoComponenets[iSelectLevel])
			{
				_tchar		wszComponentTag[MAX_PATH] = L"";
				CGameUtility::ctwc(ppProtoComponentTag[iSelectComponent], wszComponentTag);
				if (!lstrcmp(Pair.first.c_str(), wszComponentTag))
				{
					pComponent = Pair.second;
					break;
				}
			}

			if (nullptr != pComponent)
				pComponent->ImGui_RenderProperty();*/


			/* 컴포넌트 별로 기능 수행하기 */
			switch (CheckComponentType(iSelectLevel, ppProtoComponentTag[iSelectComponent]))
			{
			case COM_RENDERER:
				ImGui::Text("I am %d", COM_RENDERER);
				break;

			case COM_VIBUFFER:
				ImGui::Text("I am %d", COM_VIBUFFER);
				break;

			case COM_SHADER:
				ImGui::Text("I am %d", COM_SHADER);
				break;

			case COM_TRANSFORM:
				ImGui::Text("I am %d", COM_TRANSFORM);
				break;

			case COM_TEXTURE:
				ImGui::Text("I am %d", COM_TEXTURE);
				break;

			case COM_MODEL:
				ImGui::Text("I am %d", COM_MODEL);
				break;

			case COMPONENTTYPE_END:
				return;
			}
		}

		for (size_t i = 0; i < m_mapProtoComponenets[iSelectLevel].size(); ++i)
			Safe_Delete_Array(ppProtoComponentTag[i]);
		Safe_Delete_Array(ppProtoComponentTag);

		ImGui::EndTabItem();
	}
}

void CTool_PrototypeMgr::GameObject_Editor()
{
	if (ImGui::BeginTabItem("GameObject"))
	{


		ImGui::EndTabItem();
	}
}

CTool_PrototypeMgr::COMPONENTTYPE CTool_PrototypeMgr::CheckComponentType(_int iSelectLevel, const char* pComponentTag)
{
	CComponent*		pComponent = nullptr;

	for (auto Pair : m_mapProtoComponenets[iSelectLevel])
	{
		_tchar		wszComponentTag[MAX_PATH] = L"";
		CGameUtility::ctwc(pComponentTag, wszComponentTag);
		if (!lstrcmp(Pair.first.c_str(), wszComponentTag))
		{
			pComponent = Pair.second;
			break;
		}
	}

	NULL_CHECK_RETURN(pComponent, COMPONENTTYPE_END);

	if (dynamic_cast<CRenderer*>(pComponent))
		return COM_RENDERER;
	if (dynamic_cast<CVIBuffer*>(pComponent))
		return COM_VIBUFFER;
	if (dynamic_cast<CShader*>(pComponent))
		return COM_SHADER;
	if (dynamic_cast<CTransform*>(pComponent))
		return COM_TRANSFORM;
	if (dynamic_cast<CTexture*>(pComponent))
		return COM_TEXTURE;
	if (dynamic_cast<CModel*>(pComponent))
		return COM_MODEL;

	return COMPONENTTYPE_END;
}

CTool_PrototypeMgr * CTool_PrototypeMgr::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void * pArg)
{
	CTool_PrototypeMgr*	pInstance = new CTool_PrototypeMgr(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_PrototypeMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_PrototypeMgr::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}