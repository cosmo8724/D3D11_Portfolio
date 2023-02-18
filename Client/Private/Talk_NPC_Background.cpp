#include "stdafx.h"
#include "..\Public\Talk_NPC_Background.h"
#include "GameInstance.h"
#include "Talk_NPC_NameBox.h"
#include "Talk_NPC_Icon.h"
#include "Talk_Sigrid_Background.h"
#include "NPC.h"

CTalk_NPC_Background::CTalk_NPC_Background(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_NPC_Background::CTalk_NPC_Background(const CTalk_NPC_Background & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_NPC_Background::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Talk_NPC_BackGround";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTalk_NPC_Background::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.5f;
	m_fSizeY = (_float)g_iWinSizeY * 0.15f;
	m_fX = 0.f;
	m_fY = -(_float)g_iWinSizeY * 0.35f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	return S_OK;
}

void CTalk_NPC_Background::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bReserveDead == true && CGameInstance::GetInstance()->Key_Down(DIK_E))
	{
		if (m_iAnswer == 1)
		{
			m_bDead = true;

			if (m_wstrName == L"���� ������")
			{
				g_bReadySceneChange = true;
				CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_SceneChange_1");
				CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_UI_Shop_BackGround");
				CGameInstance::GetInstance()->Play_Sound(L"SFX_UI_BaineShop_ShopOpen.wav", g_fSFXVolume, false, false);
			}
		}
		else if (m_iAnswer == 2)
		{
			m_bDead = true;
		}
		
		m_pSentences->pop_back();
		m_pOwner->Set_Talking(false);
		m_pOwner->Play_Voice();

		return;
	}

	if (m_pNameBox != nullptr && m_pNameBox->Get_Name() == L"")
		m_pNameBox->Set_Name(m_wstrName);

	if (m_pIcon != nullptr && m_pIcon->Get_NPCIndex() == -1)
	{
		if (m_wstrName == L"�ص�")
			m_pIcon->Set_NPCIndex(NPC_ANDY);

		else if (m_wstrName == L"���� ������")
			m_pIcon->Set_NPCIndex(NPC_BAINE);

		else if (m_wstrName == L"ģ���� ����")
			m_pIcon->Set_NPCIndex(NPC_GRANDMA);

		else if (m_wstrName == L"Ŭ���� Ŭ���Ʈ")
			m_pIcon->Set_NPCIndex(NPC_KLEMENS_KLOUT);

		else if (m_wstrName == L"�ڵ����� Ŭ���Ʈ")
			m_pIcon->Set_NPCIndex(NPC_CORDELIA_KLOUT);
	}

	if (g_bNeedResizeSwapChain)
	{
		m_fX = 0.f;
		m_fY = -(_float)g_iWinSizeY * 0.35f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameInstance::GetInstance()->Key_Up(DIK_E) && m_bTalkFinish == false)
	{
		m_iCurSentence++;
		m_pOwner->Play_Voice();

		if (m_iCurSentence >= (_uint)m_pSentences->size())
		{
			m_iCurSentence = (_uint)m_pSentences->size() - 1;
			m_bTalkFinish = true;

			if (m_pSentences->size() > 2)
			{
				CTalk_Sigrid_Background*	pUI = dynamic_cast<CTalk_Sigrid_Background*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_UI_Talk_Sigrid_BackGround"));

				if (m_wstrName == L"�ص�")
					pUI->Set_Sentences(L"���� ����!", L"���߾�! �̻��� ��������!");
				else if (m_wstrName == L"���� ������")
					pUI->Set_Sentences(L"���ƿ�! �� �� ����!", L"���߾�! �̻��� ��������!");
				else if (m_wstrName == L"ģ���� ����")
					pUI->Set_Sentences(L"���ʹ� ��� ����?", L"���߾�! �̻��� �ҸӴϴ�!");
				else if (m_wstrName == L"Ŭ���� Ŭ���Ʈ")
					pUI->Set_Sentences(L"���߾�! �̻��� �Ҿƹ�����!", L"���߾�! �̻��� �Ҿƹ�����!");
				else if (m_wstrName == L"�ڵ����� Ŭ���Ʈ")
					pUI->Set_Sentences(L"�ð��ּ���!", L"���߾�! �̻��� �ҸӴϴ�!");
			}
			else
			{
				m_bReserveDead = true;
				m_iAnswer = 2;
			}
		}
	}

	/*if (CGameInstance::GetInstance()->Key_Up(DIK_R))
	{
		for (auto& pUI : m_vecChild)
			pUI->Set_Dead();

		m_bDead = true;
	}*/

	for (auto& pUI : m_vecChild)
		pUI->Tick(dTimeDelta);
}

