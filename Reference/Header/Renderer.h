#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDERGROUP_END };

public:
	CRenderer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual void			ImGui_RenderProperty() override;

public:
	HRESULT				Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject, _bool bFirst = false);
	HRESULT				Add_DebugRenderGroup(class CComponent* pComponent);
	HRESULT				Draw_RenderGroup(_bool bRenderOFF);

private:
	list<class CGameObject*>				m_RenderObjectList[RENDERGROUP_END];
	typedef	list<class CGameObject*>	RENDEROBJECTS;

	list<class CComponent*>				m_DebugObjectList;
	typedef	list<class CComponent*>	DEBUGOBJECTS;

private:
	class CRenderTargetMgr*				m_pRenderTargetMgr = nullptr;
	class CLightMgr*							m_pLightMgr = nullptr;
	class CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	class CShader*							m_pShaderCom = nullptr;
	_float4x4									m_matWorld, m_matView, m_matProj;

private:
	HRESULT				Render_Priority(_bool bRenderOFF);
	HRESULT				Render_ShadowDepth(_bool bRenderOFF);
	HRESULT				Render_NonAlphaBlend(_bool bRenderOFF);
	HRESULT				Render_LightAcc(_bool bRenderOFF);
	HRESULT				Render_Blend(_bool bRenderOFF);
	HRESULT				Render_DebugObject(_bool bRenderOFF);
	HRESULT				Render_NonLight(_bool bRenderOFF);
	HRESULT				Render_AlphaBlend(_bool bRenderOFF);
	HRESULT				Render_UI();

public:
	static	CRenderer*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual	CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual	void			Free() override;
};

END