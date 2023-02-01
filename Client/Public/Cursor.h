#pragma once
#include "UI.h"

BEGIN(Client)

class CCursor final : public CUI
{
public:
	enum CURSORSTATE { CURSOR_DEFAULT, CURSOR_SELECT, CORSOR_END };

private:
	CCursor(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CCursor(const CCursor& rhs);
	virtual ~CCursor() = default;

public:
	void						Set_CursorState(CURSORSTATE eState) { m_eState = eState; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CURSORSTATE			m_eState = CURSOR_DEFAULT;
	_long						m_MouseMoveX = 0;
	_long						m_MouseMoveY = 0;
	RECT						m_Rect;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CCursor*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END