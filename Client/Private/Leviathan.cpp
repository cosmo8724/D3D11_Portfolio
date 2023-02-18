#include "stdafx.h"
#include "..\Public\Leviathan.h"
#include "GameInstance.h"
#include "Leviathan_State.h"
#include "Sigrid.h"
#include "Static_Camera.h"

CLeviathan::CLeviathan(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEnemy(pDevice, pContext)
{
}

CLeviathan::CLeviathan(const CLeviathan & rhs)
	: CEnemy(rhs)
{
}

HRESULT CLeviathan::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CLeviathan::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 10.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(190.f, -2.f, 767.f, 1.f));

	m_pLeviathan_State = CLeviathan_State::Create(m_pDevice, m_pContext, this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);

	m_pModelCom->Set_PivotMatrix(XMMatrixRotationY(XMConvertToRadians(-90.f)));
	m_pModelCom->Set_CurAnimationIndex(0);

	m_tStatus.iMaxHP = 100;
	m_tStatus.iHP = m_tStatus.iMaxHP;
	m_tStatus.iAttack = 10;
	m_tStatus.iSpecialAttack = 20;
	m_tStatus.dInitAttackCoolTime = 6.0;
	m_tStatus.dCurAttackCoolTime = 0.0;
	m_tStatus.dInitHitCoolTime = 0.5;
	m_tStatus.dCurHitCoolTime = 0.0;

	return S_OK;
}

void CLeviathan::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	SetOn_Navigation();

	m_pLeviathan_State->Tick(dTimeDelta);
	m_pStateMachineCom->Tick(dTimeDelta);
	//m_pModelCom->Play_Animation(dTimeDelta;)
	if (m_bPlayerDetected)
		m_pModelCom->Play_Animation(dTimeDelta);
	else
		m_pModelCom->Play_Animation(0.0);

	_matrix	matSocket = m_pModelCom->Get_BoneMatrix("C_CharaA_a");
	m_pRangeCol->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tongue_b");
	m_pSphereCol[HEAD]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	CStatic_Camera*	pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	CSigrid*				pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());

	_vector	vRayPos, vRayDir;
	_float		fDist = 0.f;

	vRayPos = XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[3];
	vRayDir = XMVector3Normalize(XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[2]);

	if (m_pSphereCol[HEAD]->Collision_Ray(vRayPos, vRayDir, fDist) && fDist < 30.f && fDist > 5.f)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E) && pPlayer->Is_GrappleLauncher() == false)
		{
			_vector		vPlayerPos = XMLoadFloat4x4(&pPlayer->Get_WorldMatrix()).r[3];
			_vector		vPlayerLook = XMLoadFloat4x4(&pPlayer->Get_WorldMatrix()).r[2];
			vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
			_vector		vHeadPos = (matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix())).r[3];
			_vector		vDir = XMVector3Normalize(vPlayerPos - vHeadPos);

			_float			fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vHeadPos - vPlayerPos))));

			if (fAngle < XMConvertToRadians(25.f))
				fAngle = 0.f;
			else if (fAngle > XMConvertToRadians(25.f) && fAngle < XMConvertToRadians(75.f))
				fAngle = XMConvertToRadians(45.f);
			else
				fAngle = XMConvertToRadians(90.f);

			if (XMVectorGetY(vPlayerPos) > XMVectorGetY(vHeadPos))
				fAngle *= -1.f;

			pPlayer->Set_SnapGrappleFast(fAngle, 0.f, vHeadPos + XMVector3Normalize(XMVectorSetY(vDir, 0.f)) * 10.f, matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
		}
	}

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_c");
	m_pSphereCol[BODY_A]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_e");
	m_pSphereCol[BODY_B]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_g");
	m_pSphereCol[BODY_C]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_i");
	m_pSphereCol[BODY_D]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_k");
	m_pSphereCol[BODY_E]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_m");
	m_pSphereCol[BODY_F]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_o");
	m_pSphereCol[BODY_G]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_q");
	m_pSphereCol[BODY_H]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_s");
	m_pSphereCol[BODY_I]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_u");
	m_pSphereCol[BODY_J]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_x");
	m_pSphereCol[BODY_K]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_Tail_z");
	m_pSphereCol[BODY_L]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("C_TailfinA_c");
	m_pSphereCol[TAIL]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("L_FfinB_Spo");
	m_pSphereCol[LWING_A]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("L_Farm_b_Phy");
	m_pSphereCol[LWING_B]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("R_FfinB_Spo");
	m_pSphereCol[RWING_A]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	matSocket = m_pModelCom->Get_BoneMatrix("R_Farm_b_Phy");
	m_pSphereCol[RWING_B]->Update(matSocket * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 10.f);
	CGameInstance::GetInstance()->Set_LightPosition(3, vPos);
}

