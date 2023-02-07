#include "XcReadyRenderer.h"

XcReadyRenderer::XcReadyRenderer()
{
	//
}

XcReadyRenderer::~XcReadyRenderer()
{
	//
}

HRESULT XcReadyRenderer::InitPipeline(HWND hWnd)
{
	ComPtr<IDXGIFactory4> pFactory;
	if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory))))
		return E_FAIL;

	UINT u = 0;
	IDXGIAdapter1* pAdapter;
	while (pFactory->EnumAdapters1(u, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC Desc;
		pAdapter->GetDesc(&Desc);

		u++;//FLAGJK
	}
}