#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CEnemy abstract : public CGameObject
{
protected:
	CEnemy(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	void						Set_Player(class CSigrid* pPlayer) { m_pPlayer = pPlayer; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void				Collision_Event(class CSigrid* pPlayer) {}

protected:
	class CSigrid*				m_pPlayer = nullptr;
	_bool						m_bPlayerDetected = false;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END