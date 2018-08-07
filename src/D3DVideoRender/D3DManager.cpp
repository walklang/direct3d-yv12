/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/



#include "stdafx.h"
#include "D3DManager.h"

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern IDebug *g_pDebug ;

CD3DManager::CD3DManager(void)
	: m_pD3D(NULL)
	, m_pDevice(NULL)
	, m_bDeviceLost(false)
	, m_dwPrevTime(0)
	, m_pFont(NULL)
{	
	for ( int i = 0 ; i < SHADER_COUNT ; i++ )
	{
		m_pPixelShader[i] = NULL ;
	}

	ZeroMemory ( &m_PresentParams, sizeof(m_PresentParams) ) ;
}

CD3DManager::~CD3DManager(void)
{
	Destroy () ;
}

bool CD3DManager::CreateDevice ( HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount )
{
	do
	{
		m_pD3D = Direct3DCreate9 ( D3D_SDK_VERSION ) ;
		if ( m_pD3D == NULL )
		{
			g_pDebug->Log_Error ( _T("Direct3DCreate9() failed!") ) ;
			break ;
		}

		//*

		// window MODE
		ZeroMemory ( &m_PresentParams, sizeof(m_PresentParams) ) ;
		
		m_PresentParams.BackBufferWidth  = lWidth ;
		m_PresentParams.BackBufferHeight = lHeight ;
		
		m_PresentParams.BackBufferFormat = D3DFMT_UNKNOWN ;
		m_PresentParams.BackBufferCount = dwBufferCount ;
		
		m_PresentParams.Windowed = TRUE ;
		m_PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD ;
		
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;
		//*/


/*
		// fullscreen MODE
		ZeroMemory ( &m_PresentParams, sizeof(m_PresentParams) ) ;

		UINT count = m_pD3D->GetAdapterModeCount ( 0, D3DFMT_X8R8G8B8 ) ;
		D3DDISPLAYMODE Mode ;
		for ( int i = 0 ; i < count ; i++ )
		{
			m_pD3D->EnumAdapterModes ( 0, D3DFMT_X8R8G8B8, i, &Mode ) ;
		}
		
		m_PresentParams.BackBufferWidth  = 1024 ;
		m_PresentParams.BackBufferHeight = 768 ;
		
		m_PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_PresentParams.BackBufferCount = dwBufferCount ;
		
		m_PresentParams.Windowed = FALSE ;
		m_PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD ;
		
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;

		m_PresentParams.FullScreen_RefreshRateInHz = 60 ;
		m_PresentParams.hDeviceWindow = hWnd ;
		//*/

		D3DCAPS9 caps ;
		DWORD BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING ;
		HRESULT hr = m_pD3D->GetDeviceCaps ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ;
		if ( SUCCEEDED(hr) )
		{
			if ( !(caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) )
			{
				g_pDebug->Log_Error ( _T("VIDEO CARD NOT SUPPORT D3DCAPS2_DYNAMICTEXTURES!") ) ;
				return false ;
			}

			if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			{
				BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING ;
			}
		}

		UINT Adapter = D3DADAPTER_DEFAULT ;
		D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL ;

		hr = m_pD3D->CreateDevice ( Adapter, DeviceType, hWnd, BehaviorFlags, &m_PresentParams, &m_pDevice ) ;
		if ( FAILED(hr ) )
		{
			g_pDebug->Log_Error ( _T("m_pD3D->CreateDevice failed!\nhr=%x"), hr ) ;
			break ;
		}

		return true ;

	} while ( false ) ;

	DestroyDevice () ;
	return false ;
}

