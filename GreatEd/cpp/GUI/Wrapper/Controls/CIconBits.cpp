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

#include "Controls.h"

CIconBits::CIconBits()
	: m_pvBits( NULL )
{
	m_sz.cx = m_sz.cy = 0;
}

CIconBits::CIconBits(HICON hIcon)
	: m_pvBits( NULL )
{
	m_sz.cx = m_sz.cy = 0;

	ImportIcon( hIcon );
}

CIconBits::CIconBits(const CIconBits & bits)
	: m_pvBits( NULL )
{
	m_sz.cx = m_sz.cy = 0;
	operator=(bits);
}

CIconBits& CIconBits::operator=(const CIconBits & bits)
{
	if ( this == &bits )
		return *this;

	m_pvBits = bits.m_pvBits;
	m_sz = bits.m_sz;
	return *this;
}

CIconBits::~CIconBits()
{
	m_pvBits.clear();

	m_sz.cx = m_sz.cy = 0;
}

BOOL CIconBits::Create(SIZE sz)
{	
	if ( m_pvBits.size() > 0 )
	{
		m_pvBits.clear();
		m_sz.cx = m_sz.cy = 0;
	}

	if ( sz.cx > 0 && sz.cy > 0 )
	{
		RGBQUAD nullRGB = { 0 };
		m_pvBits.insert( m_pvBits.begin(), sz.cx * sz.cy, nullRGB );
		m_sz = sz;
		return TRUE;
	}

	return FALSE;
}

BOOL CIconBits::ImportIcon(HICON hIcon)
{
	BOOL fResult = FALSE;
	ICONINFO ii = { 0 };
	if ( GetIconInfo( hIcon, &ii ) )
	{
		fResult = ImportIcon( ii.hbmColor );
		if ( fResult )
		{
			std::vector<BYTE> pBits;
			if ( GetMaskBits( ii.hbmMask, pBits ) )
			{
				LONG c = m_sz.cx * m_sz.cy;
				for(LONG i = 0; i < c; ++i)
				{
					if ( pBits[i] )
						m_pvBits[i].rgbReserved = 0;
					else
						m_pvBits[i].rgbReserved = ( m_pvBits[i].rgbReserved ? m_pvBits[i].rgbReserved : 255 );
				}
			}
		}

		if ( ii.hbmMask )
			DeleteObject( ii.hbmMask );

		if ( ii.hbmColor )
			DeleteObject( ii.hbmColor );
	}

	return fResult;	
}

BOOL CIconBits::ImportIcon(HBITMAP hBitmap)
{
	if ( m_pvBits.size() > 0 )
	{
		m_pvBits.clear();
		m_sz.cx = m_sz.cy = 0;
	}

	BOOL fResult = FALSE;
	BITMAP bmp = { 0 };
	if ( GetObject( hBitmap, sizeof(bmp), &bmp ) )
	{
		HDC hSourceDC = GetDC(NULL);
		HDC hDC = CreateCompatibleDC( hSourceDC );
		HGDIOBJ hbmOld = SelectObject( hDC, hBitmap );
		ULONG uSize = bmp.bmWidth * bmp.bmWidth * sizeof(RGBQUAD);
		
		struct
		{
			BITMAPINFOHEADER bmiHeader;
			COLORREF pal[256];
		} bmi = { 0 };

		// BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(bmi);
		bmi.bmiHeader.biWidth = bmp.bmWidth;
		bmi.bmiHeader.biHeight = -bmp.bmHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = uSize;

		RGBQUAD nullRGB = { 0 };
		m_pvBits.insert( m_pvBits.begin(), bmp.bmWidth * bmp.bmHeight, nullRGB );
		
		fResult = ( 0 !=
			GetDIBits( hDC, hBitmap, 0, bmp.bmHeight, m_pvBits.data(), PBITMAPINFO(&bmi), DIB_RGB_COLORS ) );

		SelectObject( hDC, hbmOld );
		DeleteDC( hDC );
		ReleaseDC( NULL, hSourceDC );

		if ( !fResult )
		{
			m_pvBits.clear();
		}
		else
		{
			m_sz.cx = bmp.bmWidth;
			m_sz.cy = bmp.bmHeight;
		}
	}
	else
		_tprintf(TEXT("GetObject failed with bmp %p: %d\n"), hBitmap, GetLastError());

	return fResult;
}

