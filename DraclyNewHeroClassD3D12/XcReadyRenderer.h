#pragma once
#include <wrl/client.h>
#include "windows.h"
#include "dxgi1_4.h"
#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"

#define FRAME_COUNT 2
#define MSAA 0

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class XcReadyRenderer
{
public:
	XcReadyRenderer();
	~XcReadyRenderer();

	HRESULT InitPipeline(HWND hWnd);
	HRESULT LoadAssets();

private:
	ComPtr<ID3D12Device> m_pDevice;
	ComPtr<IDXGISwapChain3> m_pSwapChain;
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[FRAME_COUNT];
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap;
	UINT m_uRtvDescriptorSize;
	ComPtr<ID3D12Resource> m_pDepthStencils[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap;
	UINT m_uDsvDescriptorSize;

	UINT m_uFrameIndex;
	//
};