bool CD3DManager::CreateBackBuffer( HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount )
{
	if ( m_pDevice != NULL )
	{
		_ASSERTE(m_pDevice == NULL) ;
		g_pDebug->Log_Error ( _T("CreateBackBuffer: D3D Device was already created!") ) ;

		return false ;
	}

	if ( hWnd == NULL || lWidth <= 0 || lHeight <= 0 || ( lWidth % 4 ) != 0 || ( lHeight % 4 ) != 0 )
	{
		_ASSERTE(hWnd != NULL) ;
		_ASSERTE(lWidth > 0) ;
		_ASSERTE(lHeight > 0) ;
		_ASSERTE(( lWidth % 4 ) == 0) ;
		_ASSERTE(( lHeight % 4 ) == 0) ;
		g_pDebug->Log_Error ( _T("CD3DManager::Create() Invalid args!") ) ;

		return false ;
	}

	if ( !CreateDevice ( hWnd, lWidth, lHeight, dwBufferCount ) )
	{
		Destroy () ;
		return false ;
	}

	return true ;
}

bool CD3DManager::CreateImageBuffer( DWORD *dwIndex, long lWidth, long lHeight, COLOR_SPACE cs, RECT *rect )
{
	CDisplay *pDisplay = NULL ;
	*dwIndex = -1 ;

	do
	{
		if ( m_pDevice == NULL )
		{
			_ASSERTE(m_pDevice != NULL) ;
			g_pDebug->Log_Error ( _T("CD3DManager::SetImageBuffer() D3D Device wasn't created!") ) ;
			break ;
		}

		if ( rect == NULL || lWidth <= 0 || lHeight <= 0 || cs < CS_YV12 || cs > CS_RGB32 )
		{
			_ASSERTE(rect != NULL ) ;
			_ASSERTE(lWidth > 0) ;
			_ASSERTE(lHeight > 0) ;
			_ASSERTE(cs >= CS_YV12) ;
			_ASSERTE(cs <= CS_RGB32) ;
			
			g_pDebug->Log_Error ( _T("CD3DManager::SetImageBuffer() invalid args!") ) ;
			break ;
		}

		pDisplay = CD3DManager::CreateRender ( cs, m_pDevice, m_pPixelShader ) ;

		bool bret = pDisplay->SetImageBuffer ( lWidth, lHeight, cs, rect ) ;
		if ( !bret )
		{
			break ;
		}
		
		m_DisplayList.push_back ( pDisplay ) ;
		*dwIndex = reinterpret_cast<DWORD>(pDisplay) ;

		return true ;

	} while ( false ) ;
	
	SAFE_DELETE ( pDisplay ) ;
	return false ;
}

bool CD3DManager::CreateOverlayText ( DWORD *dwIndex, const char *strText, LOGFONT *logfont )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SetOverlayText() D3D Device wasn't created!") ) ;
		return false ;
	}

	if ( strText == NULL || logfont == NULL )
	{
		_ASSERTE(strText != NULL) ;
		_ASSERTE(logfont != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SetOverlayText() invalid args!") ) ;
		return false ;
	}

	CMYD3DFont *pMyFont = NULL ;
	*dwIndex = -1 ;
	do
	{
		CMYD3DFont *pMyFont = new CMYD3DFont () ;
		if ( pMyFont == NULL )
		{
			break ;
		}
		pMyFont->InitDeviceObjects ( m_pDevice ) ;

		HRESULT hr = pMyFont->SetOverlayText ( strText, *logfont ) ;
		if ( FAILED(hr) )
		{
			break ;
		}

		m_MyFontList.push_back ( pMyFont ) ;
		*dwIndex = reinterpret_cast<DWORD>(pMyFont) ;

		return true ;

	} while ( false ) ;

	SAFE_DELETE ( pMyFont ) ;
	return false ;
}

bool CD3DManager::GetOverlayTextWH ( DWORD dwIndex, long *lWidth, long *lHeight )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::GetOverlayTextWH() D3D Device wasn't created!") ) ;
		return false ;
	}

	CMYD3DFont *pMyFont = reinterpret_cast<CMYD3DFont*>(dwIndex) ;
	
	std::list<CMYD3DFont*>::iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		if ( *it == pMyFont )
		{
			return pMyFont->GetOverlayTextWH ( *lWidth, *lHeight ) ;
		}
	}
	return false ;
}

