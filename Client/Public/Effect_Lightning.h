#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CEffect_Lightning final : public CEffect_Mesh
{
private:
	CEffect_Lightning(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_Lightning(const CEffect_Lightning& rhs);
	virtual ~CEffect_Lightning() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Reflect() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bScaleFix = false;
	_double					m_dTimeDelta = 0.0;
	_double					m_dSpeed = 0.025;
	_bool						m_bFinish = false;
	_uint						m_iWidthFrame = 0;
	_uint						m_iHeightFrame = 0;
	_uint						m_iWidthCount = 0;
	_uint						m_iHeightCount = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_Reflect();

public:
	static CEffect_Lightning*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END