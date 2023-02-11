#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CStateMachine;
class CCollider;
class CNavigation;
class CTexture;
END

BEGIN(Client)

class CSigrid final : public CGameObject
{
	friend	class CSigrid_State;

public:
	enum NAVIGATIONTYPE { NAVI_DEFAULT, NAVI_ROOF, NAVI_SKY, NAVI_FANTASY, NAVI_HELLHEIM, NAVI_END };
	enum ITEMSTATE { ITEM_EQUIPPED, ITEM_NOTPURCHASED, ITEM_PURCHASED, ITEM_END };

private:
	CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSigrid(const CSigrid& rhs);
	virtual ~CSigrid() = default;

public:
	ITEMSTATE				Get_ItemState(_uint iMenuIndex, _uint iSlot) { return m_eItemState[iMenuIndex][iSlot]; }
	_uint						Get_EquipSlot(_uint iMenuIndex);
	_float4					Get_CurrentHairColor();
	void						EquipItem(_uint iMenuIndex, _uint iSlot);
	void						Set_ItemState(_uint iMenuIndex, _uint iSlot, _uint eState) { m_eItemState[iMenuIndex][iSlot] = (ITEMSTATE)eState; }
	void						Set_CurrentOutfit(_uint iIndex) { m_iCurrentOutfit = iIndex; }
	void						Set_PreviewOutfit(_uint iIndex) { m_iPreviewOutfit = iIndex; }
	void						Set_CurrentHair(_uint iIndex) { m_iCurrentHair = iIndex; }
	void						Set_PreviewHair(_uint iIndex) { m_iPreviewHair = iIndex; }
	void						Set_CurrentHat(_uint iIndex) { m_iCurrentHat = iIndex; }
	void						Set_PreviewHat(_uint iIndex) { m_iPreviewHat = iIndex; }
	void						Set_GrappleLauncher() { m_bGrappleLauncher = true; m_bGrappleHang = false; }
	void						Set_GrappleHang() { m_bGrappleLauncher = false; m_bGrappleHang = true; }
	void						Set_SnapGrappleFast(_float fGrabAngle, _float fLaunchAngle, _fvector vGrapplePos, _fmatrix matWorld) {
		m_bSnapGrappleFast = true;
		m_bSnapGrappleSlow = false;
		m_fSnapGrappleAngle = fGrabAngle;
		m_fLauncherAngle = fLaunchAngle;
		m_vSnapGrapplePos = vGrapplePos;
		m_vSnapGrappleLook = matWorld.r[2];
		m_vSnapGrappleRight = matWorld.r[0];
	}
	void						Set_SnapGrappleSlow(_float fGrabAngle, _float fLaunchAngle, _fvector vGrapplePos, _fmatrix matWorld) {
		m_bSnapGrappleFast = false;
		m_bSnapGrappleSlow = true;
		m_fSnapGrappleAngle = fGrabAngle;
		m_fLauncherAngle = fLaunchAngle;
		m_vSnapGrapplePos = vGrapplePos;
		m_vSnapGrappleLook = matWorld.r[2];
		m_vSnapGrappleRight = matWorld.r[0];
	}
	void						Set_BossBattle(_bool bBossBattle) { m_bBossBattle = bBossBattle; }
	const _bool&				Is_GrappleHang() const { return m_bGrappleHang; }
	const _bool&				Is_GrappleLauncher() const { return m_bGrappleLauncher; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_ShadowDepth() override;
	virtual HRESULT			Render_Reflect() override;
	virtual void				ImGui_RenderProperty() override;

public:
	_bool						Collision_Range(CCollider* pTargetCollider);
	_bool						Collision_Body(CCollider* pTargetCollider);
	_bool						Collision_Net(CCollider* pTargetCollider);
	void						Collision_Event(class CEnemy* pEnemy);

public:
	CCollider*					m_pSphereCol = nullptr;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*					m_pOutfitTextureCom[4] = { nullptr };
	CTexture*					m_pHairMaskTextureCom = nullptr;
	
	CCollider*					m_pOBBCol = nullptr;
	CCollider*					m_pNetSphereCol = nullptr;
	CNavigation*				m_pNavigationCom[NAVI_END] = { nullptr };
	NAVIGATIONTYPE		m_eCurNavigation = NAVI_DEFAULT;
	CStateMachine*			m_pStateMachineCom = nullptr;
	class CSigrid_State*		m_pSigridState = nullptr;

	class CStatic_Camera*	m_pCamera = nullptr;

	vector<CGameObject*>	m_vecHats;

	class CTrail_Sigrid_Hand*	m_pLeftHandTrail = nullptr;
	class CTrail_Sigrid_Hand*	m_pRightHandTrail = nullptr;
	class CTrail_Sigrid_Net*	m_pNetTrail = nullptr;

private:	/* State */
	_double					m_dTimeScale = 1.0;
	CModel::LERPTYPE		m_eLerpType = CModel::LERP_BEGIN;

	_uint						m_iCurrentOutfit = 0;
	_uint						m_iPreviewOutfit = 0;
	_uint						m_iCurrentHair = 0;
	_uint						m_iPreviewHair = 0;
	_uint						m_iCurrentHat = 0;
	_uint						m_iPreviewHat = 0;
	_bool						m_bHairMask = false;
	_float4					m_vHairColor[4];
	ITEMSTATE				m_eItemState[3][5] = { {ITEM_EQUIPPED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED},
															{ITEM_EQUIPPED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED},
															{ITEM_EQUIPPED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED, ITEM_NOTPURCHASED} };

	_bool						m_bOnOcean = false;
	_double					m_dSurfTime = 0.0;
	_double					m_dBrakeSpeed = 1.0;
	_double					m_dReduceSpeed = 0.0;

	_bool						m_bBoost = false;
	_float						m_fInertia = 0.f;

	_bool						m_bJump = false;
	_bool						m_bDoubleJump = false;
	_float						m_fGravity;
	_float						m_fInitJumpSpeed;
	_float						m_fCurJumpSpeed;
	_double					m_dPauseTime = 0.0;

	_bool						m_bDash = false;
	_float						m_fFriction;
	_float						m_fMaxDashTickCount;
	_float						m_fCurDashTickCount;

	_bool						m_bGrappleLauncher = false;
	_bool						m_bGrappleHang = false;
	_bool						m_bReadyLaunch = false;
	_bool						m_bSnapGrappleFast = false;
	_bool						m_bSnapGrappleSlow = false;
	_float						m_fSnapGrappleAngle = 0.f;
	_float						m_fLauncherAngle = 0.f;
	_vector					m_vSnapGrapplePos;
	_vector					m_vSnapGrappleLook;
	_vector					m_vSnapGrappleRight;

	_bool						m_bBossBattle = false;
	_float						m_fGroundHeight = 0.f;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts_Hat();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_LightDepth();
	HRESULT					SetUp_ShaderResource_Reflect();

private:
	void						SetOn_Terrain();
	void						SetOn_Navigation();
	NAVIGATIONTYPE		Check_CurrentNavigation();

public:
	static CSigrid*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END