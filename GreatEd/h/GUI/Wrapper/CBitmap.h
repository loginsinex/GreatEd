/*
	A NES Super Mario Bros Level Editor [public version]
	Copyright (C) 2025 ALXR aka loginsin
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define XCHGCOLORREF(_x)              ((((_x)&0xFF00FF00L)|(((_x)&0x00FF0000L)>>16)|(((_x)&0x000000FFL)<<16)))

class CBitmap
{
	struct
	{
		HDC						hDCDesktop;
		HDC						hDC;
		RGBQUAD *				pvBuffer;
		HBITMAP					hBitmap;
		HBITMAP					hDefaultBitmap;
		SIZE					sz;
		UINT					uSize;
		BOOL					fLocked;
	} m_buffer{};

	BOOL					CreateFromResource(HINSTANCE hInstance, LPCTSTR pszResource);

public:
	CBitmap();
	CBitmap(const CBitmap & bm);
	CBitmap(CBitmap && bm);
	CBitmap(INT cx, INT cy);
	CBitmap(HINSTANCE hInstance, LPCTSTR pszResource);
	~CBitmap();

	VOID						Create(INT cx, INT cy);
	VOID						Destroy();
	RGBQUAD *					Bits();
	RGBQUAD *					Bits() const;
	CBitmap &					operator=(const CBitmap & bm);
	operator HDC();
	VOID						FreeDC();
	HDC							LockDC();
	HBITMAP						Bitmap() const;
	INT							Width() const;
	INT							Height() const;

	VOID						CreateFromBitmap(HBITMAP hBmp);
	VOID						FillRect(int x, int y, int cx, int cy, COLORREF clr, BYTE bAlpha = 0);
	VOID						FillRect(INT x, INT y, INT cx, INT cy, COLORREF crColor, COLORREF crBorder, INT iWidth, BYTE fInnerAlpha = 255);
	VOID						RenderBitmap(CBitmap & bmSrc, int x, int y, int cx, int cy, int dest_x, int dest_y, BYTE bAlpha = 255, BOOL fUseSourceAlpha = FALSE);
	VOID						RenderBitmap(HDC hDC, int x, int y, int cx, int cy, int dest_x, int dest_y, BYTE bAlpha = 255, BOOL fUseSourceAlpha = FALSE);
	VOID						RenderText(INT x, INT y, INT cx, INT cy, LPCTSTR pszText, HFONT hFont, BOOL bTransparent, COLORREF crTextColor, COLORREF crBackColor, DWORD dwFlags);
	VOID						CopyRect(CBitmap & bmSrc, UINT xSrc, UINT ySrc, UINT cxSrc, UINT cySrc, UINT xDst, UINT yDst);
	VOID						GrayScale();
#ifdef UNICODE
	VOID						RenderTextA(INT x, INT y, INT cx, INT cy, LPCSTR pszText, HFONT hFont, BOOL bTransparent, COLORREF crTextColor, COLORREF crBackColor, DWORD dwFlags);
#else
#define RenderTextA				RenderText
#endif

};