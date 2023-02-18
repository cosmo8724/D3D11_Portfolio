#pragma once
#include "NPC.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CCollider;
END

BEGIN(Client)

class CHollow_Knight final : public CNPC
{
private:
	CHollow_Knight(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CHollow_Knight(const CHollow_Knight& rhs);
	virtual ~CHollow_Knight() = default;

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

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CHollow_Knight*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END