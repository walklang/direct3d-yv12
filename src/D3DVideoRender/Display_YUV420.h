/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "display.h"
#include "Shader_YUV420.h"
#include "CommonFunctions.h"

class CDisplay_YUV420 :
	public CDisplay
{
	struct YUV_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;

		D3DXVECTOR2 tex1 ;
		D3DXVECTOR2 tex2 ;
		D3DXVECTOR2 tex3 ;
	} ;

	enum
	{
		D3DFVF_YUV_VERTEX_macro = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3
	} ;

public:

	CDisplay_YUV420(HRESULT &hr, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
		: m_pVertices(NULL)
		, m_pShader(NULL)
	{
		m_pDevice = pDevice ;

		Init_Array<LPDIRECT3DTEXTURE9>( m_pTexture, CShader_YUV420::COUNT_YUV_TEX, NULL ) ;
		Init_Array<LPDIRECT3DTEXTURE9>( m_pTexTemp, CShader_YUV420::COUNT_YUV_TEX, NULL ) ;
		
		Init_Array<long>( m_lWidth, CShader_YUV420::COUNT_YUV_TEX, 0 ) ;
		Init_Array<long>( m_lHeight, CShader_YUV420::COUNT_YUV_TEX, 0 ) ;
		Init_Array<long>( m_lSize, CShader_YUV420::COUNT_YUV_TEX, 0 ) ;
				
		if ( pShader[CD3DManager::SHADER_YUV420] == NULL )
		{
			pShader[CD3DManager::SHADER_YUV420] = new CShader_YUV420 ( hr, pDevice ) ;
		}
		
		if ( pShader[CD3DManager::SHADER_YUV420] == NULL || FAILED(hr) )
		{
			SAFE_DELETE(pShader[CD3DManager::SHADER_YUV420]) ;
			return ;
		}

		m_pShader = (CShader_YUV420*)pShader[CD3DManager::SHADER_YUV420] ;
	}
public:

	~CDisplay_YUV420(void)
	{
		InvalidateDeviceObjects () ;
	}

	static CDisplay *Create (LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
	{
		HRESULT hr = S_OK ;
		CDisplay_YUV420 *p = new CDisplay_YUV420 ( hr, pDevice, pShader ) ;
		if ( FAILED(hr) )
		{
			SAFE_DELETE ( p ) ;
		}
		return p ;
	}

public:
	virtual bool SetImageBuffer ( long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect )
	{
		m_ColorSpace = cs ;
		return CreateVT ( lWidth, lHeight, rect ) ;
	}

	virtual bool UpdateImage ( BYTE *pData )
	{
		BYTE *pSrc = pData ;
		for ( int i = CShader_YUV420::Y_TEX ; i < CShader_YUV420::COUNT_YUV_TEX ; i++ )
		{
			D3DLOCKED_RECT lrect ;
			HRESULT hr = m_pTexture[i]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
			if ( SUCCEEDED(hr) )
			{
				BYTE *pDest = (BYTE*)lrect.pBits ;
				if ( lrect.Pitch == m_lWidth[i] )
				{
					memcpyMMX ( pDest, pSrc, m_lSize[i] ) ;
					pSrc += m_lSize[i] ;
				}
				else
				{
					for ( int j = 0 ; j < m_lHeight[i] ; j++ )
					{
						memcpyMMX ( pDest, pSrc, m_lWidth[i] ) ;
						pDest += lrect.Pitch ;
						pSrc += m_lWidth[i] ;
					}
				}

				hr = m_pTexture[i]->UnlockRect ( 0 ) ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pTexture[i]->UnlockRect FAILED!\nhr=%x"), hr ) ;
				}
			}
			else
			{
				__asm emms ;
				g_pDebug->Log_Error ( _T("m_pTexture[i]->LockRect FAILED!\nhr=%x"), hr ) ;
				return false ;
			}
		}

		__asm emms ;
		return true ;
	}

	virtual bool DrawImage ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *rect )
	{
		HRESULT hr = m_pDevice->SetFVF ( D3DFVF_YUV_VERTEX_macro ) ;
		hr = m_pDevice->SetPixelShader ( m_pShader->m_pPixelShader ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader failed!\nhr=%x"), hr ) ;
		} 

		SetImageRect ( Transformation, rect ) ;

		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::Y_TEX].RegisterIndex, m_pTexTemp[CShader_YUV420::Y_TEX] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::U_TEX].RegisterIndex, m_pTexTemp[CShader_YUV420::U_TEX] )  ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [U_TEX] failed!\nhr=%x"), hr ) ;
		}
		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::V_TEX].RegisterIndex, m_pTexTemp[CShader_YUV420::V_TEX] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [V_TEX] failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->SetStreamSource ( 0, m_pVertices, 0, sizeof(YUV_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}

		return true ;
	}

	virtual bool InvalidateDeviceObjects ()
	{
		SAFE_RELEASE ( m_pVertices ) ;
		for ( int i = 0 ; i < CShader_YUV420::COUNT_YUV_TEX ; i++ )
		{
			SAFE_RELEASE ( m_pTexture[i] ) ;
		}

		return true ;
	}

	virtual bool RestoreDeviceObjects ()
	{
		return CreateVT ( m_lWidth[CShader_YUV420::Y_TEX], m_lHeight[CShader_YUV420::Y_TEX], &m_DisplayRect ) ;
	}

	virtual bool Fill ( BYTE r, BYTE g, BYTE b )
	{
		if ( m_pTexture[CShader_YUV420::Y_TEX] == NULL )
		{
			return false ;
		}
		
		BYTE yuv[3] ;
		rgb2yuv ( r, g, b, &yuv[0], &yuv[1], &yuv[2] ) ;

		for ( int i = CShader_YUV420::Y_TEX ; i < CShader_YUV420::COUNT_YUV_TEX ; i++ )
		{
			D3DLOCKED_RECT lrect ;
			HRESULT hr = m_pTexTemp[i]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
			if ( SUCCEEDED(hr) )
			{
				BYTE *pDest = (BYTE*)lrect.pBits ;
				if ( lrect.Pitch == m_lWidth[i] )
				{
					memset ( pDest, yuv[i], m_lSize[i] ) ;
				}
				else
				{
					for ( int j = 0 ; j < m_lHeight[i] ; j++ )
					{
						memset ( pDest, yuv[i], m_lWidth[i] ) ;
						pDest += lrect.Pitch ;
					}
				}

				hr = m_pTexTemp[i]->UnlockRect ( 0 ) ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pTexture[i]->UnlockRect FAILED!\nhr=%x"), hr ) ;
				}
			}
			else
			{
				g_pDebug->Log_Error ( _T("m_pTexture[i]->LockRect FAILED!\nhr=%x"), hr ) ;
				return false ;
			}
		}

		return true ;
	}

