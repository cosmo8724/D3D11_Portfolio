#include "stdafx.h"
#include "..\Public\Sigrid.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Static_Camera.h"
#include "Sigrid_State.h"
#include "Ocean.h"
#include "Enemy.h"

CSigrid::CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
	, m_fGravity(0.02f)
	, m_fInitJumpSpeed(1.5f)
	, m_fCurJumpSpeed(0.f)
	, m_fFriction(0.1f)
	, m_fMaxDashTickCount(1.f)
	, m_fCurDashTickCount(0.f)
{
}

CSigrid::CSigrid(const CSigrid & rhs)
	: CGameObject(rhs)
	, m_fGravity(rhs.m_fGravity)
	, m_fInitJumpSpeed(rhs.m_fInitJumpSpeed)
	, m_fCurJumpSpeed(rhs.m_fCurJumpSpeed)
	, m_fFriction(rhs.m_fFriction)
	, m_fMaxDashTickCount(rhs.m_fMaxDashTickCount)
	, m_fCurDashTickCount(rhs.m_fCurDashTickCount)
{
}

HRESULT CSigrid::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(195.5f, 3.26f, 267.f, 1.f));

	/* Set Camera */
	m_pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	m_pCamera->Set_OwnerTransform(m_pTransformCom);
	m_pCamera->Init_Position();

	/* Set Friend State class */
	m_pSigridState = CSigrid_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom, m_pCamera);
	NULL_CHECK_RETURN(m_pSigridState, E_FAIL);

	m_pModelCom->Set_CurAnimationIndex(CSigrid_State::GROUND_IDLE);

	m_tStatus.iMaxHP = 100;
	m_tStatus.iHP = m_tStatus.iMaxHP;
	m_tStatus.iAttack = 3;
	m_tStatus.iSpecialAttack = 5;
	m_tStatus.dInitHitCoolTime = 2.0;
	m_tStatus.dCurHitCoolTime = 0.0;

	return S_OK;
}

void CSigrid::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Check_CurrentNavigation();

	if (m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_OCEAN)
		m_bOnOcean = true;
	else if (m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_GROUND || m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_ROOF)
		m_bOnOcean = false;

	if (m_bOnOcean == true)
		SetOn_Terrain();
	else
		SetOn_Navigation();

	m_pSigridState->Tick(dTimeDelta);
	m_pStateMachineCom->Tick(dTimeDelta);

	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pModelCom->Play_Animation(dTimeDelta, m_eLerpType);
	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, m_dTimeScale);

	m_pOBBCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	m_pSphereCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	_matrix	matNetRing = m_pModelCom->Get_BoneMatrix("NetRing DEF 1");
	m_pNetSphereCol->Update(matNetRing * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
}

void CSigrid::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pSigridState->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
		m_pRendererCom->Add_DebugRenderGroup(m_pOBBCol);
		m_pRendererCom->Add_DebugRenderGroup(m_pNetSphereCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
	}
}

HRESULT CSigrid::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 1);
	}

	return S_OK;
}

