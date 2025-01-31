#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Engine)

class CComponent;
class CGameObject;
class CLayer;

END

BEGIN(Client)

class CTool_PrototypeMgr final : public CTool
{
private:
	CTool_PrototypeMgr(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CTool_PrototypeMgr() = default;

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void						ImGui_RenderWindow() override;

private:
	DEVICE									m_pDevice = nullptr;
	DEVICE_CONTEXT						m_pContext = nullptr;

private:
	_uint										m_iCurLevel = 0;

	map<const wstring, CComponent*>*	m_mapProtoComponenets = nullptr;
	_uint										m_iProtoComponentCnt = 0;

	map<const wstring, CGameObject*>*	m_mapProtoObjects = nullptr;
	_uint										m_iProtoObjCnt = 0;

	map<const wstring, CLayer*>*			m_mapLayers = nullptr;
	_uint										m_iLayerCnt = 0;

	char*										m_pLevelName[LEVEL_END + 1] = { "Loading", "Logo", "Test Stage", "Public" };
	_int*										m_iSelectTextureCom = nullptr;

private:
	void								Component_Editor();
	void								GameObject_Editor();
	void								CloneObject_Editor();
	COMPONENTTYPE				CheckComponentType(_int iSelectLevel, const char* pComponentTag);
	COMPONENTTYPE				CheckComponentType(class CComponent* pComponent);
	void								SortComponentByType(char***& ppComponentTag, _uint* pComponentCnt);
	_uint								FindPrototypeComponentLevel(const _tchar* pComponentTag);

public:
	static CTool_PrototypeMgr*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void* pArg = nullptr);
	virtual void						Free() override;
};

END