#pragma once
#include "Client_Define.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
END

BEGIN(Client)

class CSword_Blade final : public CWeapon
{
private:
	CSword_Blade(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSword_Blade(const CSword_Blade& rhs);
	virtual ~CSword_Blade() = default;

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

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CSword_Blade*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END