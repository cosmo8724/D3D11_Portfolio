#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Static_Camera.h"
#include "Weapon.h"
#include "Bone.h"

CPlayer::CPlayer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

_matrix CPlayer::Get_BornMatrix(const string & strBoneName)
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->Get_BoneMatrix(strBoneName);
}

_matrix CPlayer::Get_OffsetMatrix(const string & strBoneName)
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->Get_OffsetMatrix(strBoneName);
}

_matrix CPlayer::Get_PivotMatrix()
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->Get_PivotMatrix();
}

_vector CPlayer::Get_TransformState(CTransform::STATE eState)
{
	if (m_pModelCom == nullptr)
		return XMVectorSet(0.f, 1.f, 0.f, 0.f);

	return m_pTransformCom->Get_State(eState);
}

HRESULT CPlayer::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CPlayer::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 15.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Weapon(), E_FAIL);

	m_pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	m_pCamera->Set_OwnerTransform(m_pTransformCom);

	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Mouse_Move(dTimeDelta);
	//Move_Camera();

	static _int	iCurrentAnimation = 0;
	_uint		iAnimationCnt = m_pModelCom->Get_NumAnimations();
	if (CGameInstance::GetInstance()->Key_Down(DIK_HOME))
		iCurrentAnimation++;
	if (CGameInstance::GetInstance()->Key_Down(DIK_END))
		iCurrentAnimation--;

	iCurrentAnimation = 6;

	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Go_Straight(dTimeDelta);
		iCurrentAnimation = 13;
	}
	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Go_Left(dTimeDelta);
		iCurrentAnimation = 13;
	}
	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Go_BackWard(dTimeDelta);
		iCurrentAnimation = 13;
	}
	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Go_Right(dTimeDelta);
		iCurrentAnimation = 13;
	}
	if (CGameInstance::GetInstance()->Get_DIMouseState(DIM_LB))
		iCurrentAnimation = 20;

	//m_pModelCom->Set_CurAnimationIndex(iCurrentAnimation);

	m_pModelCom->Play_Animation(dTimeDelta);

	m_iNumWeapons = (_uint)m_vecPlayerWeapon.size();
	for (_uint i = 0; i < m_iNumWeapons; ++i)
		m_vecPlayerWeapon[i]->Tick(dTimeDelta);
}

void CPlayer::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
	
	//Move_Weapon();

	for (_uint i = 0; i < m_iNumWeapons; ++i)
		m_vecPlayerWeapon[i]->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones");
	}

	return S_OK;
}

void CPlayer::ImGui_RenderProperty()
{
}

HRESULT CPlayer::Ready_Weapon()
{
	CWeapon*		pPartObject = nullptr;

	/* Sword_Handle */
	CWeapon::WEAPONDESC	tWeaponDesc;
	ZeroMemory(&tWeaponDesc, sizeof(CWeapon::WEAPONDESC));
	XMStoreFloat4x4(&tWeaponDesc.matSocketPivot, m_pModelCom->Get_PivotMatrix());
	tWeaponDesc.pSocket = m_pModelCom->Get_BoneFromEntireBone("Weapon_r");
	tWeaponDesc.pTargetTransform = m_pTransformCom;

	pPartObject = dynamic_cast<CWeapon*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Player_Parts", L"Prototype_GameObject_Handle", &tWeaponDesc));
	NULL_CHECK_RETURN(pPartObject, E_FAIL);
	pPartObject->Set_Owner(this);

	m_vecPlayerWeapon.push_back(pPartObject);

	/* Sword_Blade */
	pPartObject = dynamic_cast<CWeapon*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Player_Parts", L"Prototype_GameObject_Blade", &tWeaponDesc));
	NULL_CHECK_RETURN(pPartObject, E_FAIL);
	pPartObject->Set_Owner(this);

	m_vecPlayerWeapon.push_back(pPartObject);

	return S_OK;
}

HRESULT CPlayer::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_GhostRunner_Hand", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);

	//XMStoreFloat4x4(&m_matOffset, XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()) * XMLoadFloat4x4(&m_matOffset));
	//m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matOffset);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayer::Mouse_Move(_double dTimeDelta)
{
	/* 플레이어 회전 */
	_long		MouseMove = 0;
	if (MouseMove = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * MouseMove * 0.1f);
	if (MouseMove = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * MouseMove * 0.1f);

	m_dMouseMove = (_double)MouseMove;
}

