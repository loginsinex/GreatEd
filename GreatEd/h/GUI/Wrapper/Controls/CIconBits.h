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



class CIconBits
{
	std::vector<RGBQUAD>	m_pvBits;
	SIZE					m_sz;
	BOOL					GetMaskBits(HBITMAP hBitmap, std::vector<BYTE> & bits);

public:
	CIconBits();
	CIconBits(HICON hIcon);
	CIconBits(const CIconBits & bits);

	~CIconBits();

	CIconBits & operator=(const CIconBits & bits);
	
	BOOL					Create(SIZE sz);
	BOOL					ImportIcon(HBITMAP hBmp);
	BOOL					ImportIcon(HICON hIcon);
	HICON					ExportIcon();
	RGBQUAD	*				Bits();
	SIZE					Size();
	BOOL					Save(LPCTSTR pszFile);
};