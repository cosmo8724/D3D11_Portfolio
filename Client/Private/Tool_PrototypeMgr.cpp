#include "stdafx.h"
#include "..\Public\Tool_PrototypeMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "GameUtility.h"

#define	LEVEL_PUBLIC	3

CTool_PrototypeMgr::CTool_PrototypeMgr()
{
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

CTool_PrototypeMgr * CTool_PrototypeMgr::Create(void * pArg)
{
	CTool_PrototypeMgr*	pInstance = new CTool_PrototypeMgr;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_PrototypeMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_PrototypeMgr::Free()
{
}