void CTalk_NPC_Background::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_iAnswer != -1 && m_bReserveDead == false && m_pSentences->size() > 2)
	{
		if (m_iAnswer == 1)
		{
			if (m_wstrName == L"�ص�")
				m_pSentences->push_back(L"����?! �׷� ���͸� ������ ������ ���ƿ�.\n���ʿ� ���̴� �������� ����ġ�� ���ٰ�...");
			else if (m_wstrName == L"���� ������")
				m_pSentences->push_back(L"���� ����! ������ �ѷ������!");
			else if (m_wstrName == L"ģ���� ����")
				m_pSentences->push_back(L"�� ������ ���̴� ���� �� ���󰡸� �����ٵ�... �������״�.");
			else if (m_wstrName == L"Ŭ���� Ŭ���Ʈ")
				m_pSentences->push_back(L"�� ������ ���̴� ���� �� ���󰡸� �����ٵ�... �������״�.");
			else if (m_wstrName == L"�ڵ����� Ŭ���Ʈ")
				m_pSentences->push_back(L"�� ��Ź��, ���� �ư���.");

			m_pOwner->Set_Quest(true);
		}
		else if (m_iAnswer == 2)
		{
			if (m_wstrName == L"�ص�")
				m_pSentences->push_back(L"��... �ʰ� ���� ���� ���δٴ� �̻��� ���ھֱ���...");
			else if (m_wstrName == L"���� ������")
				m_pSentences->push_back(L"��...��?! ���� �ֵ��� ������...");
			else if (m_wstrName == L"ģ���� ����")
				m_pSentences->push_back(L"����.. ����. ���� �͵��� ����...");
			else if (m_wstrName == L"Ŭ���� Ŭ���Ʈ")
				m_pSentences->push_back(L"����? ���� �Ҿƹ���... ����..");
			else if (m_wstrName == L"�ڵ����� Ŭ���Ʈ")
				m_pSentences->push_back(L"������ �ٲ�� ������ �ٽ� ���� �ɾ��ַ�.");
		}

		m_iCurSentence++;
		m_bReserveDead = true;
		m_pOwner->Play_Voice();
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pUI : m_vecChild)
		pUI->Late_Tick(dTimeDelta);
}

HRESULT CTalk_NPC_Background::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	if (m_pSentences != nullptr)
	{
		_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
		CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", (*m_pSentences)[m_iCurSentence], _float2(vPos.x + 775.f, -vPos.y + 715.f), 0.f, _float2(m_fAspectRatioX, m_fAspectRatioY));
	}

	return S_OK;
}

void CTalk_NPC_Background::ImGui_RenderProperty()
{
	for (auto& pUI : m_vecChild)
		pUI->ImGui_RenderTransformCom();
}

HRESULT CTalk_NPC_Background::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_BackGround", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CTalk_NPC_Background::SetUp_Parts()
{
	CUI*	pUI = nullptr;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_NPC_Corner"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	m_pNameBox = dynamic_cast<CTalk_NPC_NameBox*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_NPC_NameBox"));
	NULL_CHECK_RETURN(m_pNameBox, E_FAIL);
	m_pNameBox->Set_Parent(this);

	m_vecChild.push_back(m_pNameBox);

	m_pIcon = dynamic_cast<CTalk_NPC_Icon*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_NPC_Icon"));
	NULL_CHECK_RETURN(m_pIcon, E_FAIL);
	m_pIcon->Set_Parent(this);

	m_vecChild.push_back(m_pIcon);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_ButtonE"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	return S_OK;
}

HRESULT CTalk_NPC_Background::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CTalk_NPC_Background * CTalk_NPC_Background::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_NPC_Background*	pInstance = new CTalk_NPC_Background(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_NPC_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_NPC_Background::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_NPC_Background*	pInstance = new CTalk_NPC_Background(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_NPC_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_NPC_Background::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);
}
