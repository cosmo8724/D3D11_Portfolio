#pragma once
#include "Base.h"

BEGIN(Engine)

class CObjectMgr final : public CBase
{
	DECLARE_SINGLETON(CObjectMgr)

public:
	CObjectMgr();
	virtual ~CObjectMgr() = default;

public:
	HRESULT	Reverse_Manager(_uint iNumLevels);
	HRESULT	Clear(_uint iLevelIndex);

public:
	HRESULT	Add_Prototype(const wstring& wstrPrototypeTag, class CGameObject* pPrototype);
	HRESULT	Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	void		Tick(_double dTimeDelta);
	void		Late_Tick(_double dTimeDelta);


private:	/* For Prototype Objects */
	map<const wstring, class CGameObject*>				m_mapProtypes;
	typedef map<const wstring, class CGameObject*>		PROTOTYPES;

private:	/* For Cloned Objects */
	map<const wstring, class CLayer*>*						m_pLayers = nullptr;
	typedef map<const wstring, class CLayer*>			LAYERS;
	_uint															m_iNumLevels = 0;

private:
	class CGameObject*		Find_Prototype(const wstring& wstrPrototypeTag);
	class CLayer*				Find_Layer(const wstring& wstrLayerTag);

public:
	virtual void Free() override;
};

END