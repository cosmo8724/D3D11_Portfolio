#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const wstring & wstrComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjectList.size())
		return nullptr;

	auto	iter = m_GameObjectList.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(wstrComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_GameObjectList.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjectList)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(dTimeDelta);
	}
}

void CLayer::Late_Tick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjectList)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(dTimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CLayer");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjectList)
		Safe_Release(pGameObject);

	m_GameObjectList.clear();
}