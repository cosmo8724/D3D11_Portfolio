#include "ImGuiMgr.h"
#include "Tool.h"

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
}

void CImGuiMgr::ImGui_Render()
{
	ImGui_RenderTab();
	ImGui_RenderWindow();

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

HRESULT CImGuiMgr::Add_ImGuiTabObject(CTool * pTool)
{
	NULL_CHECK_RETURN(pTool, E_FAIL);

	m_vecTab.push_back(pTool);

	return S_OK;
}

HRESULT CImGuiMgr::Add_ImGuiWindowObejct(CTool * pTool)
{
	NULL_CHECK_RETURN(pTool, E_FAIL);

	m_vecWindow.push_back(pTool);

	return S_OK;
}

void CImGuiMgr::Clear_ImGuiObject()
{
	for (auto& pTool : m_vecTab)
		Safe_Release(pTool);
	m_vecTab.clear();

	for (auto& pTool : m_vecWindow)
		Safe_Release(pTool);
	m_vecWindow.clear();
}

void CImGuiMgr::ImGui_RenderTab()
{
	if (m_vecTab.empty())
		return;

	ImGui::Begin("Tab Window");
	if (ImGui::BeginTabBar("Manager_Tab", ImGuiTabBarFlags_None))
	{
		for (const auto& ImTabObj : m_vecTab)
		{
			if (ImGui::BeginTabItem(ImTabObj->Get_TabName()))
			{
				ImTabObj->ImGui_RenderTap();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImGuiMgr::ImGui_RenderWindow()
{
	for (const auto& ImWinObj : m_vecWindow)
	{
		ImGui::Begin(ImWinObj->Get_WindowName());
		ImWinObj->ImGui_RenderWindow();
		ImGui::End();
	}
}

void CImGuiMgr::Free()
{
	Clear_ImGuiObject();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
