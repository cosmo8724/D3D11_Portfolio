#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT		Initialize(const wstring& wstrCloneObjFilePath);
	virtual void			Tick(_double dTimeDelta) override;
	virtual void			Late_Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT				Load_CloneObjects(const wstring& wstrCloneObjFilePath);
	HRESULT				Ready_Light();
	HRESULT				Ready_Layer_Camera(const wstring&  wstrLayertTag);
	HRESULT				Ready_Layer_UI(const wstring& wstrLayerTag);
	HRESULT				Ready_Layer_SkyBox(const wstring& wstrLayerTag);
	HRESULT				Ready_Layer_Ocean(const wstring& wstrLayerTag);
	HRESULT				Ready_Layer_Player(const wstring& wstrLayerTag);
	HRESULT				Ready_Layer_Islands(const wstring& wstrLayerTag);

public:
	static CLevel_Logo*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrCloneObjFilePath);
	virtual void			Free() override;
};

END