bool CD3DManager::CreateDynamicFont ( const char* strFontName, DWORD dwHeight )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SetDynamicText() D3D Device wasn't created!") ) ;
		return false ;
	}

	if ( strFontName == NULL || dwHeight == 0 )
	{
		_ASSERTE(strFontName != NULL) ;
		_ASSERTE(dwHeight != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SetDynamicText() invalid args!") ) ;
		return false ;
	}

	SAFE_DELETE ( m_pFont ) ;
	m_pFont = new CD3DFont ( strFontName, dwHeight ) ;
	if ( m_pFont == NULL )
	{
		return false ;
	}
	HRESULT hr = m_pFont->InitDeviceObjects ( m_pDevice ) ;
	if ( FAILED(hr) )
	{
		return false ;
	}
	hr = m_pFont->RestoreDeviceObjects () ;
	if ( FAILED(hr) )
	{
		return false ;
	}
	return true ;
}

bool CD3DManager::DestroyDynamicFont ( void )
{
	SAFE_DELETE ( m_pFont ) ;
	return true ;
}

bool CD3DManager::SetDefault (void)
{
	HRESULT hr = m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE ) ;
	hr = m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW ) ;
	return true ;
}

bool CD3DManager::ColorFill ( DWORD dwIndex, BYTE r, BYTE g, BYTE b )
{
	if ( m_pDevice == NULL )
	{
		g_pDebug->Log_Error ( _T("CD3DManager::Fill() D3D Device wasn't created!") ) ;
		return false ;
	}

	CDisplay *p = reinterpret_cast<CDisplay*>(dwIndex) ;

	std::list<CDisplay*>::iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		if ( *it == p )
		{
			return (*it)->Fill ( r, g, b ) ;
		}
	}
	return false ;
}

void CD3DManager::InvalidateDeviceObjects(void)
{
	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		(*it)->InvalidateDeviceObjects () ;
	}

	std::list<CMYD3DFont*>::const_iterator it1 ;
	for ( it1 = m_MyFontList.begin() ; it1 != m_MyFontList.end() ; it1++ )
	{
		(*it1)->InvalidateDeviceObjects () ;
	}

	if ( m_pFont != NULL )
	{
		m_pFont->InvalidateDeviceObjects () ;
	}
}

bool CD3DManager::RestoreDeviceObjects(void)
{
	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		(*it)->RestoreDeviceObjects () ;
	}

	std::list<CMYD3DFont*>::const_iterator it1 ;
	for ( it1 = m_MyFontList.begin() ; it1 != m_MyFontList.end() ; it1++ )
	{
		(*it1)->RestoreDeviceObjects () ;
	}

	if ( m_pFont != NULL )
	{
		m_pFont->RestoreDeviceObjects () ;
	}

	SetDefault () ;
	return true ;
}

bool CD3DManager::HandleDeviceLost(void)
{
	if ( !m_bDeviceLost )
	{
		return true ;
	}
	
	HRESULT hr = m_pDevice->TestCooperativeLevel () ;
	
	if ( FAILED(hr) )
	{
		if ( hr == D3DERR_DEVICELOST )
		{
			return false ;
		}
		else if ( hr == D3DERR_DEVICENOTRESET )
		{
			InvalidateDeviceObjects () ;
		
			hr = m_pDevice->Reset ( &m_PresentParams ) ;
			if ( FAILED( hr ) )
			{	
				return false ;
			}
			
			if ( RestoreDeviceObjects () )
			{
				m_bDeviceLost = false ;
			}
		}
		else
		{
			g_pDebug->Log_Error ( _T("VideoCard UNKNOWN DEVICE ERROR!\nhr=%x"), hr ) ;
			return false ;
		}
	}
	
	return true ;
}

