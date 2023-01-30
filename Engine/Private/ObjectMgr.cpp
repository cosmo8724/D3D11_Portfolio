#include "stdafx.h"
#include "..\Public\ObjectMgr.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameUtility.h"

IMPLEMENT_SINGLETON(CObjectMgr)

CObjectMgr::CObjectMgr()
{
}

CComponent * CObjectMgr::Get_Component(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, wstrLayerTag);
	NULL_CHECK_RETURN(pLayer, nullptr);

	return pLayer->Get_Component(wstrComponentTag, iIndex);
}

HRESULT CObjectMgr::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObjectMgr::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObjectMgr::Add_Layer(_uint iLevelIndex, const wstring & wstrLayerTag)
{
	CLayer*			pLayer = Find_Layer(iLevelIndex, wstrLayerTag);

	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);

		m_pLayers[iLevelIndex].emplace(wstrLayerTag, pLayer);
	}

	return S_OK;
}

HRESULT CObjectMgr::Add_Prototype(const wstring & wstrPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(wstrPrototypeTag))
	{
		Safe_Release(pPrototype);
		return S_OK;
	}

	m_mapProtypes.emplace(wstrPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObjectMgr::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	CGameObject*		pPrototypeObject = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototypeObject, E_FAIL);

	CGameObject*		pCloneObject = pPrototypeObject->Clone(wstrPrototypeTag, pArg);

	CLayer*				pLayer = Find_Layer(iLevelIndex, wstrLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		NULL_CHECK_RETURN(pLayer, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), E_FAIL);

		m_pLayers[iLevelIndex].emplace(wstrLayerTag, pLayer);
	}
	else
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), E_FAIL);

	return S_OK;
}

HRESULT CObjectMgr::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, _float4x4 matWorld, void* pArg)
{
	CGameObject*		pPrototypeObject = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototypeObject, E_FAIL);

	CGameObject*		pCloneObject = pPrototypeObject->Clone(wstrPrototypeTag, pArg);
	pCloneObject->Set_WorldMatrix(matWorld);

	CLayer*				pLayer = Find_Layer(iLevelIndex, wstrLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		NULL_CHECK_RETURN(pLayer, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), E_FAIL);

		m_pLayers[iLevelIndex].emplace(wstrLayerTag, pLayer);
	}
	else
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), E_FAIL);

	return S_OK;
}

CGameObject * CObjectMgr::Clone_GameObject(const wstring & wstrPrototypeTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, nullptr);

	CGameObject*	pCloneObject = pPrototype->Clone(wstrPrototypeTag, pArg);
	NULL_CHECK_RETURN(pCloneObject, nullptr);

	return pCloneObject;
}

CGameObject * CObjectMgr::Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, nullptr);

	CGameObject*	pCloneObject = pPrototype->Clone(wstrPrototypeTag, pArg);
	NULL_CHECK_RETURN(pCloneObject, nullptr);

	CLayer*			pLayer = Find_Layer(iLevelIndex, wstrLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		NULL_CHECK_RETURN(pLayer, nullptr);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), nullptr);

		m_pLayers[iLevelIndex].emplace(wstrLayerTag, pLayer);
	}
	else
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), nullptr);

	return pCloneObject;
}

CGameObject * CObjectMgr::Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, _float4x4 matWorld, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, nullptr);

	CGameObject*	pCloneObject = pPrototype->Clone(wstrPrototypeTag, pArg);
	NULL_CHECK_RETURN(pCloneObject, nullptr);
	pCloneObject->Set_WorldMatrix(matWorld);

	CLayer*			pLayer = Find_Layer(iLevelIndex, wstrLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		NULL_CHECK_RETURN(pLayer, nullptr);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), nullptr);

		m_pLayers[iLevelIndex].emplace(wstrLayerTag, pLayer);
	}
	else
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObject), nullptr);

	return pCloneObject;
}

HRESULT CObjectMgr::Add_AnimObject(CGameObject * pAnimObject)
{
	NULL_CHECK_RETURN(pAnimObject, E_FAIL);

	m_vecAnimObjects.push_back(pAnimObject);
	Safe_AddRef(pAnimObject);

	return S_OK;
}

void CObjectMgr::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Tick(dTimeDelta);
		}
	}
}

void CObjectMgr::Late_Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(dTimeDelta);
		}
	}
}

void CObjectMgr::ImGui_ProtoViewer(const wstring & wstrSelectedProto)
{
	return;
}

void CObjectMgr::ImGui_ObjectViewer(_uint iLevel, CGameObject *& pSelectedObejct)
{
	if (m_iNumLevels <= iLevel)
		return;

	_bool	bFound = false;

	const LAYERS&	targetLevel = m_pLayers[iLevel];

	if (ImGui::TreeNode("Object Viewer"))
	{
		for (auto& Pair : targetLevel)
		{
			char szLayerTag[MAX_PATH];
			CGameUtility::wctc(Pair.first.c_str(), szLayerTag);
			if (ImGui::TreeNode(szLayerTag))
			{
				if (ImGui::BeginListBox("##", ImVec2(250.f, 50.f)))
				{
					for (auto& Obj : *Pair.second->GetGameObject())
					{
						const _bool bSelected = pSelectedObejct == Obj;
						if (bSelected)
						{
							ImGui::SetItemDefaultFocus();
							bFound = true;
						}
						if (ImGui::Selectable(typeid(*Obj).name(), bSelected))
						{
							pSelectedObejct = Obj;
							bFound = true;
						}
					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (bFound == false)
		pSelectedObejct = nullptr;
}

CGameObject * CObjectMgr::Find_Prototype(const wstring & wstrPrototypeTag)
{
	PROTOTYPES::iterator	iter = find_if(m_mapProtypes.begin(), m_mapProtypes.end(), CTag_Finder(wstrPrototypeTag));

	if (iter == m_mapProtypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObjectMgr::Find_Layer(_uint iLevelIndex, const wstring & wstrLayerTag)
{
	LAYERS::iterator	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(wstrLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObjectMgr::Free()
{
	for (auto& pAnimObject : m_vecAnimObjects)
		Safe_Release(pAnimObject);
	m_vecAnimObjects.clear();

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_mapProtypes)
		Safe_Release(Pair.second);
	m_mapProtypes.clear();

}