void CLeviathan::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pLeviathan_State->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		//if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 20.f))

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		//m_pRendererCom->Add_DebugRenderGroup(m_pRangeCol);

		//for (_uint i = 0; i < (_uint)HITBOX_END; ++i)
		//	m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol[i]);
		//m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
	}
}

HRESULT CLeviathan::Render()
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

HRESULT CLeviathan::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 4);
	}

	return S_OK;
}

void CLeviathan::Collision_Event(CSigrid * pPlayer)
{
	if (pPlayer == nullptr)
		return;

	if (m_tStatus.dCurHitCoolTime < m_tStatus.dInitHitCoolTime)
		m_bHit = false;
	else
	{
		m_bHit = true;
		m_tStatus.dCurHitCoolTime = 0.0;
	}

	if (m_bHit == true)
	{
		pPlayer->Set_AttackHit();

		if (m_pPlayer->Is_SpecialAttack() == false)
			m_tStatus.iHP -= pPlayer->Get_Status().iAttack;
		else
			m_tStatus.iHP -= pPlayer->Get_Status().iSpecialAttack;
	}

	if (m_tStatus.iHP < 0)
		m_tStatus.iHP = 0;
}

HRESULT CLeviathan::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Leviathan", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("SU0004_00out.md", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Head", (CComponent**)&m_pSphereCol[HEAD], this, &ColliderDesc), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_A", (CComponent**)&m_pSphereCol[BODY_A], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_B", (CComponent**)&m_pSphereCol[BODY_B], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_C", (CComponent**)&m_pSphereCol[BODY_C], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_D", (CComponent**)&m_pSphereCol[BODY_D], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_E", (CComponent**)&m_pSphereCol[BODY_E], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_F", (CComponent**)&m_pSphereCol[BODY_F], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_G", (CComponent**)&m_pSphereCol[BODY_G], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_H", (CComponent**)&m_pSphereCol[BODY_H], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_I", (CComponent**)&m_pSphereCol[BODY_I], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_J", (CComponent**)&m_pSphereCol[BODY_J], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_K", (CComponent**)&m_pSphereCol[BODY_K], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Body_L", (CComponent**)&m_pSphereCol[BODY_L], this, &ColliderDesc), E_FAIL);

	ColliderDesc.vSize = _float3(4.f, 0.5f, 2.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_Tail", (CComponent**)&m_pSphereCol[TAIL], this, &ColliderDesc), E_FAIL);
	ColliderDesc.vSize = _float3(2.f, 0.5f, 4.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_LWing_A", (CComponent**)&m_pSphereCol[LWING_A], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_RWing_A", (CComponent**)&m_pSphereCol[RWING_A], this, &ColliderDesc), E_FAIL);
	ColliderDesc.vSize = _float3(3.f, 0.5f, 6.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_LWing_B", (CComponent**)&m_pSphereCol[LWING_B], this, &ColliderDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere_RWing_B", (CComponent**)&m_pSphereCol[RWING_B], this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(50.f, 50.f, 50.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	CNavigation::NAVIGATIONDESC		NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NavigationDesc.iCurrentIndex = 0;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", L"Com_Navigation", (CComponent**)&m_pNavigationCom, this, &NavigationDesc), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CLeviathan::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLeviathan::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_RawValue(L"g_vClipPlane", &pGameInstance->Get_ClipPlane(CPipeLine::CLIPPLANE_REFLECT), sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLeviathan::SetOn_Navigation()
{
	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	vPos = m_pNavigationCom->Get_CellHeight(vPos);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);
}

CLeviathan * CLeviathan::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CLeviathan*		pInstance = new CLeviathan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLeviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLeviathan::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CLeviathan*		pInstance = new CLeviathan(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CLeviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeviathan::Free()
{
	__super::Free();

	Safe_Release(m_pLeviathan_State);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pNavigationCom);

	for (_uint i = 0; i < (_uint)HITBOX_END; ++i)
		Safe_Release(m_pSphereCol[i]);

	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
