#include "..\Public\ComponentMgr.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponentMgr)

CComponentMgr::CComponentMgr()
{
}

HRESULT CComponentMgr::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_mapPrototype)
		return E_FAIL;

	m_mapPrototype = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponentMgr::Add_Prototype(_uint iLevelIndex, const wstring & wstrPrototypeTag, CComponent * pPrototype)
{
	NULL_CHECK_RETURN(m_mapPrototype, E_FAIL);
	assert(iLevelIndex < m_iNumLevels);

	if (nullptr != Find_Prototype(iLevelIndex, wstrPrototypeTag))
		return E_FAIL;

	m_mapPrototype[iLevelIndex].emplace(wstrPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponentMgr::Clone_Component(_uint iLevelIndex, const wstring & wstrPrototypeTag, void * pArg)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, wstrPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, nullptr);

	CComponent*		pCloneComponent = pPrototype->Clone(pArg);
	NULL_CHECK_RETURN(pCloneComponent, nullptr);

	return pCloneComponent;
}

CComponent * CComponentMgr::Find_Prototype(_uint iLevelIndex, const wstring & wstrPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = find_if(m_mapPrototype[iLevelIndex].begin(), m_mapPrototype[iLevelIndex].end(), CTag_Finder(wstrPrototypeTag));
	if (iter == m_mapPrototype[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponentMgr::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_mapPrototype[i])
			Safe_Release(Pair.second);
		
		m_mapPrototype[i].clear();
	}
	Safe_Delete_Array(m_mapPrototype);
}