#pragma once
#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
END

BEGIN(Client)

class CHat_ToothyHood final : public CGameObject
{
private:
	CHat_ToothyHood(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CHat_ToothyHood(const CHat_ToothyHood& rhs);
	virtual ~CHat_ToothyHood() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_ShadowDepth() override;
	virtual HRESULT			Render_Reflect() override;
	virtual void				ImGui_RenderProperty() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	class CSigrid*				m_pPlayer = nullptr;
	_float4x4					m_matSocketPivot;
	CBone*					m_pSocket = nullptr;
	CTransform*				m_pPlayerTransformCom = nullptr;

	_float4x4					m_matSocket;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_LightDepth();
	HRESULT					SetUp_ShaderResource_Reflect();

public:
	static CHat_ToothyHood*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END