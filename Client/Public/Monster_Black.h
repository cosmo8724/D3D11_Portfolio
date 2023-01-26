#pragma once
#include "MonsterDrink.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_Black final : public CMonsterDrink
{
private:
	CMonster_Black(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMonster_Black(const CMonster_Black& rhs);
	virtual ~CMonster_Black() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CMonster_Black*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END