#pragma once
#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CDynamic_Camera final : public CCamera
{
private:
	CDynamic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CDynamic_Camera(const CDynamic_Camera& rhs);
	virtual ~CDynamic_Camera() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	HRESULT						SetUp_Component();

public:
	static CDynamic_Camera*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*		Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END