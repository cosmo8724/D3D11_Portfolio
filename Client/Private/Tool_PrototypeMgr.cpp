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
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{

		}

		if (bAddPrototype)
		{
			ImGui::Separator();

			iSelectComponent = -1;
			static	COMPONENTTYPE	eComponentType = COM_RENDERER;
			static	COMPONENTTYPE	eLastComponentType = eComponentType;
			char*	szTypeName[COMPONENTTYPE_END] = { "Renderer", "VIBuffer", "Shader", "Transform", "Texture", "Model" };
			static _uint	iTargetLevel = 3;
			static _uint	iLastTargetLevel = iTargetLevel;

			IMGUI_LEFT_LABEL(ImGui::Combo, "Select Type ", (_int*)&eComponentType, szTypeName, COMPONENTTYPE_END);
			IMGUI_LEFT_LABEL(ImGui::Combo, "Target Level", (_int*)&iTargetLevel, m_pLevelName, LEVEL_END + 1);

			ImGui::NewLine();
			ImGui::BulletText("Options");

			/* For All Component */
			static char	szPrototypeTag[MAX_PATH] = "";
			static char	szFilePath[MAX_PATH] = "";
			const char*	Filter = "Image files (*.png *.dds *.jpg *.jpeg *.gif *.tga){.png,.dds,.jpg,.jpeg,.gif,.tga}";
			
			/* For Shader */
			static _int	iSelectDeclaration = 0;
			char*			szDeclarationType[4] = { "VTXTEX", "VTXNORTEX", "VTXMODEL", "VTXANIMMODEL" };
			static _int	iNumElements = 0;

			/* For Texture */
			static _uint	iTextureCnt = 1;

			/* For Model */
			static CModel::MODELTYPE		eModelType = CModel::MODEL_NONANIM;
			char*			szModelType[2] = { "Non-Animation", "Animation" };

			if (iTargetLevel != iLastTargetLevel || eComponentType != eLastComponentType)
			{
				iLastTargetLevel = iTargetLevel;
				eLastComponentType = eComponentType;

				strcpy_s(szPrototypeTag, "");
				strcpy_s(szFilePath, "");
				iSelectDeclaration = 0;
				iNumElements = 0;
				iTextureCnt = 1;
				eModelType = CModel::MODEL_NONANIM;
			}

			switch (eComponentType)
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
				ImGui::Text("There is no Options for this Comopnent Type.");
				break;

			case COM_TEXTURE:
				/* Input Prototype Tag */
				IMGUI_LEFT_LABEL(ImGui::InputText, "Component Tag", szPrototypeTag, sizeof(char) * MAX_PATH);

				/* Imput Texture Count */
				IMGUI_LEFT_LABEL(ImGui::InputInt, "Texture Count", (_int*)&iTextureCnt, 0, 0);

				/* Input File Path */
				IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "File Path", "Input Path here or Choose from FildDialog.", szFilePath, sizeof(char) * MAX_PATH);
				ImGui::SameLine();
				if (ImGui::SmallButton("Open"))
					ImGuiFileDialog::Instance()->OpenDialog("Choose Image", "Choose Image", Filter, "../Bin/Resource", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

				if (ImGuiFileDialog::Instance()->Display("Choose Image"))
				{
					if (ImGuiFileDialog::Instance()->IsOk())
					{
						string	strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
						char	szDrive[128] = "";
						char	szDir[MAX_PATH] = "";
						char	szFileName[MAX_PATH] = "";
						char	szExt[32] = "";
						char	szNewPath[MAX_PATH] = "";
						_splitpath_s(strFilePath.c_str(), szDrive, 128, szDir, MAX_PATH, szFileName, MAX_PATH, szExt, 32);
						CGameUtility::ConvertNumberTo_d(szFileName);
						_makepath_s(szNewPath, szDrive, szDir, szFileName, szExt);

						char	szRelativePath[MAX_PATH] = "";
						char	szDirectoryPath[MAX_PATH] = "";
						GetCurrentDirectoryA(MAX_PATH, szDirectoryPath);
						PathRelativePathToA(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY, szNewPath, FILE_ATTRIBUTE_DIRECTORY);
						strcpy_s(szFilePath, sizeof(char) * MAX_PATH, szRelativePath);

						ImGuiFileDialog::Instance()->Close();
					}
					if (!ImGuiFileDialog::Instance()->IsOk())
						ImGuiFileDialog::Instance()->Close();
				}
				break;

			case COM_MODEL:
				/* Input Prototype Tag */
				IMGUI_LEFT_LABEL(ImGui::InputText, "Component Tag", szPrototypeTag, sizeof(char) * MAX_PATH);

				/* Input Model Type */
				IMGUI_LEFT_LABEL(ImGui::Combo, "Model Type", (_int*)&eModelType, szModelType, 2);

				/* Input File Path */
				IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "File Path", "Input Path here or Choose from FildDialog.", szFilePath, sizeof(char) * MAX_PATH);
				ImGui::SameLine();
				if (ImGui::SmallButton("Open"))
					ImGuiFileDialog::Instance()->OpenDialog("Choose Model", "Choose Model", ".fbx", "../Bin/Resource/Mesh", ".fbx", 1, nullptr, ImGuiFileDialogFlags_Modal);

				if (ImGuiFileDialog::Instance()->Display("Choose Model"))
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
			}
			
			if (ImGui::Button("Create"))
			{
				for (size_t i = 0; i < m_mapProtoComponenets[iSelectLevel].size(); ++i)
					Safe_Delete_Array(ppProtoComponentTag[i]);
				Safe_Delete_Array(ppProtoComponentTag);

				switch (eComponentType)
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
					{
					_tchar		wszPrototypeTag[MAX_PATH] = L"";
					_tchar		wszFilePath[MAX_PATH] = L"";
					CGameUtility::ctwc(szPrototypeTag, wszPrototypeTag);
					CGameUtility::ctwc(szFilePath, wszFilePath);

					CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CTexture::Create(m_pDevice, m_pContext, wstring(wszFilePath), iTextureCnt));

					break;
					}
				case COM_MODEL:
					{
					_tchar		wszPrototypeTag[MAX_PATH] = L"";
					CGameUtility::ctwc(szPrototypeTag, wszPrototypeTag);

					CGameInstance::GetInstance()->Add_Prototype(iTargetLevel, wstring(wszPrototypeTag), CModel::Create(m_pDevice, m_pContext, eModelType, szFilePath));

					break;
					}
				}
				ImGui::EndTabItem();

				bAddPrototype = false;
				strcpy_s(szPrototypeTag, "");
				strcpy_s(szFilePath, "");
				iSelectDeclaration = 0;
				iNumElements = 0;
				iTextureCnt = 1;
				eModelType = CModel::MODEL_NONANIM;

				return;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				bAddPrototype = false;
				strcpy_s(szPrototypeTag, "");
				strcpy_s(szFilePath, "");
				iSelectDeclaration = 0;
				iNumElements = 0;
				iTextureCnt = 1;
				eModelType = CModel::MODEL_NONANIM;
			}
			ImGui::Separator();
		}

		if (iSelectComponent != -1)
		{
			CComponent*		pComponent = nullptr;

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
				pComponent->ImGui_RenderProperty();


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
	_int	iSelectObject = 1000;
	if (ImGui::BeginTabItem("GameObject"))
	{
		m_iProtoObjCnt = (_uint)m_mapProtoObjects->size();
		ImGui::BulletText("Prototype GameObject List");

		char**			ppProtoObjTag = new char*[m_iProtoObjCnt];
		_uint i = 0;
		for (auto Pair : *m_mapProtoObjects)
		{
			ppProtoObjTag[i] = new char[Pair.first.length() + 1];
			CGameUtility::wctc(Pair.first.c_str(), ppProtoObjTag[i]);
			++i;
		}

		ImGui::ListBox(" ", &iSelectObject, ppProtoObjTag, (_int)m_iProtoObjCnt);

		ImGui::Separator();
		ImGui::BulletText("Add Prototype GameObject");

		static char	szPrototypeTag[MAX_PATH] = "";
		char***		ppComponentTag = nullptr;
		_uint			iComponentCnt[COMPONENTTYPE_END];
		SortComponentByType(ppComponentTag, iComponentCnt);
		static _int	iSelectRender = 0, iSelectVIBuffer = 0, iSelectShader = 0, iSelectTransform = 0, iSelectModel = 0;

		static _int	iTextureComCnt = 1;
		static _int	iLastTextureComCnt = iTextureComCnt;
		static _int*	iSelectTextureCom = new _int[iTextureComCnt];

		if (iTextureComCnt != iLastTextureComCnt)
		{
			_int*		iTemp = new _int[iTextureComCnt];
			ZeroMemory(iTemp, sizeof(_int) * iTextureComCnt);

			if (iTextureComCnt < iLastTextureComCnt)
				memcpy(iTemp, iSelectTextureCom, sizeof(_int) * iTextureComCnt);
			else
				memcpy(iTemp, iSelectTextureCom, sizeof(_int) * iLastTextureComCnt);
			Safe_Delete_Array(iSelectTextureCom);

			iSelectTextureCom = iTemp;
		}

		ImGui::BulletText("Object Tag");
		IMGUI_LEFT_LABEL(ImGui::InputText, "Object Tag", szPrototypeTag, sizeof(char) * MAX_PATH);
		

		ImGui::NewLine();
		ImGui::BulletText("Select Component");

		ImGui::Combo("Renderer", &iSelectRender, *ppComponentTag[COM_RENDERER], iComponentCnt[COM_RENDERER]);
		ImGui::Combo("VIBuffer", &iSelectVIBuffer, *ppComponentTag[COM_VIBUFFER], iComponentCnt[COM_VIBUFFER]);
		ImGui::Combo("Shader", &iSelectShader, *ppComponentTag[COM_SHADER], iComponentCnt[COM_SHADER]);
		ImGui::Combo("Transform", &iSelectTransform, *ppComponentTag[COM_TRANSFORM], iComponentCnt[COM_TRANSFORM]);
		ImGui::InputInt("Texture Component Count", &iTextureComCnt, 0, 0);
		for (_uint i = 0; i < iTextureComCnt; ++i)
		{
			char	szTextureLabel[32] = "";
			sprintf(szTextureLabel, "Texture%d", i);
			const char*	pTextureLabel = szTextureLabel;
			ImGui::Combo( pTextureLabel, &iSelectTextureCom[i], *ppComponentTag[COM_TEXTURE], iComponentCnt[COM_TEXTURE]);
		}
		ImGui::Combo("Model", &iSelectModel, *ppComponentTag[COM_MODEL], iComponentCnt[COM_MODEL]);
		
		if (ImGui::Button("Create"))
		{
			for (size_t i = 0; i < m_iProtoObjCnt; ++i)
				Safe_Delete_Array(ppProtoObjTag[i]);
			Safe_Delete_Array(ppProtoObjTag);

			for (_uint i = 0; i < (_uint)COMPONENTTYPE_END; ++i)
			{
				for (_uint j = 0; j < iComponentCnt[i]; ++j)
					Safe_Delete_Array(ppComponentTag[i][j]);
				Safe_Delete_Array(ppComponentTag[i]);
			}
			Safe_Delete_Array(ppComponentTag);

			ImGui::EndTabItem();

			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{

		}

		for (size_t i = 0; i < m_iProtoObjCnt; ++i)
			Safe_Delete_Array(ppProtoObjTag[i]);
		Safe_Delete_Array(ppProtoObjTag);

		for (_uint i = 0; i < (_uint)COMPONENTTYPE_END; ++i)
		{
			for (_uint j = 0; j < iComponentCnt[i]; ++j)
				Safe_Delete_Array(ppComponentTag[i][j]);
			Safe_Delete_Array(ppComponentTag[i]);
		}
		Safe_Delete_Array(ppComponentTag);

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

void CTool_PrototypeMgr::SortComponentByType(char ***& ppComponentTag, _uint* pComponentCnt)
{
	if (ppComponentTag != nullptr)
		return;

	_uint ComponentCnt[COMPONENTTYPE_END] = { 0, 0, 0, 0, 0, 0 };

	for (_uint i = 0; i < LEVEL_END + 1; ++i)
	{
		for (auto& Pair : m_mapProtoComponenets[i])
		{
			NULL_CHECK_RETURN(Pair.second, );

			if (dynamic_cast<CRenderer*>(Pair.second))
			{
				ComponentCnt[COM_RENDERER]++;
				continue;
			}
			if (dynamic_cast<CVIBuffer*>(Pair.second))
			{
				ComponentCnt[COM_VIBUFFER]++;
				continue;
			}
			if (dynamic_cast<CShader*>(Pair.second))
			{
				ComponentCnt[COM_SHADER]++;
				continue;
			}
			if (dynamic_cast<CTransform*>(Pair.second))
			{
				ComponentCnt[COM_TRANSFORM]++;
				continue;
			}
			if (dynamic_cast<CTexture*>(Pair.second))
			{
				ComponentCnt[COM_TEXTURE]++;
				continue;
			}
			if (dynamic_cast<CModel*>(Pair.second))
				ComponentCnt[COM_MODEL]++;
		}
	}

	ppComponentTag = new char**[COMPONENTTYPE_END];
	for (_int i = 0; i < (_int)COMPONENTTYPE_END; ++i)
		ppComponentTag[i] = new char*[ComponentCnt[i]];

	char	szPrototypeTag[MAX_PATH] = "";
	_uint	ComCnt[COMPONENTTYPE_END];
	ZeroMemory(ComCnt, sizeof(_uint) * COMPONENTTYPE_END);

	for (_uint i = 0; i < LEVEL_END + 1; ++i)
	{
		for (auto& Pair : m_mapProtoComponenets[i])
		{
			NULL_CHECK_RETURN(Pair.second, );
			
			ZeroMemory(szPrototypeTag, sizeof(char) * MAX_PATH);
			CGameUtility::wctc(Pair.first.c_str(), szPrototypeTag);
			_int	iLength = (_int)strlen(szPrototypeTag) + 1;

			if (dynamic_cast<CRenderer*>(Pair.second))
			{ 
				ppComponentTag[COM_RENDERER][ComCnt[COM_RENDERER]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_RENDERER][ComCnt[COM_RENDERER]], iLength);
				sprintf_s(ppComponentTag[COM_RENDERER][ComCnt[COM_RENDERER]], iLength, szPrototypeTag);
				ComCnt[COM_RENDERER]++;
				continue;
			}
			if (dynamic_cast<CVIBuffer*>(Pair.second))
			{
				ppComponentTag[COM_VIBUFFER][ComCnt[COM_VIBUFFER]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_VIBUFFER][ComCnt[COM_VIBUFFER]], iLength);
				sprintf_s(ppComponentTag[COM_VIBUFFER][ComCnt[COM_VIBUFFER]], iLength, szPrototypeTag);
				ComCnt[COM_VIBUFFER]++;
				continue;
			}
			if (dynamic_cast<CShader*>(Pair.second))
			{
				ppComponentTag[COM_SHADER][ComCnt[COM_SHADER]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_SHADER][ComCnt[COM_SHADER]], iLength);
				sprintf_s(ppComponentTag[COM_SHADER][ComCnt[COM_SHADER]], iLength, szPrototypeTag);
				ComCnt[COM_SHADER]++;
				continue;
			}
			if (dynamic_cast<CTransform*>(Pair.second))
			{
				ppComponentTag[COM_TRANSFORM][ComCnt[COM_TRANSFORM]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_TRANSFORM][ComCnt[COM_TRANSFORM]], iLength);
				sprintf_s(ppComponentTag[COM_TRANSFORM][ComCnt[COM_TRANSFORM]], iLength, szPrototypeTag);
				ComCnt[COM_TRANSFORM]++;
				continue;
			}
			if (dynamic_cast<CTexture*>(Pair.second))
			{
				ppComponentTag[COM_TEXTURE][ComCnt[COM_TEXTURE]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_TEXTURE][ComCnt[COM_TEXTURE]], iLength);
				sprintf_s(ppComponentTag[COM_TEXTURE][ComCnt[COM_TEXTURE]], iLength, szPrototypeTag);
				ComCnt[COM_TEXTURE]++;
				continue;
			}
			if (dynamic_cast<CModel*>(Pair.second))
			{
				ppComponentTag[COM_MODEL][ComCnt[COM_MODEL]] = new char[iLength];
				ZeroMemory(ppComponentTag[COM_MODEL][ComCnt[COM_MODEL]], iLength);
				sprintf_s(ppComponentTag[COM_MODEL][ComCnt[COM_MODEL]], iLength, szPrototypeTag);
				ComCnt[COM_MODEL]++;
			}
		}
	}

	for (_int i = 0; i < (_int)COMPONENTTYPE_END; ++i)
		pComponentCnt[i] = ComponentCnt[i];
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