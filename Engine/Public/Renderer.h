#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDERGROUP_END };

public:
	CRenderer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	HRESULT				Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT				Add_DebugRenderGroup(class CComponent* pComponent);
	HRESULT				Draw_RenderGroup();

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
	HRESULT				Render_Priority();
	HRESULT				Render_NonAlphaBlend();
	HRESULT				Render_LightAcc();
	HRESULT				Render_Blend();
	HRESULT				Render_DebugObject();
	HRESULT				Render_NonLight();
	HRESULT				Render_AlphaBlend();
	HRESULT				Render_UI();

public:
	static	CRenderer*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual	CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual	void			Free() override;
};

END