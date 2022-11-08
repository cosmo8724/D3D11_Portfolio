#include "..\public\Graphic_Device.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{		
	
}

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(hWnd, WinMode, iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 셋팅한다. */
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);
	
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (float)iWinCX;
	ViewPortDesc.Height = (float)iWinCY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	m_pDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CGraphicDevice::ID3D11Device") - 1, "CGraphicDevice::ID3D11Device");
	m_pDeviceContext->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CGraphicDevice::ID3D11DeviceContext") - 1, "CGraphicDevice::ID3D11DeviceContext");

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 백버퍼를 초기화한다.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	


 	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	/*static _bool	bStandByMode = false;
	HRESULT		hr;

	if (bStandByMode)
	{
		hr = m_pSwapChain->Present(0, DXGI_PRESENT_TEST);

		if (hr == DXGI_STATUS_OCCLUDED)
			return DXGI_STATUS_OCCLUDED;

		bStandByMode = false;
	}
	
	hr = m_pSwapChain->Present(0, 0);

	if (hr == DXGI_STATUS_OCCLUDED)
		bStandByMode = true;

	return hr;	*/

	int a = ImGui::GetCurrentContext()->Viewports.Size;
	a = ImGui::GetCurrentContext()->PlatformIO.Viewports.Size;

	return m_pSwapChain->Present(0, 0);
}


HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice*			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*			pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*			pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));
			
	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;
	SwapChain.BufferDesc.RefreshRate.Numerator = 0;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;
	SwapChain.OutputWindow = hWnd;	
	SwapChain.Windowed = eWinMode;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;	

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;
	
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;	

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	/*m_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	//Safe_Release(m_pBackBufferRTV);
	//Safe_Release(m_pDepthStencilView);

	DXGI_MODE_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_MODE_DESC));

	SwapChain.Width = iWinCX;
	SwapChain.Height = iWinCY;
	SwapChain.RefreshRate.Numerator = 60;
	SwapChain.RefreshRate.Denominator = 1;
	SwapChain.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	BOOL		bIsFullScreen;
	m_pSwapChain->GetFullscreenState(&bIsFullScreen, nullptr);

	m_pSwapChain->SetFullscreenState(bIsFullScreen, nullptr);
	m_pSwapChain->ResizeTarget(&SwapChain);*/

	BOOL	bIsFullScreen = false;
	m_pSwapChain->GetFullscreenState(&bIsFullScreen, nullptr);

	if (bIsFullScreen)
	{
		ImGuiContext*	CurContext = ImGui::GetCurrentContext();
		ImGuiViewportP*	ToolWindowViewport = nullptr;
		
		ImVector<ImGuiWindow*>*		Windows = &ImGui::GetCurrentContext()->Windows;
		ImVector<ImGuiViewportP*>*	Viewports = &ImGui::GetCurrentContext()->Viewports;
		for (auto Window : *Windows)
		{
			if (fabs(Window->SizeFull.x - ImGui::GetMainViewport()->Size.x < 0.00001f) && fabs(Window->SizeFull.y - ImGui::GetMainViewport()->Size.y < 0.00001f))
				ToolWindowViewport = Window->Viewport;

			if (Window->ViewportOwned)
			{
				if (Window->Viewport == ImGui::GetCurrentContext()->MouseLastHoveredViewport)
					ImGui::GetCurrentContext()->MouseLastHoveredViewport = nullptr;

				//ImGui::DestroyPlatformWindow(Window->Viewport);
				//IM_ASSERT(ImGui::GetCurrentContext()->PlatformIO.Viewports.contains(Window->Viewport) == false);
				//IM_ASSERT(ImGui::GetCurrentContext()->Viewports[Window->Viewport->Idx] == Window->Viewport);
				//ImGui::GetCurrentContext()->Viewports.erase(ImGui::GetCurrentContext()->Viewports.Data + Window->Viewport->Idx);
				//IM_DELETE(Window->Viewport);

				//Window->Viewport = ToolWindowViewport;
				//Window->ViewportOwned = false;
				//ImGui::GetCurrentContext()->FrameCountPlatformEnded--;
				//ImGui::UpdatePlatformWindows();
				
				Window->ViewportOwned = false;

				/*for (auto Viewport = ImGui::GetCurrentContext()->PlatformIO.Viewports.begin(); Viewport != ImGui::GetCurrentContext()->PlatformIO.Viewports.end();)
				{
					if ((*Viewport)->ID == Window->Viewport->ID)
					{
						Viewport = ImGui::GetCurrentContext()->PlatformIO.Viewports.erase(Viewport);
						ImGui::GetCurrentContext()->PlatformIO.Viewports.Size--;
						continue;
					}
					Viewport++;
				}*/

				/*Window->Viewport = ToolWindowViewport;
				Window->ViewportId = ToolWindowViewport->ID;
				Window->ViewportPos = ToolWindowViewport->Pos;
				Window->ViewportOwned = false;
				Window->BeginOrderWithinContext = 2;*/
			}
		}

		iWinCX = GetSystemMetrics(SM_CXSCREEN);
		iWinCY = GetSystemMetrics(SM_CYSCREEN);

		/*for (auto Viewport : CurContext->Viewports)
		{
			if (Viewport->ID != ImGui::GetMainViewport()->ID
				&& ToolWindowViewport)
			{
				ImGuiViewportP*	LastViewport = Viewport;

				Viewport = ToolWindowViewport;
				Viewport->Size;
			}
		}*/

		//ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
	}
	else
	{
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	if (m_pBackBufferRTV)
		Safe_Release(m_pBackBufferRTV);
	if (m_pDepthStencilView)
		Safe_Release(m_pDepthStencilView);

	m_pSwapChain->ResizeBuffers(0, iWinCX, iWinCY, DXGI_FORMAT_UNKNOWN, 0);

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (float)iWinCX;
	ViewPortDesc.Height = (float)iWinCY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif

	Safe_Release(m_pDevice);
}