HICON CIconBits::ExportIcon()
{
	if ( !m_pvBits.size() )
		return NULL;

	HICON hIcon = NULL;

	HDC hSourceDC = GetDC(NULL);
	HDC hDC = CreateCompatibleDC( hSourceDC );

	ULONG uSize = m_sz.cx * m_sz.cy * sizeof(RGBQUAD);
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(bmi);
	bmi.bmiHeader.biWidth = m_sz.cx;
	bmi.bmiHeader.biHeight = -m_sz.cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = uSize;

	RGBQUAD * pv = NULL;

	HBITMAP hBmp = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (PVOID*) &pv, NULL, 0 );
	if ( hBmp )
	{
		if ( !IsBadWritePtr( pv, uSize ) )
		{
			CopyMemory( pv, m_pvBits.data(), uSize );
			ICONINFO ii = { 0 };
			ii.fIcon = TRUE;
			ii.hbmColor = ii.hbmMask = hBmp;

			hIcon = CreateIconIndirect( &ii );
		}
		DeleteObject( hBmp );
	}

	DeleteDC( hDC );
	ReleaseDC( NULL, hSourceDC );

	return hIcon;
}

BOOL CIconBits::GetMaskBits(HBITMAP hBitmap, std::vector<BYTE> & pBits)
{
	if ( !hBitmap )
		return NULL;

	std::vector<BYTE> bmiBuff( 1024 * 1024, 0 );
	std::vector<BYTE> bmpBuff( 1024 * 1024, 0 );

	BITMAP * bmp = (BITMAP*) bmpBuff.data();
	// wine creates strange object for hBmMask, so GetObjects places garbage to BITMAP struct

	// BITMAP bmp = { 0 };
	if ( GetObject(hBitmap, sizeof(BITMAP), bmp) )	
	{
		HDC hDCSource = GetDC( NULL );
		HDC hDC = CreateCompatibleDC( hDCSource );
		HGDIOBJ hbmOld = SelectObject( hDC, hBitmap );

		pBits.insert(pBits.begin(), bmp->bmWidth * bmp->bmHeight, 0);

		BITMAPINFO * bmi = (BITMAPINFO*) bmiBuff.data();
	/*
		struct
		{
			BITMAPINFOHEADER bmiHeader;
			COLORREF bmiColors[1024];
		} bmi = { 0 };
	*/
		bmi->bmiHeader.biSize = sizeof(*bmi);
		bmi->bmiHeader.biWidth = bmp->bmWidth;
		bmi->bmiHeader.biHeight = -bmp->bmHeight;
		bmi->bmiHeader.biPlanes = 1;
		bmi->bmiHeader.biBitCount = 8;
		bmi->bmiHeader.biCompression = BI_RGB;
		bmi->bmiHeader.biSizeImage = bmp->bmWidth * bmp->bmHeight / 8;

		if ( ! GetDIBits( hDC, hBitmap, 0, bmp->bmHeight, pBits.data(), (PBITMAPINFO) bmi, DIB_RGB_COLORS ) )
		{
			pBits.clear();
		}

		SelectObject( hDC, hbmOld );
		DeleteDC( hDC );
		ReleaseDC( NULL, hDCSource );
	}

	return ( pBits.size() > 0 );
}

RGBQUAD * CIconBits::Bits()
{
	return m_pvBits.data();
}

SIZE CIconBits::Size()
{
	return m_sz;
}

BOOL CIconBits::Save(LPCTSTR pszFile)
{
	PICTDESC pd = { sizeof(pd), PICTYPE_ICON };
	pd.icon.hicon = ExportIcon();

	CComPtr<IPicture> pPict = NULL;
	CComPtr<IStream>  pStrm = NULL;
	LONG cbSize = 0;

	BOOL res = FALSE, ret = FALSE;

	res = SUCCEEDED( ::CreateStreamOnHGlobal(NULL, TRUE, &pStrm) );
	res &= SUCCEEDED( ::OleCreatePictureIndirect(&pd, IID_IPicture, TRUE, (void**)&pPict) );
	res &= SUCCEEDED( pPict->SaveAsFile( pStrm, TRUE, &cbSize ) );

	if( res )
	{
		// rewind stream to the beginning
		LARGE_INTEGER li = {0};
		STATSTG stat = { 0 };
		pStrm->Stat( &stat, 0 );
		pStrm->Seek(li, STREAM_SEEK_SET, NULL);

		// write to file
		HANDLE hFile = ::CreateFile(pszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if( INVALID_HANDLE_VALUE != hFile )
		{
			DWORD dwWritten = 0, dwRead = 0, dwDone = 0;
			std::vector<BYTE> buf( stat.cbSize.QuadPart, 0 );
			
			if( SUCCEEDED(pStrm->Read(buf.data(), LODWORD( buf.size() ), &dwRead)) )
			{
				::WriteFile(hFile, buf.data(), dwRead, &dwWritten, NULL);
				ret = ( dwRead == dwWritten );
			}
			::CloseHandle(hFile);
		}
	}

	return ret;
}