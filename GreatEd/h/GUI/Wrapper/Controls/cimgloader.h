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



#define				IMGLOADER_ALLOWED_FILE_SIZE				( 64 * ( 1 << 20 ) )		// 64 MB

class CImgLoader
{
	IStream				*m_pStream;
	Gdiplus::Bitmap		*m_pgpBmp;

	HGLOBAL				StreamFromFile(LPCTSTR lpszFilename);
	HGLOBAL				StreamFromResource(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType);
	BOOL				LoadToStream(HGLOBAL hData);
//	IWICBitmapSource *	BitmapFromStream(BOOL fPreAlpha, BOOL fNoAlpha);

protected:
	virtual CLSID		DecoderCLSID() PURE;
	int					GetEncoderClsid(LPCTSTR format, CLSID* pClsid);
	Gdiplus::Bitmap *	Handle();

public:
	CImgLoader(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType);
	CImgLoader(LPCTSTR lpszFilename);
	CImgLoader(CBitmap & bmp);

	virtual				~CImgLoader();
	BOOL				Bitmap(CBitmap & bmp, BOOL fPreAlpha = FALSE);
	HBITMAP				Bitmap(SIZE & sz);
	BOOL				FixedBitmap(CBitmap & bmp);
};

class CPngLoader: public CImgLoader
{
	CLSID				DecoderCLSID();

public:
	CPngLoader(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType);
	CPngLoader(LPCTSTR lpszFilename);
	CPngLoader(CBitmap & bmp);
	BOOL				SavePng(LPCTSTR pszFilename);
};