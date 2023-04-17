#include "PscHero12.h"
#include "XcReadyRenderer.h"

PscHero12::PscHero12()
	: m_pRenderer(nullptr)
	//, m_fYaw(0.0f), m_fPitch(0.0f), m_fRoll(0.0f)
{
	XMFLOAT3 v3Front(0.0f, 0.0f, -1.0f);
	m_vFront = DirectX::XMLoadFloat3(&v3Front);
	XMFLOAT3 v3Right(-1.0f, 0.0f, 0.0f);
	m_vRight = DirectX::XMLoadFloat3(&v3Right);
}

PscHero12::~PscHero12()
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}

HRESULT PscHero12::Init(HWND hWnd, HINSTANCE hInst)
{
	m_pRenderer = new XcReadyRenderer();
	if (FAILED(m_pRenderer->InitPipeline(hWnd)))
		return E_FAIL;

	return S_OK;
}

HRESULT PscHero12::Load()
{
	std::vector<XcReadyRenderer::SVertex> vecVertices;
	//前面
	vecVertices.push_back(XcReadyRenderer::SVertex(-1.0f, 1.0f, -1.0f, /**/-1.0f, 0.0f, 0.0f, /**/0.0f, 0.0f));
	vecVertices.push_back(XcReadyRenderer::SVertex(1.0f, 1.0f, -1.0f, /**/-1.0f, 0.0f, 0.0f, /**/1.0f, 0.0f));
	vecVertices.push_back(XcReadyRenderer::SVertex(1.0f, -1.0f, -1.0f, /**/-1.0f, 0.0f, 0.0f, /**/1.0f, 1.0f));
	vecVertices.push_back(XcReadyRenderer::SVertex(-1.0f,-1.0f, -1.0f, /**/-1.0f, 0.0f, 0.0f, /**/0.0f, 1.0f));
	//

	std::vector<UINT16> vecIndices;
	//前面
	INDEX_RECT(vecIndices, 0, 1, 2, 0, 2, 3);
	//

	if (FAILED(m_pRenderer->LoadAssets(vecVertices, vecIndices)))
		return E_FAIL;

	return S_OK;
}

void PscHero12::Update()
{
	float fRotX = 0.0f, fRotY = 0.0f, fRotZ = 0.0f;
	//
	XMFLOAT3 v3AxisX(1.0f, 0.0f, 0.0f);
	XMVECTOR vAxisX = DirectX::XMLoadFloat3(&v3AxisX);
	XMFLOAT3 v3AxisY(0.0f, 1.0f, 0.0f);
	XMVECTOR vAxisY = DirectX::XMLoadFloat3(&v3AxisY);
	XMFLOAT3 v3AxisZ(0.0f, 0.0f, 1.0f);
	XMVECTOR vAxisZ = DirectX::XMLoadFloat3(&v3AxisZ);
	RotateVector3(m_vFront, vAxisX, fRotX);
	RotateVector3(m_vFront, vAxisY, fRotY);
	RotateVector3(m_vFront, vAxisZ, fRotZ);
	RotateVector3(m_vRight, vAxisX, fRotX);
	RotateVector3(m_vRight, vAxisY, fRotY);
	RotateVector3(m_vRight, vAxisZ, fRotZ);
	
	XMVECTOR&& vUp = DirectX::XMVector3Cross(m_vFront, m_vRight);
	m_vFront = DirectX::XMVector3Normalize(m_vFront);
	m_vRight = DirectX::XMVector3Normalize(m_vRight);
	vUp = DirectX::XMVector3Normalize(vUp);
	if (fabs(DirectX::XMVector3Dot(m_vFront, m_vRight).m128_f32[0]) > 0.000001f)
		m_vRight = DirectX::XMVector3Cross(vUp, m_vFront);
	XMMATRIX&& matWorld = XMMatrixSet(
		DirectX::XMVectorGetX(m_vRight), DirectX::XMVectorGetY(m_vRight), DirectX::XMVectorGetZ(m_vRight), 0.0f,
		DirectX::XMVectorGetX(vUp), DirectX::XMVectorGetY(vUp), DirectX::XMVectorGetZ(vUp), 0.0f,
		DirectX::XMVectorGetX(m_vFront), DirectX::XMVectorGetY(m_vFront), DirectX::XMVectorGetZ(m_vFront), 0.0f,
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

void PscHero12::RotateVector3(XMVECTOR& v, const XMVECTOR& vAxis, float fAngle)
{
	if (fabs(fAngle) < 0.000001f)
		return;
	v = DirectX::XMVector3Rotate(v, DirectX::XMQuaternionRotationAxis(vAxis, fAngle));
}