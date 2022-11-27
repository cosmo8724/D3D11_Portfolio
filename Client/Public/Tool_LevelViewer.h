#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Client)

class CTool_LevelViewer final : public CTool
{
private:
	CTool_LevelViewer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CTool_LevelViewer() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					ImGui_RenderWindow() override;

private:
	DEVICE						m_pDevice = nullptr;
	DEVICE_CONTEXT			m_pContext = nullptr;

	_int							m_iCurrentLevel = 0;
	_int							m_iNextLevel = 0;
	char*							m_pLevelName[LEVEL_END] = { "Loading", "Logo", "Test Stage" };

public:
	static CTool_LevelViewer*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void* pArg = nullptr);
	virtual void					Free() override;
};

END