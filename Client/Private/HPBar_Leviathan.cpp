#include "stdafx.h"
#include "..\Public\HPBar_Leviathan.h"
#include "GameInstance.h"
#include "Leviathan.h"
#include "Sigrid.h"

CHPBar_Leviathan::CHPBar_Leviathan(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CHPBar_Leviathan::CHPBar_Leviathan(const CHPBar_Leviathan & rhs)
	: CUI(rhs)
{
}

HRESULT CHPBar_Leviathan::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CHPBar_Leviathan::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_wstrUITag = L"HPBar_Boss";

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pLeviathan = dynamic_cast<CLeviathan*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Enemies")->back());
	NULL_CHECK_RETURN(m_pLeviathan, E_FAIL);

	return S_OK;
}

void CHPBar_Leviathan::Tick(_double dTimeDelta)
{
	m_pTransformCom->Set_Scale(_float3(30.f, 30.f, 30.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f));

	_float4	vTemp = m_pLeviathan->Get_Position();
	_float3	vPos = { vTemp.x, vTemp.y + 20.f, vTemp.z };
	vPos += XMVector3Normalize(m_pLeviathan->Get_WorldMatrix().Forward()) * -10.f;

	vTemp = CGameInstance::GetInstance()->Get_CameraPosition();
	_float3	vCamPos = { vTemp.x, vTemp.y, vTemp.z };
	
	_float4x4	matCam = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_CAMWORLD);
	_float3	vCamLook = { -matCam._31, matCam._32, -matCam._33 };

	CSigrid*	pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	vTemp = pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	_float3	vDir = { vTemp.x, 0.f, vTemp.z };
	//_float4x4::CreateBillboard()
	_float4x4		matBillBoard = _float4x4::CreateConstrainedBillboard(vPos, vCamPos, _float3(0.f, 1.f, 0.f), &vCamLook, &vDir);

	_matrix		matRot = XMMatrixRotationY(XMConvertToRadians(180.f));

	matBillBoard.Right(XMVector3TransformNormal(matBillBoard.Right(), matRot) * 30.f);
	matBillBoard.Up(XMVector3TransformNormal(matBillBoard.Up(), matRot) * 30.f);
	matBillBoard.Forward(XMVector3TransformNormal(matBillBoard.Forward(), matRot) * 30.f);

	m_pTransformCom->Set_WorldMatrix(matBillBoard);

	if (m_bReady == false)
	{
		m_fGlobalRatio += (_float)dTimeDelta;

		if (m_fGlobalRatio > 1.f)
		{
			m_fGlobalRatio = 1.f;
			m_bReady = true;
		}
	}

	m_iMaxHP = m_pLeviathan->Get_Status().iMaxHP;
	m_iCurHP = m_pLeviathan->Get_Status().iHP;
	m_fHPRatio = ((_float)m_iCurHP + 27.f) / ((_float)m_iMaxHP + 36.f);
}

void CHPBar_Leviathan::Late_Tick(_double dTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CHPBar_Leviathan::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 12);

	return S_OK;
}

void CHPBar_Leviathan::ImGui_RenderProperty()
{
	ImGui::InputInt("Current HP", &m_iCurHP, 1, 5);
}

HRESULT CHPBar_Leviathan::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_HPBar_Boss_Frame", L"Com_Texture_Frame", (CComponent**)&m_pFrameTexture, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_HPBar_Boss", L"Com_Texture_HP", (CComponent**)&m_pHPTexture, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_HalfCylinder", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CHPBar_Leviathan::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pFrameTexture->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
	m_pHPTexture->Bind_ShaderResource(m_pShaderCom, L"g_HPTexture");
	m_pShaderCom->Set_RawValue(L"g_fHPRatio", &m_fHPRatio, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fGlobalRatio", &m_fGlobalRatio, sizeof(_float));

	Safe_Release(pGameInstance);

	return S_OK;
}

CHPBar_Leviathan * CHPBar_Leviathan::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CHPBar_Leviathan*		pInstance = new CHPBar_Leviathan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHPBar_Leviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHPBar_Leviathan::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CHPBar_Leviathan*		pInstance = new CHPBar_Leviathan(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CHPBar_Leviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHPBar_Leviathan::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pFrameTexture);
	Safe_Release(m_pHPTexture);
}
