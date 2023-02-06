#include "stdafx.h"
#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "GameUtility.h"

const wstring CGameObject::m_wstrTransformComTag = L"Com_Transform";

CGameObject::CGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	ZeroMemory(&m_tStatus, sizeof(STATUS));
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bIsClone(true)
	, m_bHasModel(rhs.m_bHasModel)
	, m_tStatus(rhs.m_tStatus)
	, m_bDead(rhs.m_bDead)
	, m_bSpecialAttack(rhs.m_bSpecialAttack)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_Component(const wstring & wstrComponentTag)
{
	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(wstrComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_wstrPrototypeGameObjectTag = wstrPrototypeTag;

	GAMEOBJECTDESC	GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	if (nullptr != pArg)
		GameObjectDesc = *(GAMEOBJECTDESC*)pArg;

	FAILED_CHECK_RETURN(Add_Component(CGameInstance::Get_StaticLevelIndex(), CGameInstance::m_wstrPrototypeTransformTag, m_wstrTransformComTag, (CComponent**)&m_pTransformCom, this, &GameObjectDesc.TransformDesc), E_FAIL);

	return S_OK;
}

void CGameObject::Tick(_double dTimeDelta)
{
}

void CGameObject::Late_Tick(_double dTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Render_ShadowDepth()
{
	return S_OK;
}

HRESULT CGameObject::Render_Reflect()
{
	return S_OK;
}

void CGameObject::ImGui_RenderComponentProperties()
{
	ImGui::Separator();
	ImGui::BulletText("Components");

	for (const auto& Pair : m_mapComponent)
	{
		char szName[MAX_PATH];
		CGameUtility::wctc(Pair.first.c_str(), szName);

		if (ImGui::CollapsingHeader(szName))
			Pair.second->ImGui_RenderProperty();
	}
}

void CGameObject::ImGui_RenderProperty()
{
	ImGui::Separator();
	ImGui::BulletText("Status");
	ImGui::InputFloat2("HP / MaxHP", (_float*)&_float2((_float)m_tStatus.iHP, (_float)m_tStatus.iMaxHP), 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Attack", (_float*)&m_tStatus.iAttack, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Special Attack", (_float*)&m_tStatus.iSpecialAttack, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat2("Attack Cool Time", (_float*)&_float2((_float)m_tStatus.dCurAttackCoolTime, (_float)m_tStatus.dInitAttackCoolTime), 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat2("Hit Cool Time", (_float*)&_float2((_float)m_tStatus.dCurHitCoolTime, (_float)m_tStatus.dInitHitCoolTime), 0, ImGuiInputTextFlags_ReadOnly);
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & wstrPrototypeTag, const wstring & wstrComponentTag, CComponent ** ppComponentOut, CGameObject* pOwner, void * pArg)
{
	if (nullptr != Find_Component(wstrComponentTag))
		return E_FAIL;

	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, wstrPrototypeTag, pOwner, pArg);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent.emplace(wstrComponentTag, pComponent);
	Safe_AddRef(pComponent);

	*ppComponentOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const wstring & wstrComponentTag)
{
	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(wstrComponentTag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	if (m_pTransformCom != nullptr)
		Safe_Release(m_pTransformCom);

	for (auto& Pair : m_mapComponent)
		Safe_Release(Pair.second);
	m_mapComponent.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
