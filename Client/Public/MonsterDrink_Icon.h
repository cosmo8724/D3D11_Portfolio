#pragma once
#include "UI.h"
#include "MonsterDrink_Frame.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonsterDrink_Icon final : public CUI
{
private:
	CMonsterDrink_Icon(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMonsterDrink_Icon(const CMonsterDrink_Icon& rhs);
	virtual ~CMonsterDrink_Icon() = default;

public:
	virtual HRESULT			Initialize_Prototype(CMonsterDrink_Frame::MONSTERCOLOR eColor);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CMonsterDrink_Frame::MONSTERCOLOR	m_eColor = CMonsterDrink_Frame::MONSTERDRINK_END;

private:
	HRESULT					SetUp_Component(CMonsterDrink_Frame::MONSTERCOLOR eColor);
	HRESULT					SetUp_ShaderResources();

public:
	static CMonsterDrink_Icon*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CMonsterDrink_Frame::MONSTERCOLOR eColor);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END