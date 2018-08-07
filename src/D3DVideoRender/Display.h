/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once

#include <crtdbg.h>
#include <d3dx9.h>
#include "Shader_Pixel.h"

class CDisplay
{	
public:
	virtual bool SetImageBuffer ( long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect ) = 0 ;
	virtual bool UpdateImage ( BYTE *pData ) = 0 ;
	virtual bool DrawImage ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *rect ) = 0 ;
	virtual bool InvalidateDeviceObjects () = 0 ;
	virtual bool RestoreDeviceObjects () = 0 ;
	virtual bool Fill ( BYTE r, BYTE g, BYTE b ) = 0 ;

public:

	CDisplay(void)
		: m_pDevice(NULL)
		, m_ColorSpace(ID3DVRInterface::CS_UNKNOWN)
		, m_Flip(ID3DVRInterface::Not_Upper_Down_Flip)
	{
		memset ( &m_DisplayRect, 0, sizeof(m_DisplayRect) ) ;
	}

	virtual ~CDisplay(void)
	{
	}

protected:
	void rgb2yuv ( BYTE r, BYTE g, BYTE b, BYTE *y, BYTE *u, BYTE *v )
	{
		*y = ( (  66 * r + 129 * g +  25 * b + 128) >> 8) +  16 ; // Y
		*v = ( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ; // V
		*u = ( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ; // U
	}

protected:
	LPDIRECT3DDEVICE9 m_pDevice ;
	RECT m_DisplayRect ;
	ID3DVRInterface::GEOMETRIC_TRANSFORMATION m_Flip ;
	ID3DVRInterface::COLOR_SPACE m_ColorSpace ;
};
