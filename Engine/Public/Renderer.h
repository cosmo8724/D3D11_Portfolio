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
	virtual HRESULT		Initialize(void* pArg) override;

public:
	HRESULT				Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT				Draw_RenderGroup();

private:
	list<class CGameObject*>				m_RenderObjectList[RENDERGROUP_END];
	typedef	list<class CGameObject*>	RENDEROBJECTS;

private:
	HRESULT				Render_Priority();
	HRESULT				Render_NonAlphaBlend();
	HRESULT				Render_NonLight();
	HRESULT				Render_AlphaBlend();
	HRESULT				Render_UI();

public:
	static	CRenderer*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual	CComponent*	Clone(void* pArg = nullptr) override;
	virtual	void			Free() override;
};

END