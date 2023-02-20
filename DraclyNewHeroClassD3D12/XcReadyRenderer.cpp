#include "XcReadyRenderer.h"
#include "d3dx12.h"
#include <string>

#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720

XcReadyRenderer::XcReadyRenderer()
	: m_uRtvDescriptorSize(0)
	, m_uFrameIndex(0)
{
	//
}

XcReadyRenderer::~XcReadyRenderer()
{
	//
}

HRESULT XcReadyRenderer::InitPipeline(HWND hWnd)
{
	UINT uDxgiFactoryFlags = 0;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> pDebug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug))))
	{
		pDebug->EnableDebugLayer();

		uDxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory4> pFactory;
	if (FAILED(CreateDXGIFactory2(uDxgiFactoryFlags, IID_PPV_ARGS(&pFactory))))
		return E_FAIL;

	IDXGIAdapter1* pAdapter = nullptr;
	UINT u = 0;
	DXGI_MODE_DESC dMDesc;
	dMDesc.Width = 0;
	while (pFactory->EnumAdapters1(u, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC dADesc;
		pAdapter->GetDesc(&dADesc);

		std::wstring wstrDesc = dADesc.Description;
		if (wstrDesc.find_first_of(L"Geforce", 0) != std::wstring::npos ||
			wstrDesc.find_first_of(L"Radeon", 0) != std::wstring::npos ||
			wstrDesc.find_first_of(L"Arc", 0) != std::wstring::npos)
		{
			IDXGIOutput* pOutput = nullptr;
			UINT v = 0;
			while (pAdapter->EnumOutputs(v, &pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				//DXGI_OUTPUT_DESC dODesc;
				//pOutput->GetDesc(&dODesc);

				UINT uModeNum = 0;
				pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &uModeNum, nullptr);

				DXGI_MODE_DESC* pdMDescs = new DXGI_MODE_DESC[uModeNum];
				pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &uModeNum, pdMDescs);

				for (UINT w = 0; w < uModeNum; ++w)
				{
					DXGI_MODE_DESC& CurdMDesc = pdMDescs[w];
					if (CurdMDesc.Width > dMDesc.Width || CurdMDesc.Height > dMDesc.Height)
						dMDesc = CurdMDesc;
					else if (CurdMDesc.Width == dMDesc.Width && CurdMDesc.Height == dMDesc.Height)
					{
						if (CurdMDesc.RefreshRate.Numerator < dMDesc.RefreshRate.Numerator)
							dMDesc = CurdMDesc;
					}
				}

				delete[] pdMDescs;
				break;//v++;
			}
			break;
		}

		u++;
	}
	if (!pAdapter || dMDesc.Width == 0)
		return E_FAIL;

	if (FAILED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice))))
		return E_FAIL;

	D3D12_COMMAND_QUEUE_DESC CqDesc = {};
	CqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_pDevice->CreateCommandQueue(&CqDesc, IID_PPV_ARGS(&m_pCommandQueue))))
		return E_FAIL;

#if MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS FdMsaaQ;
	FdMsaaQ.Format = dMDesc.Format;
	FdMsaaQ.SampleCount = 4;
	FdMsaaQ.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	FdMsaaQ.NumQualityLevels = 0;
	if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &FdMsaaQ, sizeof(FdMsaaQ))))
		return E_FAIL;
#endif

	DXGI_SWAP_CHAIN_DESC ScDesc = {};
	ScDesc.BufferDesc = dMDesc;
	ScDesc.BufferDesc.Width = VIEW_WIDTH;
	ScDesc.BufferDesc.Height = VIEW_HEIGHT;
	//ScDesc.BufferDesc.RefreshRate.Numerator = 60;
	//ScDesc.BufferDesc.RefreshRate.Denominator = 1;
	//ScDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
#if MSAA
	ScDesc.SampleDesc.Count = (FdMsaaQ.NumQualityLevels > 0 ? 4 : 1);
	ScDesc.SampleDesc.Quality = (FdMsaaQ.NumQualityLevels > 0 ? FdMsaaQ.NumQualityLevels - 1 : 0);
#else
	ScDesc.SampleDesc.Count = 1;
	ScDesc.SampleDesc.Quality = 0;
#endif
	ScDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	ScDesc.BufferCount = FRAME_COUNT;
	ScDesc.OutputWindow = hWnd;
	ScDesc.Windowed = true;
	ScDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ScDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain> pSwapChain;
	if (FAILED(pFactory->CreateSwapChain(m_pCommandQueue.Get(), &ScDesc, &pSwapChain)))
		return E_FAIL;
	if (FAILED(pSwapChain.As(&m_pSwapChain)))
		return E_FAIL;
	m_uFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC RtvHDesc = {};
	RtvHDesc.NumDescriptors = FRAME_COUNT;
	RtvHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RtvHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(m_pDevice->CreateDescriptorHeap(&RtvHDesc, IID_PPV_ARGS(&m_pRtvHeap))))
		return E_FAIL;
	m_uRtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(m_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (u = 0; u < FRAME_COUNT; ++u)
	{
		if (FAILED(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[u]))))
			return E_FAIL;

		if (FAILED(m_pSwapChain->GetBuffer(u, IID_PPV_ARGS(&m_pRenderTargets[u]))))
			return E_FAIL;
		m_pDevice->CreateRenderTargetView(m_pRenderTargets[u].Get(), nullptr, RtvHandle);
		RtvHandle.Offset(1, m_uRtvDescriptorSize);
	}

	return S_OK;
}

//FLAGJK