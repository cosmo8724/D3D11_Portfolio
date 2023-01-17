#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4	vEye, vAt, vUp;
		_float		fFov, fAspect, fNear, fFar;
	} CAMERADESC;

protected:
	CCamera(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	_bool&						Get_RenderState() { return m_bRender; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	_bool						m_bRender = false;
	CAMERADESC				m_CameraDesc;

private:
	class CPipeLine*			m_pPipeLine = nullptr;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END