#pragma once
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{
		CTransform::TRANSFORMDESC		TransformDesc;
	} GAMEOBJECTDESC;

protected:
	CGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	const _bool&				Get_HasModel() { return m_bHasModel; }
	const _float4x4&		Get_WorldMatrix() const { return m_pTransformCom->Get_WorldMatrix(); }

public:
	static const wstring		m_wstrTransformComTag;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void				Tick(_double dTimeDelta);
	virtual void				Late_Tick(_double dTimeDelta);
	virtual HRESULT			Render();

public:
	void						ImGui_RenderComponentProperties();
	virtual void				ImGui_RenderProperty() {}

protected:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	map<const wstring, class CComponent*>		m_mapComponent;

	CTransform*				m_pTransformCom = nullptr;

	_bool						m_bHasModel = false;

protected:
	HRESULT					Add_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrComponentTag, class CComponent** ppComponentOut, void* pArg = nullptr);
	class CComponent*		Find_Component(const wstring& wstrComponentTag);

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) PURE;
	virtual void				Free();
};

END