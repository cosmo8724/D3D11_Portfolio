#include "ImGuiMgr.h"
#include "Tool.h"
#include "GameInstance.h"

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
																										
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

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
	if (CGameInstance::GetInstance()->Key_Down(DIK_T) && CGameInstance::GetInstance()->Get_DIKeyState(DIK_LCONTROL) & 0x80)
		m_bDrawImGui = !m_bDrawImGui;

	if (!m_bDrawImGui)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_dTimeDelta = dTimeDelta;

	ImGui_DockSpace();
	ImGui_RenderTab();
	ImGui_RenderWindow();
}

void CImGuiMgr::ImGui_Render()
{
	if (!m_bDrawImGui)
		return;

	ImGui::Render();
}

void CImGuiMgr::ImGui_Render_Update()
{
	if (!m_bDrawImGui)
		return;

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

void CImGuiMgr::ImGui_DockSpace()
{
	ImGuiWindowFlags			WindowFlag = ImGuiWindowFlags_NoDocking;

	const ImGuiViewport*	Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->WorkPos);
	ImGui::SetNextWindowSize(Viewport->WorkSize);
	ImGui::SetNextWindowViewport(Viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	WindowFlag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	WindowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	WindowFlag |= ImGuiDockNodeFlags_PassthruCentralNode;
	WindowFlag |= ImGuiWindowFlags_NoBackground;

	_bool	bIsShow = true;

	ImGui::Begin("DockSpace", &bIsShow, WindowFlag);
	ImGui::PopStyleVar(1);
	ImGui::PopStyleVar(2);

	ImGuiIO&	io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID	DockSpaceID = ImGui::GetID("DockSpace");
		ImGuiDockNodeFlags Flag = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(DockSpaceID, ImVec2(0.f, 0.f), Flag);
	}

	ImGui::End();
}

void CImGuiMgr::Free()
{
	Clear_ImGuiObject();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
