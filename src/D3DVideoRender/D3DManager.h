/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/



#pragma once

#include "d3dfont.h"
#include "Myd3dfont.h"
#include "D3DVideoRenderInterface.h"
#include "RenderFactory.h"
#include "Display.h"
#include <list>

class CD3DManager : public ID3DVRInterface
{
public:
	enum
	{
		SHADER_YUV420 = 0,
		SHADER_YUV422 = 1,
		SHADER_NV12 = 2,
		SHADER_RGB24 = 3,
		SHADER_COUNT = 4 
	} ;

public:
	bool CreateBackBuffer ( HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount ) ;
	
	bool CreateImageBuffer ( DWORD *dwIndex, long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect ) ;
	bool DestroyImageBuffer ( DWORD dwIndex ) ;
	bool CreateOverlayText ( DWORD *dwIndex, const char *strText, LOGFONT *logfont ) ;
	bool DestroyOverlayText ( DWORD dwIndex ) ;
	bool GetOverlayTextWH ( DWORD dwIndex, long *lWidth, long *lHeight ) ;

	bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) ;
	bool DestroyDynamicFont ( void ) ;

	bool ColorFill ( DWORD dwIndex, BYTE r, BYTE g, BYTE b ) ;

	bool UpdateImage ( DWORD dwIndex, BYTE *pData ) ;

	bool BeginRender ( bool bClearBackBuffer, DWORD dwBKColor ) ;

	bool DrawImage ( DWORD dwIndex, GEOMETRIC_TRANSFORMATION Transformation, RECT *rect ) ;
	bool DrawOverlayText ( DWORD dwIndex, DWORD dwColor, RECT *rect ) ;
	bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) ;
		
	bool EndRender (void) ;
		
	CD3DManager(void);
	~CD3DManager(void);

private:	

	static CDisplay *CreateRender ( ID3DVRInterface::COLOR_SPACE cs, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader )
	{
		extern CRenderFactory g_RenderFactory ;
		return g_RenderFactory.CreateRender ( cs, pDevice, pShader ) ;
	}

	bool CreateDevice(HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount);
	
	void Destroy(void);
	void DestroyFont(void);
	void DestroyDisplay(void);
	void DestroyShader(void);
	void DestroyDevice(void);

	bool SetDefault (void) ;

	bool HandleDeviceLost(void);
	bool RestoreDeviceObjects(void);
	void InvalidateDeviceObjects(void);

private:
	
	LPDIRECT3D9 m_pD3D ;
	LPDIRECT3DDEVICE9 m_pDevice ;
	D3DPRESENT_PARAMETERS m_PresentParams ;
	
	CPixelShader *m_pPixelShader[SHADER_COUNT] ;

	std::list<CDisplay*> m_DisplayList ;
	std::list<CMYD3DFont*> m_MyFontList ;
	CD3DFont *m_pFont ;

	bool m_bDeviceLost ;
	DWORD m_dwPrevTime ;
};