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

class CGrapple_Hang final : public CGameObject
{
private:
	CGrapple_Hang(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CGrapple_Hang(const CGrapple_Hang& rhs);
	virtual ~CGrapple_Hang() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Reflect() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pRangeCol = nullptr;
	CCollider*					m_pGrapplePointCol = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_Reflect();

public:
	static CGrapple_Hang*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END