void CPlayer::Move_Camera()
{
	/* 플레이어 키 구하기 */
	_vector	vHeadPos = (m_pModelCom->Get_BoneFromEntireBone("head")->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix()).r[3];
	_vector	vRootPos = (m_pModelCom->Get_BoneFromEntireBone("root")->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix()).r[3];
	m_fTall = XMVectorGetX(XMVector3Length(vHeadPos - vRootPos));

	/* pitch 구하기 */
	_float		fPitch = - asinf(XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.f, 1.f, 0.f, 0.f))));

	/* z, z' 구하기 */
	_float		fLookDist = m_fTall * sinf(fPitch);
	_float		fYDist = m_fTall - m_fTall * fabs(cosf(fPitch));

	/* 보정행렬 구하기 */
	_vector	vPlayerLook = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()).r[2];
	vPlayerLook = XMVector3Normalize(XMVectorSetY(vPlayerLook, 0.f)) ;
	_vector	vDir = vPlayerLook * -fLookDist;
	_vector	vUp = XMVectorSet(0.f, fYDist, 0.f, 0.f);
	_float4	vOffset = vDir + vUp;

	m_matOffset = m_pTransformCom->Get_WorldMatrix();
	m_matOffset._41 += vOffset.x;
	m_matOffset._42 += vOffset.y;
	m_matOffset._43 += vOffset.z;

	_matrix	matCamPitvot = _float4x4::CreateRotationY(XMConvertToRadians(-90.f));

	_matrix	matHead = m_pModelCom->Get_BoneFromEntireBone("head")->Get_CombindMatrix();
	_float3	fScale;

	fScale.x = XMVectorGetX(XMVector3Length(matHead.r[0]));
	fScale.y = XMVectorGetX(XMVector3Length(matHead.r[1]));
	fScale.z = XMVectorGetX(XMVector3Length(matHead.r[2]));

	matHead.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale.x;
	matHead.r[0] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale.y;
	matHead.r[0] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale.z;
	matHead.r[3] = XMVectorSetX(matHead.r[3], XMVectorGetX(matHead.r[3]) - 10.f);
	matHead.r[3] = XMVectorSetY(matHead.r[3], XMVectorGetY(matHead.r[3]) + 1.f);
	matHead.r[3] = XMVectorSetZ(matHead.r[3], XMVectorGetZ(matHead.r[3]) - 10.f);

	matHead = matCamPitvot
				* matHead
				* m_pModelCom->Get_PivotMatrix() 
				* XMLoadFloat4x4(&m_matOffset);

	m_matHeadWithOffset = matHead;

	dynamic_cast<CTransform*>(m_pCamera->Get_Component(L"Com_Transform"))->Set_WorldMatrix(m_matHeadWithOffset);
}

void CPlayer::Move_Weapon()
{
	_vector	vWeaponPos = (m_pModelCom->Get_BoneFromEntireBone("Weapon_r")->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix()).r[3];
	_vector	vRootPos = (m_pModelCom->Get_BoneFromEntireBone("root")->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix()).r[3];
	m_fWeaponHeight = XMVectorGetX(XMVector3Length(vWeaponPos - vRootPos));

	/* pitch 구하기 */
	_float		fPitch = -asinf(XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.f, 1.f, 0.f, 0.f))));

	/* z, z' 구하기 */
	_float		fLookDist = m_fWeaponHeight * sinf(fPitch);
	_float		fYDist = m_fWeaponHeight - m_fWeaponHeight * fabs(cosf(fPitch));

	/* 보정행렬 구하기 */
	_vector	vPlayerLook = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()).r[2];
	vPlayerLook = XMVector3Normalize(XMVectorSetY(vPlayerLook, 0.f));
	_vector	vDir = vPlayerLook * -fLookDist;
	_vector	vUp = XMVectorSet(0.f, fYDist, 0.f, 0.f);
	_float4	vOffset = vDir + vUp;

	m_matWeaponOffset = m_pTransformCom->Get_WorldMatrix();
	m_matWeaponOffset._41 += vOffset.x;
	m_matWeaponOffset._42 += vOffset.y;
	m_matWeaponOffset._43 += vOffset.z;

	_matrix	matWeapon = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(180.f))
		* m_pModelCom->Get_BoneFromEntireBone("Weapon_r")->Get_CombindMatrix()
		* m_pModelCom->Get_PivotMatrix()
		* XMLoadFloat4x4(&m_matOffset);

	for (auto& pWeapon : m_vecPlayerWeapon)
		//pWeapon->Set_SocketMatrix(m_matWeaponOffset);
		pWeapon->Set_WorldMatrix(matWeapon);
}

CPlayer * CPlayer::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pWeapon : m_vecPlayerWeapon)
		Safe_Release(pWeapon);
	m_vecPlayerWeapon.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
