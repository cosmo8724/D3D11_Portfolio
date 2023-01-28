#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	CUI(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	const wstring&			Get_UITag() const { return m_wstrUITag; }
	void						Set_Player(class CSigrid* pPlayer) { m_pPlayer = pPlayer; }

protected:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	CUI*						Find_UI(_uint iLevelIndex, const wstring& wstrUITag);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

protected:
	CUI*						m_pParent = nullptr;
	class CSigrid*				m_pPlayer = nullptr;
	wstring					m_wstrUITag = L"";
	_float4x4					m_matWorldmulParent;

protected:
	_uint					m_iLastWinSizeX = 0, m_iLastWinSizeY = 0;
	_float					m_fAspectRatioX = 1.f, m_fAspectRatioY = 1.f;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_matView;
	_float4x4				m_matProj;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END