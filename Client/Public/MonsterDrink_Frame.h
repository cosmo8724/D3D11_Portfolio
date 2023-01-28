#pragma once
#include "UI.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonsterDrink_Frame final : public CUI
{
public:
	enum MONSTERCOLOR { MONSTERDRINK_BLACK, MONSTERDRINK_ORANGE, MONSTERDRINK_PINK, MONSTERDRINK_WHITE, MONSTERDRINK_END };

private:
	CMonsterDrink_Frame(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMonsterDrink_Frame(const CMonsterDrink_Frame& rhs);
	virtual ~CMonsterDrink_Frame() = default;

public:
	virtual HRESULT			Initialize_Prototype(MONSTERCOLOR eColor);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	MONSTERCOLOR			m_eColor = MONSTERDRINK_END;
	RECT						m_Rect;
	_bool						m_bInitPos = true;
	_int						m_iCurMoveCnt = 0;
	_int						m_iComebackMoveCnt = 0;
	_int						m_iLastMoveCnt = 0;
	_int						m_iMaxMoveCnt = 50;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CMonsterDrink_Frame*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MONSTERCOLOR eColor);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END