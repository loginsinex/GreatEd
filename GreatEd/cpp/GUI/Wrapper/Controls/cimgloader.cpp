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

#pragma comment(lib, "gdiplus.lib")

CPngLoader::CPngLoader(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType)
	:
	CImgLoader(hInstance, lpszResName, lpszResType)
{

}

CPngLoader::CPngLoader(LPCTSTR lpszFilename)
	:
	CImgLoader(lpszFilename)
{

}

CPngLoader::CPngLoader(CBitmap & bmp)
	: CImgLoader(bmp)
{

}

CLSID CPngLoader::DecoderCLSID()
{
	return CLSID_NULL; // CLSID_WICPngDecoder;
}

BOOL CPngLoader::SavePng(LPCTSTR pszFilename)
{
	if ( !Handle() )
		return FALSE;

	CLSID   encoderClsid;
	// Get the CLSID of the PNG encoder.
	if ( -1 == GetEncoderClsid(L"image/png", &encoderClsid) )
		return FALSE;

	Handle()->Save(pszFilename, &encoderClsid, NULL);

	return TRUE;
}

CImgLoader::CImgLoader(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType)
	:
	m_pStream( NULL ),
	m_pgpBmp( NULL )
{

	HGLOBAL hMem = StreamFromResource( hInstance, lpszResName, lpszResType );
	if ( hMem )
	{
		if ( !LoadToStream( hMem ) )
		{
			GlobalFree( hMem );
			hMem = NULL;
		}
		else
		{
			m_pgpBmp = new Gdiplus::Bitmap(m_pStream);
		}

	}

}

CImgLoader::CImgLoader(LPCTSTR lpszFilename)
	:
	m_pStream( NULL ),
	m_pgpBmp( NULL )
{
	HGLOBAL hMem = StreamFromFile( lpszFilename );
	if ( hMem )
	{
		if ( !LoadToStream( hMem ) )
		{
			GlobalFree( hMem );
			hMem = NULL;
		}
		else
		{
			m_pgpBmp = new Gdiplus::Bitmap(m_pStream);
		}
	}
}

CImgLoader::CImgLoader(CBitmap & bmp)
	:
	m_pStream( NULL ),
	m_pgpBmp( NULL )
{
	HBITMAP hbmp = bmp.Bitmap();
	m_pgpBmp = new Gdiplus::Bitmap(hbmp, NULL);
	DeleteObject(hbmp);
}

CImgLoader::~CImgLoader()
{
	if ( m_pgpBmp )
		delete m_pgpBmp;

	if ( m_pStream )
		m_pStream->Release();
}

HGLOBAL CImgLoader::StreamFromFile(LPCTSTR lpszFilename)
{
	HGLOBAL hMem = NULL;

	HANDLE hFile = CreateFile(lpszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER uFilesize = { 0 };
		
		if ( GetFileSizeEx( hFile, &uFilesize ) && uFilesize.QuadPart < IMGLOADER_ALLOWED_FILE_SIZE )
		{
			hMem = GlobalAlloc( GHND, uFilesize.LowPart );
			if ( hMem )
			{
				BOOL fSuccess = FALSE;
				LPVOID lpvData = GlobalLock( hMem );
				if ( lpvData )
				{
					DWORD dw = 0;
					fSuccess = ( ReadFile( hFile, lpvData, uFilesize.LowPart, &dw, NULL ) && uFilesize.LowPart == dw );
					GlobalUnlock( hMem );
				}

				if ( !fSuccess )
				{
					GlobalFree( hMem );
					hMem = NULL;
				}
			}
		}
		CloseHandle( hFile );
	}

	return hMem;
}

HGLOBAL CImgLoader::StreamFromResource(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType)
{
	HGLOBAL hMem = NULL;
	HRSRC hRes = FindResource(hInstance, lpszResName, lpszResType);
	if ( hRes )
	{
		DWORD dwSize = SizeofResource( hInstance, hRes );
		HGLOBAL hData = LoadResource( hInstance, hRes );
		if ( hData )
		{
			LPVOID pvData = LockResource( hData );
			if ( pvData )
			{
				hMem = GlobalAlloc( GHND, dwSize );
				if ( hMem )
				{
					LPVOID pvTarget = GlobalLock( hMem );
					if ( pvTarget )
					{
						CopyMemory( pvTarget, pvData, dwSize );
						GlobalUnlock( hMem );
					}
					else
					{
						GlobalFree( hMem );
						hMem = NULL;
					}
				}
			}
		}
	}

	return hMem;
}

BOOL CImgLoader::LoadToStream(HGLOBAL hData)
{
	return SUCCEEDED( CreateStreamOnHGlobal( hData, TRUE, &m_pStream ) );
}

HBITMAP CImgLoader::Bitmap(SIZE & sz)
{
	if ( !m_pgpBmp )
		return NULL;

	HBITMAP hBmp;
	UINT w = 0, h = 0;
	if ( Gdiplus::Ok ==  m_pgpBmp->GetHBITMAP(Gdiplus::Color(), &hBmp) )
	{
		sz.cx = m_pgpBmp->GetWidth();
		sz.cy = m_pgpBmp->GetHeight();
		return hBmp;
	}

	return NULL;

}

BOOL CImgLoader::Bitmap(CBitmap & bmp, BOOL fPreAlpha)
{
	return FixedBitmap(bmp);
}

BOOL CImgLoader::FixedBitmap(CBitmap & bmp)
{
	if ( !m_pgpBmp )
		return FALSE;

	HBITMAP hBmp;
	UINT w = 0, h = 0;
	if ( Gdiplus::Ok ==  m_pgpBmp->GetHBITMAP(Gdiplus::Color(), &hBmp) )
	{
		bmp.Create(m_pgpBmp->GetWidth(), m_pgpBmp->GetHeight());
		bmp.CreateFromBitmap(hBmp);
		return TRUE;
	}

	return FALSE;
}

Gdiplus::Bitmap * CImgLoader::Handle()
{
	return m_pgpBmp;
}

int CImgLoader::GetEncoderClsid(LPCTSTR format, CLSID * pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(new BYTE[size]);
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         delete[] (PBYTE) (pImageCodecInfo);
         return j;  // Success
      }    
   }

   delete[] (PBYTE) (pImageCodecInfo);
   return -1;  // Failure
}