bool CD3DManager::UpdateImage ( DWORD dwImageBufferIndex, BYTE *pBuf )
{
	return (reinterpret_cast<CDisplay*>(dwImageBufferIndex))->UpdateImage ( pBuf ) ;
}


bool CD3DManager::BeginRender ( bool bClearBackBuffer, DWORD dwBKColor )
{
	HRESULT hr ;
	if ( bClearBackBuffer )
	{
		hr = m_pDevice->Clear ( 0, NULL, D3DCLEAR_TARGET, dwBKColor, 1.0f, 0 ) ;
	}
	hr = m_pDevice->BeginScene () ;
	if ( SUCCEEDED(hr) )
	{
		return true ;
	}
	else
	{
		g_pDebug->Log_Error ( _T("m_pDevice->BeginScene() failed!\nhr=%x"), hr ) ;
		return false ;
	}
}

bool CD3DManager::EndRender (void)
{
	HRESULT hr = m_pDevice->EndScene () ;
	hr = m_pDevice->Present ( NULL, NULL, NULL, NULL ) ;
	if ( hr != D3D_OK )
	{
		DWORD dw = GetTickCount () ;
		if ( dw - m_dwPrevTime >= 10000 ) // 10s
		{
			m_dwPrevTime = dw ;
			g_pDebug->Log_Error ( _T("m_pDevice->Present FAILED!\nhr=%x"), hr ) ;
		}		

		if ( hr == D3DERR_DEVICELOST )
		{
			m_bDeviceLost = true ;
		}
		return false ;
	}
	return true ;
}

bool CD3DManager::DrawImage ( DWORD dwIndex, GEOMETRIC_TRANSFORMATION Transformation, RECT *rect )
{
	return (reinterpret_cast<CDisplay*>(dwIndex))->DrawImage ( Transformation, rect ) ;
}

bool CD3DManager::DrawOverlayText ( DWORD dwIndex, DWORD dwColor, RECT *rect )
{
	(reinterpret_cast<CMYD3DFont*>(dwIndex))->DrawText ( dwColor, rect ) ;
	return true ;
}

bool CD3DManager::DrawDynamicText ( long x, long y, DWORD dwColor, const TCHAR* strText )
{
	m_pFont->DrawText ( x, y, dwColor, strText ) ;
	return true ;
}


void CD3DManager::Destroy(void)
{
	DestroyFont () ;
	DestroyDisplay () ;
	DestroyShader () ;

	DestroyDevice () ;
}

void CD3DManager::DestroyFont(void)
{
	SAFE_DELETE ( m_pFont ) ;

	std::list<CMYD3DFont*>::const_iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		delete *it ;
	}
	m_MyFontList.clear () ;
}

void CD3DManager::DestroyDisplay(void)
{
	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		delete *it ;
	}
	m_DisplayList.clear () ;
}

void CD3DManager::DestroyShader(void)
{
	for ( int i = 0 ; i < SHADER_COUNT ; i++ )
	{
		SAFE_DELETE ( m_pPixelShader[i] ) ;
	}
}

void CD3DManager::DestroyDevice(void)
{
	SAFE_RELEASE ( m_pDevice ) ;
	SAFE_RELEASE ( m_pD3D ) ;
}

bool CD3DManager::DestroyImageBuffer ( DWORD dwIndex )
{
	CDisplay *p = reinterpret_cast<CDisplay*>(dwIndex) ;

	std::list<CDisplay*>::iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		if ( *it == p )
		{
			delete *it ;
			m_DisplayList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}

bool CD3DManager::DestroyOverlayText ( DWORD dwIndex )
{
	CMYD3DFont *p = reinterpret_cast<CMYD3DFont*>(dwIndex) ;

	std::list<CMYD3DFont*>::iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		if ( *it == p )
		{
			delete *it ;
			m_MyFontList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}