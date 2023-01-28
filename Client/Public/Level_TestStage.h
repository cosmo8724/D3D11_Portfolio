#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_TestStage final : public CLevel
{
private:
	CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel_TestStage() = default;

public:
	virtual HRESULT				Initialize(const wstring& wstrCloneObjFilePath);
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	HRESULT						Load_CloneObjects(const wstring& wstrCloneObjFilePath);
	HRESULT						Ready_Light();
	HRESULT						Ready_Layer_Ocean(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_Camera(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_SkyBox(const wstring &wstrLayerTag);
	HRESULT						Ready_Layer_Player(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_NPCs(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_Islands(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_Enemies(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_Objects(const wstring& wstrLayerTag);
	HRESULT						Ready_Layer_UI(const wstring& wstrLayerTag);

public:
	static CLevel_TestStage*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrCloneObjFilePath);
	virtual void					Free() override;
};

END