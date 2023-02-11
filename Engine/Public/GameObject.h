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
	class CComponent*		Get_Component(const wstring& wstrComponentTag);
	const wstring&			Get_PrototypeGameObjectTag() { return m_wstrPrototypeGameObjectTag; }
	const _bool&				Get_HasModel() { return m_bHasModel; }
	const _float4x4&		Get_WorldMatrix() const { return m_pTransformCom->Get_WorldMatrix(); }
	_vector					Get_Position() { return m_pTransformCom->Get_WorldMatrixXMMatrix().r[3]; }
	STATUS&					Get_Status() { return m_tStatus; }
	const _bool&				Is_Dead() const { return m_bDead; }
	const _bool&				Is_Attack() const { return m_bAttack; }
	const _bool&				Is_SpecialAttack() const { return m_bSpecialAttack; }
	void						Set_Dead() { m_bDead = true; }
	void						Set_WorldMatrix(_float4x4 matWorld) { if (m_pTransformCom != nullptr)		m_pTransformCom->Set_WorldMatrix(matWorld); }
	class CComponent*		Find_Component(const wstring& wstrComponentTag);

public:
	static const wstring		m_wstrTransformComTag;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg);
	virtual void				Tick(_double dTimeDelta);
	virtual void				Late_Tick(_double dTimeDelta);
	virtual HRESULT			Render();
	virtual HRESULT			Render_ShadowDepth();
	virtual HRESULT			Render_Reflect();

public:
	void						ImGui_RenderComponentProperties();
	virtual void				ImGui_RenderProperty();
	virtual pair<_bool, _float3>		Picking_Mesh() { return pair<_bool, _float3>{false, _float3(0.f, 0.f, 0.f)}; }

protected:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	map<const wstring, class CComponent*>		m_mapComponent;

	CTransform*				m_pTransformCom = nullptr;

	_bool						m_bIsClone = false;
	_bool						m_bHasModel = false;
	wstring					m_wstrPrototypeGameObjectTag = L"";

	STATUS					m_tStatus;
	_bool						m_bDead = false;
	_bool						m_bAttack = false;
	_bool						m_bSpecialAttack = false;
	_bool						m_bHit = false;

protected:
	HRESULT					Add_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrComponentTag, class CComponent** ppComponentOut, CGameObject* pOwner, void* pArg = nullptr);

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free();
};

END