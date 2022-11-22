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
	void		ImGui_Render();
	void		ImGui_Render_Update();
	
public:
	HRESULT	Add_ImGuiTabObject(class CTool* pTool);
	HRESULT	Add_ImGuiWindowObejct(class CTool* pTool);
	void		Clear_ImGuiObject();

private:
	void		ImGui_RenderTab();
	void		ImGui_RenderWindow();

private:
	_double					m_dTimeDelta;

	vector<class CTool*>			m_vecTab;
	vector<class CTool*>			m_vecWindow;

public:
	virtual		void	Free() override;
};

END