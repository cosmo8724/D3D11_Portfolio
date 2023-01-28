#include "stdafx.h"
#include "..\Public\MonsterDrink_Frame.h"
#include "GameInstance.h"
#include "GameUtility.h"

CMonsterDrink_Frame::CMonsterDrink_Frame(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CMonsterDrink_Frame::CMonsterDrink_Frame(const CMonsterDrink_Frame & rhs)
	: CUI(rhs)
	, m_eColor(rhs.m_eColor)
{
}

HRESULT CMonsterDrink_Frame::Initialize_Prototype(MONSTERCOLOR eColor)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_eColor = eColor;

	return S_OK;
}

HRESULT CMonsterDrink_Frame::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_BLACK)
		m_wstrUITag = L"UI_MonsterDrink_Black";
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_ORANGE)
		m_wstrUITag = L"UI_MonsterDrink_Orange";
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_PINK)
		m_wstrUITag = L"UI_MonsterDrink_Pink";
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_WHITE)
		m_wstrUITag = L"UI_MonsterDrink_White";

	m_fSizeX = 666.f * 0.5 * 0.2f;
	m_fSizeY = 1600.f * 0.5 * 0.2f;
	m_fX = (_float)g_iWinSizeX * 0.3f;
	m_fY = (_float)g_iWinSizeY * 0.4f - ((_float)m_eColor * g_iWinSizeY * 0.058f);

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.dSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &GameObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));

	return S_OK;
}

void CMonsterDrink_Frame::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

 	if (g_bNeedResizeSwapChain)
	{
		m_fX = -(_float)g_iWinSizeX * 0.3f;
		m_fY = (_float)g_iWinSizeY * 0.4f - ((_float)m_eColor * g_iWinSizeY * 0.044f);

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
 
		m_Rect = { _long(((g_iWinSizeX * 0.5f) + m_fX) - m_fSizeY * 0.5f), _long(((g_iWinSizeY * 0.5f) - m_fY) - m_fSizeX * 0.5f),
			_long(((g_iWinSizeX * 0.5f) + m_fX) + m_fSizeY * 0.5f), _long(((g_iWinSizeY * 0.5f) - m_fY) + m_fSizeX * 0.5f) };
 	} 	if (CGameUtility::Rect_Picking(g_hWnd, m_Rect))
 	{ 
		m_bInitPos = false;
		//m_iComebackMoveCnt = 0;

		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
		_vector	vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);;

		vPos += XMVector3Normalize(vUp) * (_float)m_pTransformCom->Get_TransformDesc().dSpeedPerSec * _float(m_iMaxMoveCnt - m_iCurMoveCnt) / (_float)m_iMaxMoveCnt;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);

		if (m_iLastMoveCnt < m_iCurMoveCnt)
			m_iLastMoveCnt = m_iCurMoveCnt;

		if (m_iCurMoveCnt < m_iMaxMoveCnt)
			m_iCurMoveCnt++;

		if (m_iComebackMoveCnt != 0)
			m_iComebackMoveCnt--;

		return;
 	}
 	else
 	{ 
		if (m_bInitPos == false)
		{
			if (m_iComebackMoveCnt == m_iLastMoveCnt)
			{
				m_bInitPos = true;
				m_iCurMoveCnt = 0;
				m_iComebackMoveCnt = 0;
				m_iLastMoveCnt = 0;
			}

			if (m_iLastMoveCnt != 0)
			{
				int a = 0;
				m_iCurMoveCnt--;
				//m_iLastMoveCnt = 0;
			}

			_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
			_vector	vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);;

			vPos -= XMVector3Normalize(vUp) * (_float)m_pTransformCom->Get_TransformDesc().dSpeedPerSec * _float(m_iMaxMoveCnt - m_iComebackMoveCnt) / (_float)m_iMaxMoveCnt;

			m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);

			if (m_iComebackMoveCnt < m_iLastMoveCnt)
			{
				m_iComebackMoveCnt++;
				//m_iCurMoveCnt--;
			}
		}
 	}
}

void CMonsterDrink_Frame::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMonsterDrink_Frame::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin((_uint)m_eColor + 2);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMonsterDrink_Frame::ImGui_RenderProperty()
{
	ImGui::InputInt("Current Count", &m_iCurMoveCnt, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Comeback Count", &m_iComebackMoveCnt, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Last Count", &m_iLastMoveCnt, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Max Count", &m_iMaxMoveCnt, 0, 0, ImGuiInputTextFlags_ReadOnly);
}

HRESULT CMonsterDrink_Frame::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Frame", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CMonsterDrink_Frame::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CMonsterDrink_Frame * CMonsterDrink_Frame::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MONSTERCOLOR eColor)
{
	CMonsterDrink_Frame*	pInstance = new CMonsterDrink_Frame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColor)))
	{
		MSG_BOX("Failed to Create : CMonsterDrink_Frame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterDrink_Frame::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CMonsterDrink_Frame*	pInstance = new CMonsterDrink_Frame(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CMonsterDrink_Frame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterDrink_Frame::Free()
{
	__super::Free();
}
