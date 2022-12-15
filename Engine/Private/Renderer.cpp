#include "stdafx.h"
#include "..\Public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	assert(eRenderGroup < RENDERGROUP_END);
	assert(pGameObject != nullptr);

	m_RenderObjectList[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	FAILED_CHECK_RETURN(Render_Priority(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonAlphaBlend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonLight(), E_FAIL);
	FAILED_CHECK_RETURN(Render_AlphaBlend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_UI(), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_UI].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(CGameObject * pOwner, void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();
}
