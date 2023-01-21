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
END

BEGIN(Client)

class CSigrid final : public CGameObject
{
	friend	class CSigrid_State;

public:
	enum NAVIGATIONTYPE { NAVI_DEFAULT, NAVI_ROOF, NAVI_SKY, NAVI_FANTASY, NAVI_END };

private:
	CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSigrid(const CSigrid& rhs);
	virtual ~CSigrid() = default;

public:
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
	const _bool&				Is_GrappleHang() const { return m_bGrappleHang; }
	const _bool&				Is_GrappleLauncher() const { return m_bGrappleLauncher; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
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
	
	CCollider*					m_pOBBCol = nullptr;
	CCollider*					m_pNetSphereCol = nullptr;
	CNavigation*				m_pNavigationCom[NAVI_END] = { nullptr };
	NAVIGATIONTYPE		m_eCurNavigation = NAVI_DEFAULT;
	CStateMachine*			m_pStateMachineCom = nullptr;
	class CSigrid_State*		m_pSigridState = nullptr;

	class CStatic_Camera*	m_pCamera = nullptr;

private:	/* State */
	_double					m_dTimeScale = 1.0;
	CModel::LERPTYPE		m_eLerpType = CModel::LERP_BEGIN;

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

	_float						m_fGroundHeight = 0.f;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

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