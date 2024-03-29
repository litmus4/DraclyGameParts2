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

	struct SConstantParam
	{
		SConstantParam();
		SConstantParam(const XMMATRIX& xWorld,
			const XMFLOAT3& xEye, const XMFLOAT3& xLookAt, const XMFLOAT3& xRight);

		XMMATRIX matWorld;
		XMFLOAT3 v3Eye;
		XMFLOAT3 v3LookAt;
		XMFLOAT3 v3Right;
	};
	struct SConstants
	{
		XMMATRIX matWorldViewProj;
		XMMATRIX matWorld;
		XMFLOAT3 v3Eye;
	};

public:
	XcReadyRenderer();
	~XcReadyRenderer();

	HRESULT InitPipeline(HWND hWnd);
	HRESULT LoadAssets(const std::vector<SVertex>& vecVertices, const std::vector<UINT16>& vecIndices,
		const std::vector<std::wstring>& vecTexFiles, const SConstantParam& ConstantParam,
		const std::vector<UINT16>* pvecMultiTexVerts = nullptr, bool bTiangleStrip = false);

private:
	struct SShaderBlobGroup
	{
		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pErrorVS;
		ComPtr<ID3DBlob> pHullShader;
		ComPtr<ID3DBlob> pErrorHS;
		ComPtr<ID3DBlob> pDomainShader;
		ComPtr<ID3DBlob> pErrorDS;
		ComPtr<ID3DBlob> pGeometryShader;
		ComPtr<ID3DBlob> pErrorGS;
		ComPtr<ID3DBlob> pPixelShader;
		ComPtr<ID3DBlob> pErrorPS;
	};

private:
	HRESULT CompileShaders(LPCWSTR wszFile,
		LPCSTR szVSEntry, LPCSTR szHSEntry, LPCSTR szDSEntry, LPCSTR szGSEntry, LPCSTR szPSEntry);
	UINT CalcConstantBufferByteSize(UINT uByteSize);

#if MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_FdMsaaQ = {};
#endif

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
	bool m_bDsvResStateInited[FRAME_COUNT];

	ComPtr<ID3D12Resource> m_pVertexBuffer;
	ComPtr<ID3D12Resource> m_pIndexBuffer;
	ComPtr<ID3D12RootSignature> m_pRootSignature;
	SShaderBlobGroup m_Shaders;
	ComPtr<ID3D12PipelineState> m_pPSO;
	SConstants m_Constants;
	UINT m_uCbvSrvDescriptorSize;

	UINT m_uFrameIndex;
	//
};