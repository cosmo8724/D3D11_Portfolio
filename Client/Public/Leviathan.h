#pragma once
#include "Enemy.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CStateMachine;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CLeviathan final : public CEnemy
{
	friend class CLeviathan_State;

public:
	enum HITBOX {
		HEAD, BODY_A, BODY_B, BODY_C, BODY_D, BODY_E, BODY_F, BODY_G, BODY_H, BODY_I, BODY_J, BODY_K, BODY_L, TAIL, LWING_A, LWING_B, RWING_A, RWING_B, HITBOX_END
	};

private:
	CLeviathan(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CLeviathan(const CLeviathan& rhs);
	virtual ~CLeviathan() = default;

public:
	const _bool&				Get_Die() const { return m_bDie; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Reflect() override;
	virtual void				Collision_Event(class CSigrid* pPlayer) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pRangeCol = nullptr;
	CCollider*					m_pSphereCol[HITBOX_END] = { nullptr };
	CNavigation*				m_pNavigationCom = nullptr;
	CStateMachine*			m_pStateMachineCom = nullptr;
	class CLeviathan_State*	m_pLeviathan_State = nullptr;

private:
	_bool						m_bDie = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_Reflect();
	void						SetOn_Navigation();

public:
	static CLeviathan*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END