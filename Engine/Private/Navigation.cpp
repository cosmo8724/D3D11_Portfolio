#include "stdafx.h"
#include "..\Public\Navigation.h"
#include "Cell.h"

CNavigation::CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
{
}

HRESULT CNavigation::Initialize_Prototype(const wstring & wstrFilePath)
{


	return S_OK;
}

HRESULT CNavigation::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);
	
	return S_OK;
}

CNavigation * CNavigation::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath)))
	{
		MSG_BOX("Failed to Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(CGameObject * pOwner, void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_vecCell)
		Safe_Release(pCell);
	m_vecCell.clear();
}
