#include "PscHero12.h"

PscHero12::PscHero12()
	//: m_pRenderer(nullptr)
	//, m_fYaw(0.0f), m_fPitch(0.0f), m_fRoll(0.0f)
{
	m_v3Front = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_v3Right = XMFLOAT3(-1.0f, 0.0f, 0.0f);
}

PscHero12::~PscHero12()
{
	//if (m_pRenderer)
	//{
	//	delete m_pRenderer;
	//	m_pRenderer = nullptr;
	//}
}

HRESULT PscHero12::Init(HWND hWnd, HINSTANCE hInst)
{
	//

	return S_OK;
}

HRESULT PscHero12::Load()
{
	//

	return S_OK;
}

void PscHero12::Update()
{
	XMVECTOR&& vFront = DirectX::XMLoadFloat3(&m_v3Front);
	XMVECTOR&& vRight = DirectX::XMLoadFloat3(&m_v3Right);
	XMVECTOR&& vUp = DirectX::XMVector3Cross(vFront, vRight);
	vFront = DirectX::XMVector3Normalize(vFront);
	vRight = DirectX::XMVector3Normalize(vRight);
	vUp = DirectX::XMVector3Normalize(vUp);
	//FLAGJK 如果vFront·vRight不大概为0，则利用Cross把vRight弄的与vFront垂直
	XMMATRIX&& matRot = XMMatrixSet(
		DirectX::XMVectorGetX(vRight), DirectX::XMVectorGetY(vRight), DirectX::XMVectorGetZ(vRight), 0.0f,
		DirectX::XMVectorGetX(vUp), DirectX::XMVectorGetY(vUp), DirectX::XMVectorGetZ(vUp), 0.0f,
		DirectX::XMVectorGetX(vFront), DirectX::XMVectorGetY(vFront), DirectX::XMVectorGetZ(vFront), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

HRESULT PscHero12::Render()
{
	//

	return S_OK;
}

HRESULT PscHero12::Clear()
{
	//

	return S_OK;
}

HRESULT PscHero12::Destroy()
{
	//

	return S_OK;
}

void PscHero12::KbAcquire()
{
	//
}