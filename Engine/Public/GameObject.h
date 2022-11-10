#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void				Tick(_double dTimeDelta);
	virtual void				Late_Tick(_double dTimeDelta);
	virtual HRESULT			Render();

protected:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	map<const wstring, class CComponent*>		m_mapComponent;

protected:
	HRESULT					Add_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrComponentTag, class CComponent** ppComponentOut, void* pArg = nullptr);
	class CComponent*		Find_Component(const wstring& wstrComponentTag);

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) PURE;
	virtual void				Free();
};

END