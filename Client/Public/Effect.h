#pragma once
#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect abstract : public CGameObject
{
protected:
	CEffect(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pMaskTextureCom = nullptr;

protected:
	class CSigrid*				m_pPlayer = nullptr;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END