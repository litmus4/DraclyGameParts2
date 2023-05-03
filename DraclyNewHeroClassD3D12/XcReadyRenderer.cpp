#include "XcReadyRenderer.h"
#include "d3dx12.h"
#include <string>

#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720

enum ECbvSrvRange
{
	Cbv,
	Srv,
	Max
};

XcReadyRenderer::SVertex::SVertex() : uTexIndex(0)
{
	//
}

XcReadyRenderer::SVertex::SVertex(float fX, float fY, float fZ,
	float fNormX, float fNormY, float fNormZ,
	float fU, float fV) : uTexIndex(0)
{
	v3Position.x = fX;
	v3Position.y = fY;
	v3Position.z = fZ;
	v3Normal.x = fNormX;
	v3Normal.y = fNormY;
	v3Normal.z = fNormZ;
	v2Texcoord.x = fU;
	v2Texcoord.y = fV;
}

XcReadyRenderer::XcReadyRenderer()
	: m_uRtvDescriptorSize(0)
	, m_uDsvDescriptorSize(0)
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

	D3D12_DESCRIPTOR_HEAP_DESC DsvHDesc = {};
	DsvHDesc.NumDescriptors = FRAME_COUNT;
	DsvHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DsvHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(m_pDevice->CreateDescriptorHeap(&DsvHDesc, IID_PPV_ARGS(&m_pDsvHeap))))
		return E_FAIL;
	m_uDsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	DXGI_FORMAT eDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D12_RESOURCE_DESC DsDesc = {};
	DsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DsDesc.Width = VIEW_WIDTH;
	DsDesc.Height = VIEW_HEIGHT;
	DsDesc.DepthOrArraySize = 1;
	DsDesc.MipLevels = 1;
	DsDesc.Format = eDepthStencilFormat;
#if MSAA
	DsDesc.SampleDesc.Count = (FdMsaaQ.NumQualityLevels > 0 ? 4 : 1);
	DsDesc.SampleDesc.Quality = (FdMsaaQ.NumQualityLevels > 0 ? FdMsaaQ.NumQualityLevels - 1 : 0);
#else
	DsDesc.SampleDesc.Count = 1;
	DsDesc.SampleDesc.Quality = 0;
#endif
	DsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_HEAP_PROPERTIES DsHeapProp(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE DsClear;
	DsClear.Format = eDepthStencilFormat;
	DsClear.DepthStencil.Depth = 1.0f;
	DsClear.DepthStencil.Stencil = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE DsvHandle(m_pDsvHeap->GetCPUDescriptorHandleForHeapStart());
	for (u = 0; u < FRAME_COUNT; ++u)
	{
		if (FAILED(m_pDevice->CreateCommittedResource(&DsHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&DsDesc,
				D3D12_RESOURCE_STATE_COMMON,
				&DsClear,
				IID_PPV_ARGS(&m_pDepthStencils[u]))))
			return E_FAIL;

		m_pDevice->CreateDepthStencilView(m_pDepthStencils[u].Get(), nullptr, DsvHandle);
		//CommandList ResourceBarrier D3D12_RESOURCE_STATE_COMMON->D3D12_RESOURCE_STATE_DEPTH_WRITE
		DsvHandle.Offset(1, m_uDsvDescriptorSize);
	}

	return S_OK;
}

