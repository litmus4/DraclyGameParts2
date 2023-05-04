#pragma once
#include <wrl/client.h>
#include "windows.h"
#include "dxgi1_4.h"
#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"
#include <vector>
#include <string>

#define INDEX_RECT(Vec, I1, I2, I3, I4, I5, I6) Vec.push_back(I1);\
	Vec.push_back(I2);\
	Vec.push_back(I3);\
	Vec.push_back(I4);\
	Vec.push_back(I5);\
	Vec.push_back(I6)

#define FRAME_COUNT 2
#define MSAA 0

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class XcReadyRenderer
{
public:
	struct SVertex
	{
		SVertex();
		SVertex(float fX, float fY, float fZ,
			float fNormX, float fNormY, float fNormZ,
			float fU, float fV);

		XMFLOAT3 v3Position;
		XMFLOAT3 v3Normal;
		XMFLOAT2 v2Texcoord;
		UINT16 uTexIndex;
	};

public:
	XcReadyRenderer();
	~XcReadyRenderer();

	HRESULT InitPipeline(HWND hWnd);
	HRESULT LoadAssets(const std::vector<SVertex>& vecVertices, const std::vector<UINT16>& vecIndices,
		const std::vector<std::wstring>& vecTexFiles,
		const std::vector<UINT16>* pvecMultiTexVerts = nullptr, bool bTiangleStrip = false);

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

	ComPtr<ID3D12Resource> m_pVertexBuffer;
	ComPtr<ID3D12Resource> m_pIndexBuffer;
	ComPtr<ID3D12RootSignature> m_pRootSignature;
	UINT m_uCbvSrvDescriptorSize;

	UINT m_uFrameIndex;
	//
};