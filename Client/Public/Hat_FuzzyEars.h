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

class CHat_FuzzyEars final : public CGameObject
{
private:
	CHat_FuzzyEars(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CHat_FuzzyEars(const CHat_FuzzyEars& rhs);
	virtual ~CHat_FuzzyEars() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_ShadowDepth() override;
	virtual void				ImGui_RenderProperty() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*					m_pHairMaskTextureCom = nullptr;

private:
	class CSigrid*				m_pPlayer = nullptr;
	_float4x4					m_matSocketPivot;
	CBone*					m_pSocket = nullptr;
	CTransform*				m_pPlayerTransformCom = nullptr;

	_float4x4					m_matSocket;

private:
	_bool						m_bHairMask = false;
	_float4					m_vHairColor = { 0.f, 0.f, 0.f, 0.f };

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_LightDepth();

public:
	static CHat_FuzzyEars*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END