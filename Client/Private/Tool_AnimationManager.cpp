#include "stdafx.h"
#include "..\Public\Tool_AnimationManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "GameUtility.h"

CTool_AnimationManager::CTool_AnimationManager()
{
}

HRESULT CTool_AnimationManager::Initialize(void * pArg)
{
	m_szWIndowName = "Animation Manager";

	m_vecAnimObjects = CGameInstance::GetInstance()->Get_AnimObjects();
	NULL_CHECK_RETURN(m_vecAnimObjects, E_FAIL);

	return S_OK;
}

void CTool_AnimationManager::ImGui_RenderWindow()
{
	_uint		iObjectCount = (_uint)m_vecAnimObjects->size();

	ImGui::BulletText("Animation Objects");
	
	if (iObjectCount == 0)
	{
		ImGui::Text("There is no Object has Animation Model Component");
		return;
	}

	static _int	iSelectObject = -1;
	char**		ppObjectTag = new char*[iObjectCount];

	for (_uint i = 0; i < iObjectCount; ++i)
	{
		_uint	iTagLength = (_uint)lstrlen((*m_vecAnimObjects)[i]->Get_PrototypeGameObjectTag().c_str()) + 1;
		ppObjectTag[i] = new char[iTagLength];
		CGameUtility::wctc((*m_vecAnimObjects)[i]->Get_PrototypeGameObjectTag().c_str(), ppObjectTag[i]);
	}
	
	ImGui::ListBox("##", &iSelectObject, ppObjectTag, iObjectCount);

	if (iSelectObject != -1)
	{
		CGameObject*	pGameObject = (*m_vecAnimObjects)[iSelectObject];

		CModel*	pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
		if (pModel != nullptr)
		{
			ImGui::Separator();
			pModel->ImGui_RenderAnimation();
		}
	}

	for (_uint i = 0; i < iObjectCount; ++i)
		Safe_Delete_Array(ppObjectTag[i]);
	Safe_Delete_Array(ppObjectTag);
}

CTool_AnimationManager * CTool_AnimationManager::Create(void * pArg)
{
	CTool_AnimationManager*	pInstance = new CTool_AnimationManager;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_AnimationManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_AnimationManager::Free()
{
}