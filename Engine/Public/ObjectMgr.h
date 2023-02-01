#pragma once
#include "Base.h"
#include "Layer.h"

BEGIN(Engine)

class CObjectMgr final : public CBase
{
	DECLARE_SINGLETON(CObjectMgr)

public:
	CObjectMgr();
	virtual ~CObjectMgr() = default;

public:
	map<const wstring, class CGameObject*>*	Get_Prototypes() { return &m_mapProtypes; }
	map<const wstring, class CLayer*>*			Get_Layers(_uint iLevelIndex) { return &m_pLayers[iLevelIndex]; }
	list<class CGameObject*>*						Get_CloneObjectList(_uint iLevelIndex, const wstring& wstrLayerTag) { return Find_Layer(iLevelIndex, wstrLayerTag)->GetGameObject(); }
	vector<class CGameObject*>*					Get_AnimObjects() { return &m_vecAnimObjects; }
	class CComponent*								Get_Component(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrComponentTag, _uint iIndex);

public:
	HRESULT	Reserve_Manager(_uint iNumLevels);
	HRESULT	Clear(_uint iLevelIndex);

public:
	HRESULT	Add_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);
	HRESULT	Add_Prototype(const wstring& wstrPrototypeTag, class CGameObject* pPrototype);
	HRESULT	Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	HRESULT	Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, _float4x4 matWorld, void* pArg = nullptr);
	class CGameObject*		Clone_GameObject(const wstring& wstrPrototypeTag, void* pArg = nullptr);
	class CGameObject*		Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	class CGameObject*		Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, _float4x4 matWorld, void* pArg = nullptr);
	HRESULT	Add_AnimObject(class CGameObject* pAnimObject);

	void		Tick(_double dTimeDelta);
	void		Late_Tick(_double dTimeDelta);

public:
	void		ImGui_ProtoViewer(const wstring& wstrSelectedProto);
	void		ImGui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObejct);

private:	/* For Prototype Objects */
	map<const wstring, class CGameObject*>				m_mapProtypes;
	typedef map<const wstring, class CGameObject*>		PROTOTYPES;

private:	/* For Cloned Objects */
	map<const wstring, class CLayer*>*						m_pLayers = nullptr;
	typedef map<const wstring, class CLayer*>				LAYERS;
	_uint															m_iNumLevels = 0;

private:	/* For Animation Objects. Used Animation Tool. */
	vector<class CGameObject*>								m_vecAnimObjects;
	typedef vector<class CGameObject*>						ANIMOBJECTS;

private:
	class CGameObject*		Find_Prototype(const wstring& wstrPrototypeTag);
	class CLayer*				Find_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);

public:
	virtual void Free() override;
};

END