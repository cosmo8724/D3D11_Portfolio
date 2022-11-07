#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final :	public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT			Add_GameObject(class CGameObject* pGameObject);
	HRESULT			Initialize();
	void				Tick(_double dTimeDelta);
	void				Late_Tick(_double dTimeDelta);

private:
	list<class CGameObject*>				m_GameObjectList;
	typedef list<class CGameOjject*>		GAMEOBJECTS;

public:
	static CLayer*	Create();
	virtual void		Free() override;
};

END