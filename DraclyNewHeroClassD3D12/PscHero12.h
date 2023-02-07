#pragma once
#include "windows.h"
//#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"
#include "Xinput.h"

using namespace DirectX;

class XcReadyRenderer;

class PscHero12
{
public:
	PscHero12();
	~PscHero12();

	HRESULT Init(HWND hWnd, HINSTANCE hInst);
	HRESULT Load();
	void Update();
	HRESULT Render();
	HRESULT Clear();
	HRESULT Destroy();
	void KbAcquire();

private:
	void RotateVector3(XMVECTOR& v, const XMVECTOR& vAxis, float fAngle);

	//渲染器
	XcReadyRenderer* m_pRenderer;
	
	//XInput
	//
	
	//欧拉角
	//float m_fYaw;
	//float m_fPitch;
	//float m_fRoll;
	//旋转向量
	XMVECTOR m_vFront;
	XMVECTOR m_vRight;
};