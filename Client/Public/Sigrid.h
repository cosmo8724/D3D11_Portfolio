#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CStateMachine;
class CCollider;
END

BEGIN(Client)

class CSigrid final : public CGameObject
{
	friend	class CSigrid_State;

private:
	CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSigrid(const CSigrid& rhs);
	virtual ~CSigrid() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pSphereCol = nullptr;
	CCollider*					m_pOBBCol = nullptr;
	CCollider*					m_pNetSphereCol = nullptr;
	CStateMachine*			m_pStateMachineCom = nullptr;
	class CSigrid_State*		m_pSigridState = nullptr;

	class CStatic_Camera*	m_pCamera = nullptr;

private:	/* State */
	CModel::LERPTYPE		m_eLerpType = CModel::LERP_BEGIN;

	_bool						m_bJump = false;
	_bool						m_bDoubleJump = false;
	_float						m_fGravity;
	_float						m_fInitJumpSpeed;
	_float						m_fCurJumpSpeed;

	_bool						m_bDash = false;
	_float						m_fFriction;
	_float						m_fMaxDashTickCount;
	_float						m_fCurDashTickCount;

	_float						m_fGroundHeight = 0.f;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

private:
	void						SetOn_Terrain();

public:
	static CSigrid*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END