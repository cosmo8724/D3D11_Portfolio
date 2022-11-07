#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
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