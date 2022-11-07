#include "..\Public\ObjectMgr.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CObjectMgr)

CObjectMgr::CObjectMgr()
{
}

HRESULT CObjectMgr::Reverse_Manager(_uint iNumLevels)
{
	NULL_CHECK_RETURN(m_pLayers, E_FAIL);

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

HRESULT CObjectMgr::Add_Prototype(const wstring & wstrPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(wstrPrototypeTag))
		return E_FAIL;

	m_mapProtypes.emplace(wstrPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObjectMgr::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	CGameObject*		pPrototypeObject = Find_Prototype(wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototypeObject, E_FAIL);

	CGameObject*		pCloneObject = pPrototypeObject->Clone(pArg);

	CLayer*				pLayer = Find_Layer(wstrLayerTag);

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

CGameObject * CObjectMgr::Find_Prototype(const wstring & wstrPrototypeTag)
{
	PROTOTYPES::iterator	iter = find_if(m_mapProtypes.begin(), m_mapProtypes.end(), CTag_Finder(wstrPrototypeTag));

	if (iter == m_mapProtypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObjectMgr::Find_Layer(const wstring & wstrLayerTag)
{
	LAYERS::iterator	iter = find_if(m_pLayers->begin(), m_pLayers->end(), CTag_Finder(wstrLayerTag));

	if (iter == m_pLayers->end())
		return nullptr;

	return iter->second;
}

void CObjectMgr::Free()
{
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
