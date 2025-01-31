#pragma once
#include "Client_Define.h"
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

class CShinobi final : public CEnemy
{
private:
	CShinobi(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShinobi(const CShinobi& rhs);
	virtual ~CShinobi() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				Collision_Event(class CSigrid* pPlayer) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pRangeCol = nullptr;
	CCollider*					m_pSphereCol = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;
	CStateMachine*			m_pStateMachineCom = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	void						SetOn_Navigation();

public:
	static CShinobi*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END