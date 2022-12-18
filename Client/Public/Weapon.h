#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CBone;
class CTransform;
END

BEGIN(Client)

class CWeapon abstract : public CGameObject
{
public:
	typedef struct tagWeaponDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4x4				matSocketPivot;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
	} WEAPONDESC;

protected:
	CWeapon(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	void						Set_Owner(class CPlayer* pOwner) { m_pOwner = pOwner; }
	void						Set_SocketMatrix(_float4x4 matSocket);

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	class CPlayer*			m_pOwner = nullptr;

	WEAPONDESC			m_tWeaponDesc;
	_float4x4					m_matSocket;
	_float4x4					m_matPivot;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END