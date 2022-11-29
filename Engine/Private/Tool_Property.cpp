#include "..\Public\Tool_Property.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "LevelMgr.h"

HRESULT CTool_Property::Initialize(void * pArg)
{
	m_szTabName = "Property Editor"; 

	return S_OK;
}

void CTool_Property::ImGui_RenderTap()
{
	CObjectMgr::GetInstance()->ImGui_ObjectViewer(CLevelMgr::GetInstance()->Get_CurLevelIndex(), m_pSelectedObject);

	if (m_pSelectedObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pSelectedObject).name());
		
		m_pSelectedObject->ImGui_RenderProperty();
		m_pSelectedObject->ImGui_RenderComponentProperties();
	}
}

void CTool_Property::ImGui_RenderWindow()
{
	assert("Don't Use as Window.");
}

CTool_Property * CTool_Property::Create(void * pArg)
{
	CTool_Property*		pInstance = new CTool_Property;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Property");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Property::Free()
{
}