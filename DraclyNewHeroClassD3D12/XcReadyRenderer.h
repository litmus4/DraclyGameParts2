#pragma once
#include <wrl/client.h>
#include "windows.h"
#include "dxgi1_4.h"
#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"

#define FRAME_COUNT 2

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class XcReadyRenderer
{
public:
	XcReadyRenderer();
	~XcReadyRenderer();

	HRESULT InitPipeline(HWND hWnd);

private:
	ComPtr<ID3D12Device> m_pDevice;
	ComPtr<IDXGISwapChain3> m_pSwapChain;
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap;
	UINT m_uRtvDescriptorSize;

	UINT m_uFrameIndex;
	//
};