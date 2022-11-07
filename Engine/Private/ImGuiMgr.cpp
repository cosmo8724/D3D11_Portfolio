#include "ImGuiMgr.h"

IMPLEMENT_SINGLETON(CImGuiMgr)

CImGuiMgr::CImGuiMgr()
{
}

HRESULT CImGuiMgr::Ready_ImGui(HWND hWnd, ID3D11Device* pGraphicDev, ID3D11DeviceContext* pDeviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
																
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   
																										
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pGraphicDev, pDeviceContext);
	
	return S_OK;
}

void CImGuiMgr::ImGui_NewFrame(_double dTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_dTimeDelta = dTimeDelta;

	for (size_t i = 0; i < m_vecTool.size(); ++i)
		m_vecTool[i]->Update_Tool(dTimeDelta);
}

void CImGuiMgr::ImGui_Render()
{
	ImGui::Render();
}

void CImGuiMgr::ImGui_Render_Update()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

HRESULT CImGuiMgr::Add_Tool(CTool * pTool)
{
	NULL_CHECK_RETURN(pTool, E_FAIL);

	m_vecTool.push_back(pTool);

	return S_OK;
}

void CImGuiMgr::Free()
{
	if (m_vecTool.size())
	{
		for (size_t i = 0; i < m_vecTool.size(); ++i)
			Safe_Release(m_vecTool[i]);
		m_vecTool.clear();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
