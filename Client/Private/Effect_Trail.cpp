#include "stdafx.h"
#include "..\Public\Effect_Trail.h"
#include "GameInstance.h"
#include "GameUtility.h"

CEffect_Trail::CEffect_Trail(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Trail::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	return S_OK;
}

void CEffect_Trail::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pVIBufferCom->Tick_Trail(dTimeDelta);

	/*if (m_bActive == false)
		return;

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_vecPrePositions.empty() || m_vecPrePositions.back() != vPos)
		m_vecPrePositions.push_back(vPos);

	if (m_vecPrePositions.size() >= 3)
	{
		while (m_vecPrePositions.size() > 3)
			m_vecPrePositions.erase(m_vecPrePositions.begin());

		_vector	vCamPos = CGameInstance::GetInstance()->Get_CameraPosition();
		_float		fSplineLength = XMVectorGetX(XMVector3Length(m_vecPrePositions[1] - m_vecPrePositions[2]));
		_uint		iSegmentCnt = _uint(fSplineLength / m_fSegmentSize);

		_vector	vPoint0 = m_vecPrePositions[0];
		_vector	vPoint1 = m_vecPrePositions[1];
		_vector	vPoint2 = m_vecPrePositions[2];
		_vector	vPoint3 = m_vecPrePositions[2];
		_float		fPreLife = 0.f;

		if (m_pVIBufferCom->Get_InstanceInfo()->size())
			fPreLife = m_pVIBufferCom->Get_InstanceInfo()->back().vPosition.w;

		_vector	vPrePos = vPoint1;


		_float		fWeight = 0.f, fRadian = 0.f;
		_vector	vSplinePos, vRight, vUp, vLook, vDir;
		for (_uint i = 0; i < iSegmentCnt; ++i)
		{
			fWeight = _float(i + 1) / (_float)iSegmentCnt;
			vSplinePos = XMVectorCatmullRom(vPoint0, vPoint1, vPoint2, vPoint3, fWeight);

			vRight = XMVector3Normalize(vSplinePos - vPrePos);
			vDir = XMVector3Normalize(vCamPos - vSplinePos);

			fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
			if (fRadian < 5.f)
				continue;

			vUp = XMVector3Cross(vRight, vDir);
			vLook = XMVector3Cross(vRight, vUp);

			vSplinePos = XMVectorSetW(vSplinePos, CGameUtility::FloatLerp(fPreLife, m_fLife, fWeight));

			_float4x4		matTrail(vRight, vUp, vLook, vSplinePos);
			m_pVIBufferCom->Add_Instance(matTrail);

			vPrePos = vSplinePos;
		}
	}*/
}

void CEffect_Trail::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_bActive == false)
		return;

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_vecPrePositions.empty() || m_vecPrePositions.back() != vPos)
		m_vecPrePositions.push_back(vPos);

	if (m_vecPrePositions.size() >= 3)
	{
		while (m_vecPrePositions.size() > 3)
			m_vecPrePositions.erase(m_vecPrePositions.begin());

		_vector	vCamPos = CGameInstance::GetInstance()->Get_CameraPosition();
		_float		fSplineLength = XMVectorGetX(XMVector3Length(m_vecPrePositions[1] - m_vecPrePositions[2]));
		_uint		iSegmentCnt = _uint(fSplineLength / m_fSegmentSize);

		_vector	vPoint0 = m_vecPrePositions[0];
		_vector	vPoint1 = m_vecPrePositions[1];
		_vector	vPoint2 = m_vecPrePositions[2];
		_vector	vPoint3 = m_vecPrePositions[2];
		_float		fPreLife = 0.f;

		if (m_pVIBufferCom->Get_InstanceInfo()->size())
			fPreLife = m_pVIBufferCom->Get_InstanceInfo()->back().vPosition.w;

		_vector	vPrePos = vPoint1;

		_float		fWeight = 0.f, fRadian = 0.f;
		_vector	vSplinePos, vRight, vUp, vLook, vDir;
		for (_uint i = 0; i < iSegmentCnt; ++i)
		{
			fWeight = _float(i + 1) / (_float)iSegmentCnt;
			vSplinePos = XMVectorCatmullRom(vPoint0, vPoint1, vPoint2, vPoint3, fWeight);

			vRight = XMVector3Normalize(vSplinePos - vPrePos);
			vDir = XMVector3Normalize(vCamPos - vSplinePos);

			fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
			if (fRadian < 5.f)
				continue;

			vUp = XMVector3Cross(vRight, vDir);
			vLook = XMVector3Cross(vRight, vUp);

			vSplinePos = XMVectorSetW(vSplinePos, CGameUtility::FloatLerp(fPreLife, m_fLife, fWeight));

			_float4x4		matTrail(vRight, vUp, vLook, vSplinePos);
			m_pVIBufferCom->Add_Instance(matTrail);

			vPrePos = vSplinePos;
		}
	}
}

HRESULT CEffect_Trail::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CEffect_Trail::ImGui_RenderProperty()
{
	ImGui::Text("Active");
	ImGui::SameLine();
	if (ImGui::RadioButton("On", m_bActive))
		m_bActive = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("Off", !m_bActive))
		m_bActive = false;

	ImGui::Text("Alpha");
	ImGui::SameLine();
	if (ImGui::RadioButton("Yes", m_bAlpha))
		m_bAlpha = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("No", !m_bAlpha))
		m_bAlpha = false;

	ImGui::InputFloat("Init Life", &m_fLife, 0.01f, 0.1f);
	ImGui::InputFloat("Width", &m_fWidth, 0.1f, 0.5f);
	ImGui::InputFloat("SegmentSize", &m_fSegmentSize, 0.01f, 0.1f);

	ImGui::ColorPicker3("Water Color", (_float*)&m_vColor, ImGuiColorEditFlags_None);
}

HRESULT CEffect_Trail::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Trail", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Trail::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	int a = 0;
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pVIBufferCom->Bind_ShaderResource(m_pShaderCom, L"g_matInfo");
	m_pShaderCom->Set_RawValue(L"g_fLife", &m_fLife, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fWidth", &m_fWidth, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_vColor", &m_vColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_bDistanceAlpha", &m_bAlpha, sizeof(_bool));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
