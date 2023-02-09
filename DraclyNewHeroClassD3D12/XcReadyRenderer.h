#pragma once
#include <wrl/client.h>
#include "windows.h"
#include "dxgi1_4.h"
#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"

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
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
};