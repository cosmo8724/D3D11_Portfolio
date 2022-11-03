#pragma once
#include "Base.h"

BEGIN(Engine)

class CImGuiMgr : public CBase
{
	DECLARE_SINGLETON(CImGuiMgr)

private:
	CImGuiMgr();
	virtual ~CImGuiMgr() = default;

public:
	HRESULT	Ready_ImGui(HWND hWnd, ID3D11Device* pGraphicDev, ID3D11DeviceContext* pDeviceContext);
	void		ImGui_NewFrame(_double dTimeDelta);
	void		ImGui_EndFrame();
	void		ImGui_Render();

public:
	virtual		void	Free() override;

private:
	_double					m_dTimeDelta;
};

END