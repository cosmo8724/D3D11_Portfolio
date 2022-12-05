#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4	vEye, vAt, vUp;
	} CAMERADESC;

protected:
	CCamera(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	class CPipeLine*			m_pPipeLine = nullptr;
	CAMERADESC				m_CameraDesc;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END