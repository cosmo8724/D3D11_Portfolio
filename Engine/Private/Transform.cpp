#include "..\Public\Transform.h"
#include "GameInstance.h"
#include "Shader.h"

CTransform::CTransform(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
	//, m_TransformDesc(rhs.m_TransformDesc) // Initialize()에서 복사 해줌.
{
}

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	if (eState == STATE_TRANS)
		return;

	_vector	vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scale(_float3 vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANS)
		return;

	Set_State(eState, Get_State(eState) * fScale);
}

HRESULT CTransform::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	m_dInitSpeed = m_TransformDesc.dSpeedPerSec;

	return S_OK;
}

void CTransform::ImGui_RenderProperty()
{
	ImGuizmo::BeginFrame();

	static ImGuizmo::OPERATION CurGuizmoType(ImGuizmo::TRANSLATE);

	ImGui::BulletText("ImGuizmo Type");
	if (ImGui::RadioButton("Translate", CurGuizmoType == ImGuizmo::TRANSLATE))
		CurGuizmoType = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", CurGuizmoType == ImGuizmo::SCALE))
		CurGuizmoType = ImGuizmo::SCALE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", CurGuizmoType == ImGuizmo::ROTATE))
		CurGuizmoType = ImGuizmo::ROTATE;

	_float	vPos[3], vScale[3], vAngle[3];
	ImGuizmo::DecomposeMatrixToComponents((_float*)&m_matWorld, vPos, vAngle, vScale);
	IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Translate", vPos);
	IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Scale", vScale);
	IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Rotate", vAngle);
	ImGuizmo::RecomposeMatrixFromComponents(vPos, vAngle, vScale, (_float*)&m_matWorld);

	ImGuiIO&	io = ImGui::GetIO();
	RECT		rt;
	GetClientRect(CGameInstance::GetInstance()->Get_Handle(), &rt);
	POINT		LT{ rt.left, rt.top };
	ClientToScreen(CGameInstance::GetInstance()->Get_Handle(), &LT);
	ImGuizmo::SetRect((_float)LT.x, (_float)LT.y, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4		matView, matProj;
	XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	ImGuizmo::Manipulate((_float*)&matView, (_float*)&matProj, CurGuizmoType, ImGuizmo::WORLD, (_float*)&m_matWorld);
}

void CTransform::Go_Straight(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPos += XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_BackWard(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPos -= XMVector3Normalize(vLook) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Left(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos -= XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Right(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPos += XMVector3Normalize(vRight) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Up(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPos += XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Go_Down(_double dTimeDelta)
{
	_vector	vPos = Get_State(CTransform::STATE_TRANS);
	_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPos -= XMVector3Normalize(vUp) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

	Set_State(CTransform::STATE_TRANS, vPos);
}

void CTransform::Speed_Up(_bool bKeyState)
{
	if (bKeyState)
		m_TransformDesc.dSpeedPerSec = m_dInitSpeed * 5.0;
	else
		m_TransformDesc.dSpeedPerSec = m_dInitSpeed;
}

void CTransform::Turn(_fvector vAxis, _double dTimeDelta)
{
	_matrix	matRotation = XMMatrixRotationAxis(vAxis, (_float)m_TransformDesc.dRotationPerSec * (_float)dTimeDelta);

	_vector	vRight		= Get_State(CTransform::STATE_RIGHT);
	_vector	vUp		= Get_State(CTransform::STATE_UP);
	_vector	vLook		= Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, matRotation));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, matRotation));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, matRotation));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix	matRotation = XMMatrixRotationAxis(vAxis, fRadian);

	_vector	vRight		= XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector	vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector	vLook		= XMVectorSet(0.f, 0.f, 1.f, 0.f);

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, matRotation));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, matRotation));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, matRotation));
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3	vScale = Get_Scale();

	_vector	vLook = XMVector3Normalize(vTargetPos - Get_State(STATE_TRANS)) * vScale.z;
	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Chase(_fvector vTargetPos, _double dTimeDelta, _float fLimit)
{
	_vector	vPos = Get_State(STATE_TRANS);
	_vector	vDir = vTargetPos - vPos;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance > fLimit)
	{
		vPos += XMVector3Normalize(vDir) * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;
		Set_State(STATE_TRANS, vPos);
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const wstring wstrConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	return pShaderCom->Set_Matrix(wstrConstantName, &m_matWorld);
}

CTransform * CTransform::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