HRESULT XcReadyRenderer::LoadAssets(const std::vector<SVertex>& vecVertices, const std::vector<UINT16>& vecIndices,
	const std::vector<UINT16>* pvecMultiTexVerts, bool bTiangleStrip)
{
	if (vecVertices.empty() || vecIndices.empty())
		return E_FAIL;
	if (pvecMultiTexVerts)
	{
		UINT16 uSum = 0;
		std::vector<UINT16>::const_iterator itMtv = pvecMultiTexVerts->begin();
		for (; itMtv != pvecMultiTexVerts->end(); itMtv++)
			uSum += *itMtv;
		if (uSum != vecVertices.size())
			return E_FAIL;
	}

	CD3DX12_HEAP_PROPERTIES BufferHeapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RANGE ReadRange(0, 0);

	D3D12_RESOURCE_DESC VbDesc = {};
	VbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	VbDesc.Width = vecVertices.size() * sizeof(SVertex);
	VbDesc.Height = 1;
	VbDesc.DepthOrArraySize = 1;
	VbDesc.MipLevels = 1;
	VbDesc.Format = DXGI_FORMAT_UNKNOWN;
	VbDesc.SampleDesc.Count = 1;
	VbDesc.SampleDesc.Quality = 0;
	VbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	VbDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(m_pDevice->CreateCommittedResource(&BufferHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&VbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer))))
		return E_FAIL;

	UINT8* pVertexBufferGPUData = nullptr;
	if (FAILED(m_pVertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexBufferGPUData))))
		return E_FAIL;
	SVertex* pVertexBufferData = new SVertex[vecVertices.size()];
	std::vector<SVertex>::const_iterator itVert = vecVertices.begin();
	int i = 0, j = 0;
	UINT16 uSum = (pvecMultiTexVerts ? (*pvecMultiTexVerts)[j] : vecVertices.size());
	for (; itVert != vecVertices.end(); itVert++)
	{
		pVertexBufferData[i] = *itVert;
		pVertexBufferData[i].uTexIndex = j;
		i++;
		if (pvecMultiTexVerts && i >= uSum && j < pvecMultiTexVerts->size() - 1)
			uSum += (*pvecMultiTexVerts)[++j];
	}
	memcpy(pVertexBufferGPUData, pVertexBufferData, sizeof(pVertexBufferData));
	m_pVertexBuffer->Unmap(0, nullptr);
	delete[] pVertexBufferData;

	D3D12_RESOURCE_DESC IbDesc = {};
	IbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	IbDesc.Width = vecIndices.size() * sizeof(UINT16);
	IbDesc.Height = 1;
	IbDesc.DepthOrArraySize = 1;
	IbDesc.MipLevels = 1;
	IbDesc.Format = DXGI_FORMAT_UNKNOWN;//DXGI_FORMAT_R16_UINT
	IbDesc.SampleDesc.Count = 1;
	IbDesc.SampleDesc.Quality = 0;
	IbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	IbDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(m_pDevice->CreateCommittedResource(&BufferHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&IbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pIndexBuffer))))
		return E_FAIL;

	UINT8* pIndexBufferGPUData = nullptr;
	if (FAILED(m_pIndexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pIndexBufferGPUData))))
		return E_FAIL;
	UINT16* pIndexBufferData = new UINT16[vecIndices.size()];
	std::vector<UINT16>::const_iterator itInd = vecIndices.begin();
	i = 0;
	for (; itInd != vecIndices.end(); itInd++)
	{
		pIndexBufferData[i] = *itInd;
		i++;
	}
	memcpy(pIndexBufferGPUData, pIndexBufferData, sizeof(pIndexBufferData));
	m_pIndexBuffer->Unmap(0, nullptr);
	delete[] pIndexBufferData;

	//==========

	CD3DX12_ROOT_PARAMETER RootParameters[1];

	CD3DX12_DESCRIPTOR_RANGE CbvSrvRanges[ECbvSrvRange::Max];
	CbvSrvRanges[ECbvSrvRange::Cbv].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CbvSrvRanges[ECbvSrvRange::Srv].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0);
	RootParameters[0].InitAsDescriptorTable(2, CbvSrvRanges);

	CD3DX12_ROOT_SIGNATURE_DESC RsDesc(1, RootParameters, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> pSerializedRootSig;
	ComPtr<ID3DBlob> pErrorBlob;
	if (FAILED(D3D12SerializeRootSignature(&RsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRootSig, &pErrorBlob)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateRootSignature(0,
		pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_pRootSignature))))
		return E_FAIL;

	//FLAGJK Shaders

	D3D12_INPUT_ELEMENT_DESC IeDescs[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXINDEX", 0, DXGI_FORMAT_R16_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	//FLAGJK

	return S_OK;
}