private:
	bool CreateVT ( long lWidth, long lHeight, RECT *rect )
	{
		HRESULT hr ;

		do
		{
			long x = rect->left ;
			long y = rect->top ;
			long dx = rect->right ;
			long dy = rect->bottom ;
			m_DisplayRect.left = x ;
			m_DisplayRect.top = y ;
			m_DisplayRect.right = dx ;
			m_DisplayRect.bottom = dy ;

			hr = m_pDevice->CreateVertexBuffer ( 4 * sizeof(YUV_VERTEX_st), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_YUV_VERTEX_macro, D3DPOOL_DEFAULT, &m_pVertices, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateVertexBuffer failed!\nhr=%x"), hr ) ;
				break ;
			}

			YUV_VERTEX_st *pVB ;

			hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
				break ;
			}

			pVB[0].pos = D3DXVECTOR4 ( x-0.5f,         y-0.5f,			0.0f, 1.0f ) ;
			pVB[1].pos = D3DXVECTOR4 ( dx - 0.5f,     y-0.5f,			0.0f, 1.0f ) ;
			pVB[2].pos = D3DXVECTOR4 ( dx - 0.5f,     dy - 0.5f,   0.0f, 1.0f ) ;
			pVB[3].pos = D3DXVECTOR4 ( x-0.5f,         dy - 0.5f,   0.0f, 1.0f ) ;

			DWORD dw = D3DCOLOR_RGBA(255,255,255,255) ;
			pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = dw ;

			if ( m_Flip == ID3DVRInterface::Not_Upper_Down_Flip )
			{
				pVB[0].tex1 = pVB[0].tex2 = pVB[0].tex3 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
				pVB[1].tex1 = pVB[1].tex2 = pVB[1].tex3 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
				pVB[2].tex1 = pVB[2].tex2 = pVB[2].tex3 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
				pVB[3].tex1 = pVB[3].tex2 = pVB[3].tex3 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
			}
			else
			{
				pVB[0].tex1 = pVB[0].tex2 = pVB[0].tex3 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
				pVB[1].tex1 = pVB[1].tex2 = pVB[1].tex3 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
				pVB[2].tex1 = pVB[2].tex2 = pVB[2].tex3 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
				pVB[3].tex1 = pVB[3].tex2 = pVB[3].tex3 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
			}

			hr = m_pVertices->Unlock () ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
				break ;
			}

			for ( int i = CShader_YUV420::Y_TEX ; i < CShader_YUV420::COUNT_YUV_TEX ; i++ )
			{
				long Width = lWidth ;
				long Height = lHeight ;
				if ( i != 0 )
				{
					Width  /= 2 ;
					Height /= 2 ;
				}
				hr = m_pDevice->CreateTexture ( Width, Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pTexture[i], NULL ) ;

				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
					break ;
				}

				m_lWidth[i] = Width ;
				m_lHeight[i] = Height ;
				m_lSize[i] = Width * Height ;
			}

			m_pTexTemp[CShader_YUV420::Y_TEX] = m_pTexture[CShader_YUV420::Y_TEX] ;
			if ( m_ColorSpace == ID3DVRInterface::CS_YV12 )
			{
				m_pTexTemp[CShader_YUV420::U_TEX] = m_pTexture[CShader_YUV420::V_TEX] ;
				m_pTexTemp[CShader_YUV420::V_TEX] = m_pTexture[CShader_YUV420::U_TEX] ;
			}
			else if ( m_ColorSpace == ID3DVRInterface::CS_I420 )
			{
				m_pTexTemp[CShader_YUV420::U_TEX] = m_pTexture[CShader_YUV420::U_TEX] ;
				m_pTexTemp[CShader_YUV420::V_TEX] = m_pTexture[CShader_YUV420::V_TEX] ;
			}

			return true ;

		} while ( false ) ;

		InvalidateDeviceObjects () ;
		return false ;
	}

	bool SetImageRect ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Flip, RECT *rect )
	{
		if ( Flip == m_Flip && rect == NULL )
		{
			return true ;
		}

		RECT *pRt = &m_DisplayRect ;
		if ( rect != NULL )
		{
			if ( Flip == m_Flip && pRt->left == rect->left && pRt->right == rect->right
				&& pRt->top == rect->top && pRt->bottom == rect->bottom )
			{
				return true ;
			}
			else
			{
				pRt->left = rect->left ;
				pRt->top = rect->top ;
				pRt->right = rect->right ;
				pRt->bottom = rect->bottom ;
			}
		}
		
		long x = pRt->left ;
		long y = pRt->top ;
		long dx = pRt->right ;
		long dy = pRt->bottom ;
		

		YUV_VERTEX_st *pVB ;

		HRESULT hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, D3DLOCK_DISCARD ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
			return false ;
		}

		pVB[0].pos = D3DXVECTOR4 ( x-0.5f,         y-0.5f,			0.0f, 1.0f ) ;
		pVB[1].pos = D3DXVECTOR4 ( dx - 0.5f,     y-0.5f,			0.0f, 1.0f ) ;
		pVB[2].pos = D3DXVECTOR4 ( dx - 0.5f,     dy - 0.5f,   0.0f, 1.0f ) ;
		pVB[3].pos = D3DXVECTOR4 ( x-0.5f,         dy - 0.5f,   0.0f, 1.0f ) ;

		pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = 0xffffffff ;

		if ( Flip == ID3DVRInterface::Not_Upper_Down_Flip )
		{
			pVB[0].tex1 = pVB[0].tex2 = pVB[0].tex3 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
			pVB[1].tex1 = pVB[1].tex2 = pVB[1].tex3 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
			pVB[2].tex1 = pVB[2].tex2 = pVB[2].tex3 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
			pVB[3].tex1 = pVB[3].tex2 = pVB[3].tex3 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
		}
		else
		{
			pVB[0].tex1 = pVB[0].tex2 = pVB[0].tex3 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
			pVB[1].tex1 = pVB[1].tex2 = pVB[1].tex3 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
			pVB[2].tex1 = pVB[2].tex2 = pVB[2].tex3 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
			pVB[3].tex1 = pVB[3].tex2 = pVB[3].tex3 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
		}
		m_Flip = Flip ;

		hr = m_pVertices->Unlock () ;

		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
			return false ;
		}
		return true ;
	}

private:

	LPDIRECT3DVERTEXBUFFER9 m_pVertices ;
	LPDIRECT3DTEXTURE9	m_pTexture[CShader_YUV420::COUNT_YUV_TEX] ;

	LPDIRECT3DTEXTURE9	m_pTexTemp[CShader_YUV420::COUNT_YUV_TEX] ;

	long m_lWidth[CShader_YUV420::COUNT_YUV_TEX] ;
	long m_lHeight[CShader_YUV420::COUNT_YUV_TEX] ;
	long m_lSize[CShader_YUV420::COUNT_YUV_TEX] ;
	CShader_YUV420 *m_pShader ;	
};
