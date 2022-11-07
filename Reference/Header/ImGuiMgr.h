#pragma once
#include "Base.h"
#include "Tool.h"

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
	HRESULT	Add_Tool(CTool* pTool);

public:
	virtual		void	Free() override;

private:
	_double					m_dTimeDelta;
	vector<CTool*>		m_vecTool;
};

END