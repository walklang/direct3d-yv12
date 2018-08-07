/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once

// NOTE
//
// 1. 所有函数调用只支持单线程，不支持多线程。
// 2. 根据D3D9.0c SDK文档说明，本模块所有函数最好在视频显示窗口的消息处理线程中调用。
// 3. 同一个进程可以创建多个渲染模块实例，一个渲染模块实例可渲染多个视频图像。
// 4. 使用默认显卡，不支持多显卡。
// 5. 像素SHADER版本不能低于2.0
// 6. 不支持全屏模式。
// 7. VC++ 2005 ＋ D3D9.0c SDK 编译。NVIDIA GeForce 9800 GT显卡上测试通过。


class ID3DVRInterface
{
public:
	enum COLOR_SPACE
	{
		CS_YV12		= 0, // YVU420
		CS_I420		= 1, // YUV420
		CS_YUY2		= 2, // YUYV
		CS_UYVY		= 3, // UYVY 
		CS_NV12		= 4, // NV12
		CS_RGB24	= 5, // rgb rgb ...
		CS_BGR24	= 6, // bgr bgr ...
		CS_RGB16	= 7, // R5G6B5
		CS_RGB15	= 8, // X1R5G5B5
		CS_RGB32	= 9, // bgra bgra ...
		CS_UNKNOWN  = 0xffffffff
	} ;

	enum GEOMETRIC_TRANSFORMATION
	{
		Upper_Down_Flip = 1,    // 图像上下颠倒
		Not_Upper_Down_Flip = 2
	} ;

public:

	// CreateBackBuffer创建后备帧缓冲。
	// 输入参数：
	// hWnd:			视频渲染窗口句柄。
	// lWidth:			后备缓冲区宽度，单位：像素。
	// lHeight:			后备缓冲区高度，单位：像素。
	// dwBufferCount:   后备缓冲区个数。
	
	// 输出参数：无。
	virtual bool CreateBackBuffer ( HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount = 2 ) = 0 ;

	// CreateImageBuffer创建视频图像缓冲区。
	// 输入参数：
	// lWidth:  视频图像缓冲区宽度，单位：像素。
	// lHeight: 视频图像缓冲区高度，单位：像素。
	// rect:	视频图像显示区域。
	// cs:      视频图像颜色空间。

	// 输出参数： 
	// dwIndex: 视频图像缓冲区索引。
	virtual bool CreateImageBuffer ( DWORD *dwIndex, long lWidth, long lHeight, COLOR_SPACE cs, RECT *rect ) = 0 ;

	// DestroyImageBuffer销毁视频图像缓冲区。
	// 输入参数：
	// dwIndex: 视频图像缓冲区索引。
	virtual bool DestroyImageBuffer ( DWORD dwIndex ) = 0 ;

	// CreateOverlayText创建叠加文本, 支持中文。
	// 输入参数：
	// strText: 叠加文本内容。
	// logfont: 叠加文本字体。

	// 输出参数：
	// dwIndex: 叠加文本索引。
	virtual bool CreateOverlayText ( DWORD *dwIndex, const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText销毁叠加文本。
	// 输入参数：
	// dwIndex: 叠加文本索引。
	virtual bool DestroyOverlayText ( DWORD dwIndex ) = 0 ;

	// 输入参数：
	// dwIndex: 叠加文本索引。

	// 输出参数：
	// lWidth: 叠加文本的实际宽度，单位：像素。
	// lHeight: 叠加文本的实际高度，单位：像素。
	virtual bool GetOverlayTextWH ( DWORD dwIndex, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont创建内容动态变化的数字、英文字符文本，主要用来显示变化的数字，不支持中文。
	// 输入参数：
	// strFongName: 字体名称。
	// dwHeight: 字体高度。
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyOverlayText销毁动态文本。
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill填充视频缓冲区。
	// 输入参数：
	// dwIndex: 视频缓冲区索引。
	// r g b  : RGB COLOR。
	virtual bool ColorFill ( DWORD dwIndex, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost处理D3D设备丢失
	virtual bool HandleDeviceLost( void ) = 0 ;

	// UpdateImage更新视频缓冲区内容。
	// 输入参数：
	// lIndex: 视频缓冲区索引。
	// pData: 更新数据缓冲区指针。
	virtual bool UpdateImage ( DWORD dwIndex, BYTE *pData ) = 0 ;

	// BeginRender开始D3D渲染。
	// 输入参数：
	// bClearBackBuffer: 是否清空后备缓冲区(不清空后备缓冲区可以提高性能)。
	// dwBKColor: 清空后备缓冲区为指定颜色。[0Xargb]
	virtual bool BeginRender ( bool bClearBackBuffer, DWORD dwBKColor ) = 0 ;

	// DrawImage渲染视频图像。
	// 输入参数：
	// dwIndex: 视频缓冲区索引。
	// bFlip:  是否上下反转视频图像。
	// rect:   视频显示区域。
	virtual bool DrawImage ( DWORD dwIndex, GEOMETRIC_TRANSFORMATION Transformation = Not_Upper_Down_Flip, RECT *rect = NULL ) = 0 ;

	// DrawText渲染叠加文本。
	// 输入参数：
	// dwIndex: 文本索引。
	// dwColor: 文本颜色。[0Xargb]
	// rect: 文本显示区域。
	virtual bool DrawOverlayText ( DWORD dwIndext, DWORD dwColor, RECT *rect ) = 0 ;

	// DrawDynamicText渲染动态文本。
	// 输入参数：
	// x,y: 动态文本渲染坐标(左上角)。
	// dwColor: 动态文本颜色。[0Xargb]
	// strText: 动态文本。
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender结束D3D渲染。
	virtual bool EndRender ( void ) = 0 ;
} ;


#if defined(__cplusplus)
extern "C" 
{
#endif

namespace D3D_VIDEO_RENDER
{
	/*****************************************************************************/
	//	功能：	创建视频渲染模块。
	//
	//	函数原型：
	//
	/**/	ID3DVRInterface* D3D_Video_Render_Create () ;
	//
	//	参数：
	//
	//		输入：void。
	//
	//		输出：无。
	//              
	//	返回值：指向视频渲染模块接口的指针,若为NULL表示失败。
	//
	//	备注：一个进程可以创建多个视频渲染模块实例。
	/*****************************************************************************/

	/*****************************************************************************/
	//	功能：	销毁所有视频渲染模块。
	//
	//	函数原型：
	//
	/**/	void D3D_Video_Render_Destroy () ;
	//
	//	参数：
	//
	//		输入：无。
	//
	//		输出：无。
	//              
	//	返回值：无。
	//
	//	备注：
	/*****************************************************************************/
}

#if defined(__cplusplus)
}
#endif