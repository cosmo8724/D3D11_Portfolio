#pragma once
#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CStatic_Camera final : public CCamera
{
private:
	CStatic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CStatic_Camera(const CStatic_Camera& rhs);
	virtual ~CStatic_Camera() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CGameObject*				m_pOwner = nullptr;

private:
	HRESULT						SetUp_Component();

public:
	static CStatic_Camera*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*		Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END