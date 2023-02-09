#include "XcReadyRenderer.h"
#include <string>

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

	IDXGIAdapter* pAdapter = nullptr;
	UINT u = 0;
	DXGI_MODE_DESC dMDesc;
	dMDesc.Width = 0;
	while (pFactory->EnumAdapters(u, &pAdapter) != DXGI_ERROR_NOT_FOUND)
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
				pOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &uModeNum, nullptr);

				DXGI_MODE_DESC* pdMDescs = new DXGI_MODE_DESC[uModeNum];
				pOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &uModeNum, pdMDescs);

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

	//FLAGJK
}