void CSigrid::ImGui_RenderProperty()
{
	__super::ImGui_RenderProperty();

	ImGui::InputInt("Coin", &m_tStatus.iCoin, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Navigation", (_int*)&m_eCurNavigation, 1, 1);

	_int	iCurNavigation = (_int)m_eCurNavigation;
	CGameUtility::Saturate(iCurNavigation, (_int)NAVI_END, 0);
	m_eCurNavigation = (NAVIGATIONTYPE)iCurNavigation;
}

_bool CSigrid::Collision_Range(CCollider * pTargetCollider)
{
	return m_pSphereCol->Collision(pTargetCollider);
}

_bool CSigrid::Collision_Body(CCollider * pTargetCollider)
{
	return m_pOBBCol->Collision(pTargetCollider);
}

_bool CSigrid::Collision_Net(CCollider * pTargetCollider)
{
	return m_pNetSphereCol->Collision(pTargetCollider);
}

void CSigrid::Collision_Event(CEnemy * pEnemy)
{
	if (m_tStatus.dCurHitCoolTime < m_tStatus.dInitHitCoolTime)
		m_bHit = false;
	else
	{
		m_bHit = true;
		m_tStatus.dCurHitCoolTime = 0.0;
	}

	if (m_bHit == true)
	{
		if (pEnemy->Is_SpecialAttack() == false)
			m_tStatus.iHP -= pEnemy->Get_Status().iAttack;
		else
			m_tStatus.iHP -= pEnemy->Get_Status().iSpecialAttack;
	}

	if (m_tStatus.iHP < 0)
		m_tStatus.iHP = 0;
}

HRESULT CSigrid::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Sigrid", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("Sigrid_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_OBB", L"Com_OBB", (CComponent**)&m_pOBBCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("NetRing_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.3f, (Ymax - Ymin) * 0.3f, (Zmax - Zmin) * 0.3f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_NetSphere", (CComponent**)&m_pNetSphereCol, this, &ColliderDesc), E_FAIL);

	CNavigation::NAVIGATIONDESC		NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NavigationDesc.iCurrentIndex = 25;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", L"Com_Navigation_Default", (CComponent**)&m_pNavigationCom[NAVI_DEFAULT], this, &NavigationDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Roof", L"Com_Navigation_Roof", (CComponent**)&m_pNavigationCom[NAVI_ROOF], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Sky", L"Com_Navigation_Sky", (CComponent**)&m_pNavigationCom[NAVI_SKY], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Fantasy_Island", L"Com_Navigation_Fantasy_Island", (CComponent**)&m_pNavigationCom[NAVI_FANTASY], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Hellheim", L"Com_Navigation_Hellheim", (CComponent**)&m_pNavigationCom[NAVI_HELLHEIM], this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_RawValue(L"g_WinSize", &_float2((_float)g_iWinSizeX, (_float)g_iWinSizeY),  sizeof(_float2));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSigrid::SetOn_Terrain()
{
	_float4	vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	COcean*	pOcean = dynamic_cast<COcean*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Ocean")->front());
	CVIBuffer_Terrain*		pVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(pOcean->Get_Component(L"Com_VIBuffer"));
	_uint		iNumVerticesX = pVIBuffer->Get_NumVerticesX();
	_uint		iNumVerticesZ = pVIBuffer->Get_NumVerticesZ();
	_float3*	pTerrainPos = pVIBuffer->Get_TerrainPosition();

	_uint	iIndex = (_int)vPlayerPos.z * iNumVerticesX + (_int)vPlayerPos.x;

	if (iIndex < 0 || iIndex > (iNumVerticesX - 1) * (iNumVerticesZ - 1) || vPlayerPos.x < 0.f || vPlayerPos.z < 0.f)
		vPlayerPos.y = 0.f;
	else
	{
		_float	fWidth = vPlayerPos.x - pTerrainPos[iIndex + iNumVerticesX].x;
		_float	fHeight = pTerrainPos[iIndex + iNumVerticesX].z - vPlayerPos.z;

		_vector	vPlane;

		if (fWidth > fHeight)
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + iNumVerticesX + 1], pTerrainPos[iIndex + 1]);
		else
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + 1], pTerrainPos[iIndex]);

		vPlayerPos.y = (XMVectorGetX(vPlane) * vPlayerPos.x * -1.f - XMVectorGetZ(vPlane) * vPlayerPos.z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
	}

	m_fGroundHeight = vPlayerPos.y;

	//if (m_bOnOcean == true && m_bJump == false)
	if (m_bJump == false)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&vPlayerPos));
}

void CSigrid::SetOn_Navigation()
{
	_float4	vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	vPlayerPos = m_pNavigationCom[m_eCurNavigation]->Get_CellHeight(vPlayerPos);
	m_fGroundHeight = vPlayerPos.y;

	if (m_bJump == false)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&vPlayerPos));
}

CSigrid::NAVIGATIONTYPE CSigrid::Check_CurrentNavigation()
{
	NAVIGATIONTYPE	eType = NAVI_END;
	_float					fMinDist = 10000.f;

	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_int		iIndex = -1;

	for (_uint i = 0; i < (_uint)NAVI_END; ++i) 
	{
		if (m_pNavigationCom[i] != nullptr)
		{
			_float		fDist = m_pNavigationCom[i]->IsOnNavigation(vPos, iIndex);

			if (fDist != 0.f && fMinDist > fDist)
			{
				fMinDist = fDist;
				eType = (NAVIGATIONTYPE)i;
			}
		}
	}

	if (iIndex != -1 && eType != NAVI_END)
	{
		m_eCurNavigation = eType;
		m_pNavigationCom[m_eCurNavigation]->Set_CurrentCellIndex(iIndex);
	}

	return eType;
}

CSigrid * CSigrid::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSigrid*		pInstance = new CSigrid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSigrid::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSigrid*		pInstance = new CSigrid(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigrid::Free()
{
	__super::Free();

	Safe_Release(m_pSigridState);

	for (_uint i = 0; i < (_uint)NAVI_END; ++i)
		Safe_Release(m_pNavigationCom[i]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pOBBCol);
	Safe_Release(m_pNetSphereCol);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateMachineCom);
}
