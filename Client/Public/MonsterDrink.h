#pragma once
#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CMonsterDrink abstract : public CGameObject
{
protected:
	CMonsterDrink(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMonsterDrink(const CMonsterDrink& rhs);
	virtual ~CMonsterDrink() = default;

public:
	void						Set_Player(class CSigrid* pPlayer) { m_pPlayer = pPlayer; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	CCollider*					m_pSphereCol = nullptr;

protected:
	class CMonsterDrink_Frame*		m_pUIFrame = nullptr;
	class CSigrid*				m_pPlayer = nullptr;
	
	_uint						m_iValue = 0;

private:
	_double					m_dMoveTime = 0.0;

private:
	HRESULT					SetUp_Component();

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END