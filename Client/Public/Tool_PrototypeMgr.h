#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Engine)

class CComponent;
class CGameObject;

END

BEGIN(Client)

class CTool_PrototypeMgr final : public CTool
{
	enum COMPONENTTYPE { COM_RENDERER, COM_VIBUFFER, COM_SHADER, COM_TRANSFORM, COM_TEXTURE, COM_MODEL, COMPONENTTYPE_END };

private:
	CTool_PrototypeMgr();
	virtual ~CTool_PrototypeMgr() = default;

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void						ImGui_RenderWindow() override;

private:
	_uint										m_iCurLevel = 0;

	map<const wstring, CComponent*>*	m_mapProtoComponenets = nullptr;
	_uint										m_iProtoComponentCnt = 0;

	map<const wstring, CGameObject*>*	m_mapProtoObjects = nullptr;
	_uint										m_iProtoObjCnt = 0;

	char*										m_pLevelName[LEVEL_END + 1] = { "Loading", "Logo", "Test Stage", "Public"};

private:
	void								Component_Editor();
	void								GameObject_Editor();
	COMPONENTTYPE				CheckComponentType(_int iSelectLevel, const char* pComponentTag);

public:
	static CTool_PrototypeMgr*	Create(void* pArg = nullptr);
	virtual void						Free() override;
};

END