#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Engine)

class CGameInstance;
class CGameObject;
class CLayer;

END

BEGIN(Client)

class CTool_MapEditor final : public CTool
{
private:
	CTool_MapEditor();
	virtual ~CTool_MapEditor() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					ImGui_RenderWindow() override;

private:
	map<const wstring, CGameObject*>*					m_mapPrototypes = nullptr;
	_uint														m_iPrototypeCnt = 0;

	map<const wstring, CLayer*>*							m_mapLayers = nullptr;
	_uint														m_iCurLevel = 0;

	map<const wstring, CGameObject*>					m_mapPrototypeModels;

private:
	void							CheckNewPrototype();
	_bool							CheckLevel();

public:
	static CTool_MapEditor*	Create(void* pArg = nullptr);
	virtual void					Free() override;
};

END