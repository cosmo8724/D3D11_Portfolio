#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGrapple_Launcher final : public CGameObject
{
private:
	CGrapple_Launcher(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CGrapple_Launcher(const CGrapple_Launcher& rhs);
	virtual ~CGrapple_Launcher() = default;

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
	CCollider*					m_pRangeCol = nullptr;
	CCollider*					m_pGrapplePointCol = nullptr;

private:
	_float						m_fLaunchAngle = 0.f;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CGrapple_Launcher*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END