#pragma once
#include "windows.h"
//#include "d3d12.h"
//#include "d3dx12.h"
#include "DirectXMath.h"
#include "Xinput.h"

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
	//äÖÈ¾Æ÷
	//XcReadyRenderer* m_pRenderer;
	
	//XInput
	//
	
	//Å·À­½Ç
	float m_fYaw;
	float m_fPitch;
	float m_fRoll;
};