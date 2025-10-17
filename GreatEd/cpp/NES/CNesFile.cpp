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

#include "h\NES\NES.h"

CPPU::CPPU()
	: m_fTilesPage2( FALSE )
{
	m_vPPU.clear();
	m_vPPU.insert( m_vPPU.begin(), 0x4000, 0 );
}

VOID CPPU::TilesPage2(BOOL fTilesPage2)
{
	m_fTilesPage2 = fTilesPage2;
}

VOID CPPU::DecodeTiles( std::vector<NES_TILE>& tile )
{
	size_t cPage = 0x1000 * ( m_fTilesPage2 ? 1 : 0 ), cEndPage = cPage + 0x1000;
	tile.clear();

	for ( size_t n = cPage; n < cEndPage; n += 0x10 )
	{
		NES_TILE tl;

		for ( int y = 0; y < 8; ++y )
		{
			for ( int x = 0; x < 8; ++x )
			{
				tl.pix[ y ][ x ] =
					( 1 & ( m_vPPU[ n + y ] >> ( 7 - x ) ) |
					( 1 & ( m_vPPU[ 8 + n + y ] >> ( 7 - x ) ) ) << 1 );

				/*
				if ( tl.pix[ y ][ x ] )
					_tprintf( TEXT( "%d" ), tl.pix[ y ][ x ] );
				else
					_tprintf( TEXT( " " ));
				*/
			}
			// _tprintf( TEXT( "\n" ) );
		}

		// _tprintf( TEXT( "\n\n" ) );

		tile.push_back( tl );
	}	
}

VOID CPPU::DrawTile( RGBQUAD * prgb, const PBYTE pbPal, NES_TILE & tile, BYTE nTile, BYTE nAttr )
{
	const int cx = 8, cy = 8, tiles = 0x100, bmWidth = tiles * cx;

	RGBQUAD * pY = ( prgb + cx * int( nTile ) + cy * bmWidth * int( nAttr ) );
	COLORREF * pcY = (COLORREF*) pY;

	for(int y = 0; y < cy; ++y)
	{
		RGBQUAD * pX = pY;
		COLORREF * pcX = pcY;

		for ( int x = 0; x < cx; ++x )
		{
			BYTE & pix = tile.pix[ y ][ x ];
			*pcX = g_crNesColor[ pbPal[ pix ] ];
			pX->rgbReserved = ( pix ? 255 : 0 );
			pcX++;
			pX++;
		}

		pY += bmWidth;
		pcY += bmWidth;
	}

	return VOID();
}

VOID CPPU::CacheTiles()
{
	std::vector<NES_TILE> vTiles;
	DecodeTiles( vTiles );

	m_bmTiles.Create( 0x100 * 8, 4 * 8 );	// 256 tiles with 4 attributes
	RGBQUAD * p = m_bmTiles.Bits();
	NES_PPU_PALETTE & pal = Palette();

	for ( size_t n = 0; n < 0x100; ++n )
	{
		DrawTile( p, &pal.bPal[  0 ], vTiles[ n ], LOBYTE( n ), 0 );
		DrawTile( p, &pal.bPal[  4 ], vTiles[ n ], LOBYTE( n ), 1 );
		DrawTile( p, &pal.bPal[  8 ], vTiles[ n ], LOBYTE( n ), 2 );
		DrawTile( p, &pal.bPal[ 12 ], vTiles[ n ], LOBYTE( n ), 3 );
	}
}

VOID CPPU::DrawTileGroup( CBitmap & bm, int x, int y, WORD nTiles, WORD nAttr )
{
	x -= ( x % 2 );
	y -= ( y % 2 );

	int uAttrId = ( ( x >> 2 ) + ( ( y >> 2 ) << 3 ) );
	int uSubAttr = ( ( ( ( x >> 1 ) % 2 ) + ( ( ( y >> 1 ) % 2 ) << 1 ) ) << 1 );

	int xbAttr = 8 * ( 0x03 & ( Attribute<BYTE>( nAttr + uAttrId ) >> uSubAttr ) );

	int bTile[ 4 ] = {
		Screen<BYTE>( LOWORD( nTiles + ( x + 0 ) + 0x20 * ( y + 0 ) ) ),
		Screen<BYTE>( LOWORD( nTiles + ( x + 1 ) + 0x20 * ( y + 0 ) ) ),
		Screen<BYTE>( LOWORD( nTiles + ( x + 0 ) + 0x20 * ( y + 1 ) ) ),
		Screen<BYTE>( LOWORD( nTiles + ( x + 1 ) + 0x20 * ( y + 1 ) ) )
	};

	bm.CopyRect( m_bmTiles, 8 * bTile[ 0 ], xbAttr, 8, 8, 8 * ( x + 0 ), 8 * ( y + 0 ) );
	bm.CopyRect( m_bmTiles, 8 * bTile[ 1 ], xbAttr, 8, 8, 8 * ( x + 1 ), 8 * ( y + 0 ) );
	bm.CopyRect( m_bmTiles, 8 * bTile[ 2 ], xbAttr, 8, 8, 8 * ( x + 0 ), 8 * ( y + 1 ) );
	bm.CopyRect( m_bmTiles, 8 * bTile[ 3 ], xbAttr, 8, 8, 8 * ( x + 1 ), 8 * ( y + 1 ) );

	/*
	CString s;
	s.Format( TEXT( "%01X" ), xbAttr / 8 );
	bm.RenderText( 8 * x, 8 * y, 16, 16, s, GetStockFont( SYSTEM_FONT ), TRUE, RGB( 0xff, 0xff, 0xff ), 0, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
	*/
}

BOOL CPPU::TilesPage2()
{
	return m_fTilesPage2;
}

BOOL CPPU::DrawPage( int page, CBitmap & bm )
{
	if ( page < 0 || page >= 4 )
		return FALSE;

	CacheTiles();

	bm.Create( 256, 240 );
	const WORD nTileByte = 0x2000 + page * 0x400;
	const WORD nAttrByte = nTileByte + 0x3C0;
	
	for ( int y = 0; y < 30; y += 2 )
	{
		for ( int x = 0; x < 32; x += 2 )
		{
			DrawTileGroup( bm, x, y, nTileByte, nAttrByte );
		}
	}
	

	// bm.CopyRect( m_bmTiles, 0, 0, 256, 4 * 8, 0, 0 );

	return TRUE;
}

BOOL CPPU::DrawPageWithScroll( int scrollX, int scrollY, CBitmap & bm )
{
	return FALSE;
}

// ---------------------------------------------------------------------

CNesFile::CNesFile()
	: m_uMapper( 0 ), m_fFileLoaded( FALSE )
{
	ZeroMemory(&m_iNesHeader, sizeof(m_iNesHeader));
}

VOID CNesFile::SetHeader(LPCTSTR pszFile, NES_HEADER nHeader)
{
	m_iNesHeader.uSignature = NES_FILE_SIGNATURE;
	m_iNesHeader.fReserved = 0;
	m_iNesHeader.fReserved2 = m_iNesHeader.fReserved3 = 3;
	m_iNesHeader.fMirroring = nHeader.fMirroring;
	m_iNesHeader.fMapperHi = nHeader.fMapperHi;
	m_iNesHeader.fMapperLo = nHeader.fMapperLo;
	m_iNesHeader.bChrCount = LOBYTE( m_vCHR.size() / NES_CHR_SIZE );
	m_iNesHeader.bPrgCount = LOBYTE( m_vPRG.size() );
	m_iNesHeader.bPrgRamSize = nHeader.bPrgRamSize;
	m_iNesHeader.fBusConflicts = nHeader.fBusConflicts;
	m_iNesHeader.fHeaderFormat = nHeader.fHeaderFormat;
	m_iNesHeader.fPlayChoice10 = nHeader.fPlayChoice10;
	m_iNesHeader.fPrgRAMPresent = nHeader.fPrgRAMPresent;
	m_iNesHeader.fPrgRamPresentUO = nHeader.fPrgRamPresentUO;
	m_iNesHeader.fSquareVRAM = nHeader.fSquareVRAM;
	m_iNesHeader.fTrainerPresent = FALSE;
	m_iNesHeader.fTVSystem = nHeader.fTVSystem;
	m_iNesHeader.fTVSystemUO = nHeader.fTVSystemUO;
	m_iNesHeader.fVSUnisystem = nHeader.fVSUnisystem;
	m_fFileLoaded = ( m_iNesHeader.bChrCount > 0 && m_iNesHeader.bPrgCount > 0 );
	m_sFilename = pszFile;
}

BOOL CNesFile::ParseFile()
{
	if ( m_vFile.size() < 0x10 )
		return FALSE;

	PNES_HEADER pNes = (PNES_HEADER) &m_vFile[0];

	if ( pNes->uSignature != NES_FILE_SIGNATURE )
		return FALSE;

	m_uMapper = pNes->fMapperLo | ( pNes->fMapperHi << 4 );

	size_t uMinimumSize = 
		size_t(pNes->bPrgCount) * NES_PRG_SIZE +
		size_t(pNes->bChrCount) * NES_CHR_SIZE +
		size_t( pNes->fTrainerPresent ? 0x200 : 0 ) +
		sizeof(NES_HEADER);

	if ( m_vFile.size() < uMinimumSize )
		return FALSE;

	std::vector<BYTE>::iterator ptr = m_vFile.begin() + sizeof(NES_HEADER);
	if ( pNes->fTrainerPresent )
	{
		m_vTrainer.insert(m_vTrainer.begin(), 
			ptr,
			ptr + 0x200
			);
		ptr += 0x200;
	}

	for(int i = 0; i < pNes->bPrgCount; ++i)
	{
		std::vector<BYTE> vPRG;
		vPRG.insert(vPRG.begin(), ptr, ptr + NES_PRG_SIZE);
		m_vPRG.push_back(vPRG);
		ptr += NES_PRG_SIZE;
	}

	for(int i = 0; i < pNes->bChrCount; ++i)
	{
		m_vCHR.insert(m_vCHR.end(), ptr, ptr + NES_CHR_SIZE);
		ptr += NES_CHR_SIZE;
	}

	CopyMemory(&m_iNesHeader, pNes, sizeof(m_iNesHeader));
	m_fFileLoaded = TRUE;

	return TRUE;
}

BOOL CNesFile::InternalLoadFile(LPCTSTR pszFile)
{
	if ( m_fFileLoaded )
		return FALSE;

	BOOL fResult = FALSE;

	HANDLE hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER li = { 0 };
		if ( GetFileSizeEx(hFile, &li) && li.QuadPart <= 10 * ( 1 << 20 ) )	// maximum 10 MB
		{
			DWORD w = 0;
			m_vFile.clear();
			m_vFile.insert(m_vFile.begin(), li.LowPart, 0);

			if ( ::ReadFile(hFile, &m_vFile[0], li.LowPart, &w, NULL) && w == li.LowPart )
				fResult = TRUE;
		}
		CloseHandle(hFile);
	}

	if ( fResult )
		fResult = ParseFile();

	if ( !fResult )
		InternalFreeFile();
	else
		m_sFilename = pszFile;

	return fResult;
}

BOOL CNesFile::InternalSaveFile(LPCTSTR pszFile)
{
	if ( !m_fFileLoaded )
		return FALSE;

	size_t ptr = 0;

	m_iNesHeader.bPrgCount = LOBYTE(m_vPRG.size());
	m_iNesHeader.bChrCount = LOBYTE(m_vCHR.size() / NES_CHR_SIZE);
	m_iNesHeader.fTrainerPresent = ( m_vTrainer.size() > 0 );

	m_vFile.clear();
	m_vFile.insert(m_vFile.begin(), sizeof(NES_HEADER), 0);

	CopyMemory(&m_vFile[ptr], &m_iNesHeader, sizeof(m_iNesHeader));
	ptr += sizeof(m_iNesHeader);
	if ( m_vTrainer.size() > 0 )
	{
		m_vFile.insert(m_vFile.end(), m_vTrainer.begin(), m_vTrainer.end());
		ptr += m_vTrainer.size();
	}

	for(int i = 0; i < m_iNesHeader.bPrgCount; ++i)
	{
		m_vFile.insert(m_vFile.end(), m_vPRG[i].begin(), m_vPRG[i].end());
		ptr += NES_PRG_SIZE;
	}

	m_vFile.insert(m_vFile.end(), m_vCHR.begin(), m_vCHR.end());

	if ( !pszFile )
		pszFile = m_sFilename.c_str();

	HANDLE hFile = CreateFile(pszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

	BOOL fResult = FALSE;
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD w = 0;
		if ( ::WriteFile(hFile, &m_vFile[0], (DWORD) (m_vFile.size()), &w, NULL) && w == m_vFile.size() )
			fResult = TRUE;

		CloseHandle(hFile);
	}

	if ( fResult )
		m_sFilename = pszFile;

	return fResult;
}

BOOL CNesFile::InternalFreeFile()
{
	m_sFilename = TEXT("");
	m_uMapper = 0;
	m_fFileLoaded = FALSE;

	m_vFile.clear();
	m_vPRG.clear();
	m_vTrainer.clear();
	m_vCHR.clear();
	ZeroMemory(&m_iNesHeader, sizeof(m_iNesHeader));
	return TRUE;
}

BOOL CNesFile::IsFileLoaded()
{
	return m_fFileLoaded;
}

BYTE CNesFile::Mapper()
{
	return m_uMapper;
}

CNesFile::operator LPCTSTR()
{
	return m_sFilename.c_str();
}

CNesFile::operator tstring()
{
	return m_sFilename;
}

NES_HEADER CNesFile::GetHeader()
{
	return m_iNesHeader;
}

std::vector<BYTE> &	CNesFile::AddPrg()
{
	if ( !IsFileLoaded() )
		throw std::exception("tried to extend file while its not being loaded");

	std::vector<BYTE> prg(NES_PRG_SIZE, 0);
	m_vPRG.insert(m_vPRG.end() - 1, prg);
	m_iNesHeader.bPrgCount = LOBYTE( m_vPRG.size() );

	return m_vPRG[0];
}

BOOL CNesFile::Get_CHR(std::vector<BYTE> & vCHR)
{
	if ( !IsFileLoaded() )
		return FALSE;

	vCHR = m_vCHR;
	return TRUE;
}

BOOL CNesFile::Get_PRG(std::vector< std::vector<BYTE> > & vPRG)
{
	if ( !IsFileLoaded() )
		return FALSE;

	vPRG = m_vPRG;
	return TRUE;
}

BOOL CNesFile::InternalTilesLoaded()
{
	return ( m_vInternalTileset.size() > 0 );
}

VOID CNesFile::InternalLoadTiles(size_t nOffset, size_t nCount)
{
	size_t count = m_vCHR.size();
	m_vInternalTileset.clear();
	if ( nOffset + 0x10 * nCount >= count )
		throw std::exception("out of CHR bank while loading tiles");

	for(size_t n = 0; n < 0x10 * nCount; n += 0x10)
	{
		NES_TILE tile = { 0 };
		size_t nTile = n + nOffset;

		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				tile.pix[y][x] = 
					( 1 & ( m_vCHR[nTile + y] >> ( 7 - x ) ) | 
					( 1 & ( m_vCHR[8 + nTile + y] >> ( 7 - x ) ) ) << 1 );
			}
		}

		m_vInternalTileset.push_back(tile);
	}
}

VOID CNesFile::InternalStorTiles(size_t nOffset, size_t nCount)
{
	size_t count = m_vCHR.size();
	if ( nOffset + 0x10 * nCount >= count )
		throw std::exception("out of CHR bank while storing tiles");

	if ( nCount > m_vInternalTileset.size() )
		throw std::exception("too many tiles requested to store");

	for(size_t n = 0; n < 0x10 * nCount; n += 0x10)
	{
		NES_TILE & tile = m_vInternalTileset[ n / 0x10 ];
		size_t nTile = n + nOffset;

		for(int y = 0; y < 8; ++y)
		{
			m_vCHR[nTile + y] = 0;
			m_vCHR[8 + nTile + y] = 0;
			for(int x = 0; x < 8; ++x)
			{
				m_vCHR[nTile + y] |= (tile.pix[y][x] & 1) << (7 - x);
				m_vCHR[8 + nTile + y] |= ((tile.pix[y][x] >> 1) & 1 ) << (7 - x);
			}
		}
	}
}

VOID CNesFile::InternalDrawTile(HDC hDC, INT cx, INT cy, WORD ntile, PBYTE pbPal, BOOL fMirrorX, BOOL fMirrorY)
{
	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };

	if ( pbPal )
	{
		clr[1] = g_crNesColor[ pbPal[1] ];
		clr[2] = g_crNesColor[ pbPal[2] ];
		clr[3] = g_crNesColor[ pbPal[3] ];
	}

	CBitmap tbm(8, 8);
	HDC hTileDC = tbm;

	BitBlt(hTileDC, 0, 0, tbm.Width(), tbm.Height(), hDC, cx, cy, SRCCOPY);
	tbm.FreeDC();

	NES_TILE & tile = m_vInternalTileset[ ntile ];

	COLORREF * pclr = (COLORREF*) tbm.Bits();
	int bit = 0;

	if ( !fMirrorX && !fMirrorY )
	{
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				if ( tile.pix[y][x] )
					// *((COLORREF*) &tbm[y][x]) = clr[ tile.pix[ty][tx] ];
					pclr[bit] = clr[ tile.pix[y][x] ];
				bit++;
			}
		}
	} 
	else if ( fMirrorX && !fMirrorY )
	{
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 7; x >= 0; --x)
			{
				if ( tile.pix[y][x] )
					pclr[bit] = clr[ tile.pix[y][x] ];
				bit++;
			}
		}
	}
	else if ( !fMirrorX && fMirrorY )
	{
		for(int y = 7; y >= 0; --y)
		{
			for(int x = 0; x < 8; ++x)
			{
				if ( tile.pix[y][x] )
					pclr[bit] = clr[ tile.pix[y][x] ];
				bit++;
			}
		}	
	}
	else if ( fMirrorX && fMirrorY )
	{
		for(int y = 7; y >= 0; --y)
		{
			for(int x = 7; x >= 0; --x)
			{
				if ( tile.pix[y][x] )
					pclr[bit] = clr[ tile.pix[y][x] ];
				bit++;
			}
		}	
	}

	BitBlt(hDC, cx, cy, tbm.Width(), tbm.Height(), tbm, 0, 0, SRCCOPY);
	tbm.FreeDC();
}

VOID CNesFile::InternalDrawTileset(HDC hDC, INT tx, INT ty)
{
	return;
	/*
	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };

	CBitmap tbm[2];
	tbm[0].Create(128, 128); tbm[1].Create(128, 128);
	size_t nCount = m_vInternalTileset.size();

	for(int i = 0; i < nCount; ++i)
	{
		CBitmap & bm = tbm[i / 0x100];
		INT ntile = (i % 0x100), tx = ntile % 16, ty = ntile / 16;
		NES_TILE & tile = m_vInternalTileset[ i ];
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				*((COLORREF*) &bm[8 * ty + y][8 * tx + x]) = clr[ tile.pix[y][x] ]; // clr[ m_vTileset[ m_vTiles[i] ].pix[y][x]];
			}
		}
	}

	StretchBlt(hDC, tx, ty, 2 * tbm[0].Width(), 2 * tbm[0].Height(), tbm[0], 0, 0, 
		tbm[0].Width(), tbm[0].Height(),
		SRCCOPY);

	StretchBlt(hDC, tx + 260, ty, 2 * tbm[1].Width(), 2 * tbm[1].Height(), tbm[1], 0, 0, 
		tbm[1].Width(), tbm[1].Height(),
		SRCCOPY);
	*/
}

// mmc3

CNesSimpleMMC3::CNesSimpleMMC3()
{
	ZeroMemory(m_uSelectedBank, sizeof(m_uSelectedBank));
	m_uSelectedBank[0] = 0x00;
	m_uSelectedBank[1] = 0x07;
	ZeroMemory(m_uSelectedCHR, sizeof(m_uSelectedCHR));
	//m_bmDrawTileCache.Create( 8, 8 );
}

BOOL CNesSimpleMMC3::IsROMExtended()
{
	return ( GetHeader().bPrgCount == 0x10 );
}

BOOL CNesSimpleMMC3::SetExROM()
{
	if ( !IsFileLoaded() )
		return FALSE;

	if ( IsROMExtended() )
		return TRUE;

	if ( GetHeader().bPrgCount != 0x08 )
		return FALSE;

	RevertPRG();
//	BYTE bBank1 = Data<BYTE>(0x8000), bBank2 = Data<BYTE>(0xC000);

	for(BYTE i = 0x08; i < 0x10; ++i)
		AddPrg();

//	m_uSelectedBank[0] = FindBank(bBank1);
//	m_uSelectedBank[1] = FindBank(bBank2);
	ApplyPRG();

	return TRUE;
}

BOOL CNesSimpleMMC3::LoadFile(LPCTSTR pszFile)
{
	if ( InternalLoadFile(pszFile) )
	{
		if ( Mapper() != 0x04 || ( m_vPRG.size() != 0x08 && m_vPRG.size() != 0x10 ) || m_vCHR.size() != 0x20000 )
		{
			InternalFreeFile();
			return FALSE;
		}
		m_strInternalFilename = pszFile;
		return InitMapper();
	}
	return FALSE;
}

BOOL CNesSimpleMMC3::InitMapper()
{
	m_uSelectedBank[0] = 0;
	m_uSelectedBank[1] = 1;

	m_vRAM.clear();
	m_vPPU.clear();
	m_vRAM.insert(m_vRAM.begin(), 0x8000, 0);
	m_vPPU.insert(m_vPPU.begin(), 0x2000, 0);
	LoadTiles();
	ApplyPRG();
	ApplyCHR();

	CopyMemory(&m_vRAM[0x4000], &m_vPRG[ m_vPRG.size() - 1 ][0], NES_PRG_SIZE);		// 0xc000 hardwared for this game

	return TRUE;
}

VOID CNesSimpleMMC3::ApplyPRG()
{	
	CopyMemory(&m_vRAM[0x0000], &m_vPRG[m_uSelectedBank[0] / 2 ][0x2000 * ( m_uSelectedBank[0] % 2 )], NES_PRG_SIZE / 2);
	CopyMemory(&m_vRAM[0x2000], &m_vPRG[m_uSelectedBank[1] / 2 ][0x2000 * ( m_uSelectedBank[1] % 2 )], NES_PRG_SIZE / 2);
}

VOID CNesSimpleMMC3::RevertPRG()
{
	CopyMemory(&m_vPRG[m_uSelectedBank[0] / 2 ][0x2000 * ( m_uSelectedBank[0] % 2 )], &m_vRAM[0x0000], NES_PRG_SIZE / 2);
	CopyMemory(&m_vPRG[m_uSelectedBank[1] / 2 ][0x2000 * ( m_uSelectedBank[1] % 2 )], &m_vRAM[0x2000], NES_PRG_SIZE / 2);
}

VOID CNesSimpleMMC3::ApplyCHR()
{
	POINT c[6] =
	{
		{ m_uSelectedCHR[0] * NES_CHR_BANK, 2 * NES_CHR_BANK },
		{ m_uSelectedCHR[2] * NES_CHR_BANK, 2 * NES_CHR_BANK },
		{ m_uSelectedCHR[4] * NES_CHR_BANK, NES_CHR_BANK },
		{ m_uSelectedCHR[5] * NES_CHR_BANK, NES_CHR_BANK },
		{ m_uSelectedCHR[6] * NES_CHR_BANK, NES_CHR_BANK },
		{ m_uSelectedCHR[7] * NES_CHR_BANK, NES_CHR_BANK }
	};

	INT ptr = 0;
	for(int i = 0; i < sizeof(c) / sizeof(c[0]); ++i)
	{
		for(int n = 0; n < c[i].y; n += 0x10)
			m_vTiles[(ptr + n) / 0x10 ] = LOWORD(( c[i].x + n ) / 0x10);

		ptr += c[i].y;
	}
}

VOID CNesSimpleMMC3::LoadTiles()
{
	size_t count = m_vCHR.size();
	m_vTileset.clear();
	for(size_t n = 0; n < count; n += 0x10)
	{
		NES_TILE tile = { 0 };
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				tile.pix[y][x] = 
					( 1 & ( m_vCHR[n + y] >> ( 7 - x ) ) | 
					( 1 & ( m_vCHR[8 + n + y] >> ( 7 - x ) ) ) << 1 );
			}
		}

		m_vTileset.push_back(tile);
	}
}

VOID CNesSimpleMMC3::StorTiles()
{
	size_t count = m_vCHR.size();
	for(size_t n = 0; n < count; n += 0x10)
	{
		NES_TILE & tile = m_vTileset[ n / 0x10 ];
		
		for(int y = 0; y < 8; ++y)
		{
			m_vCHR[n + y] = 0;
			m_vCHR[8 + n + y] = 0;
			for(int x = 0; x < 8; ++x)
			{
				m_vCHR[n + y] |= (tile.pix[y][x] & 1) << (7 - x);
				m_vCHR[8 + n + y] |= ((tile.pix[y][x] >> 1) & 1 ) << (7 - x);
			}
		}
	}
}

BOOL CNesSimpleMMC3::SaveFile(LPCTSTR pszFile)
{
	if ( !IsFileLoaded() )
		return FALSE;
	
	CopyMemory(&m_vPRG[m_vPRG.size() - 1][0], &m_vRAM[0x4000], NES_PRG_SIZE);	// 0xc000 hardwared for this game
	RevertPRG();
	StorTiles();
	if ( InternalSaveFile( pszFile ) )
	{
		if ( pszFile ) m_strInternalFilename = pszFile;
		return TRUE;
	}

	return FALSE;
}

BOOL CNesSimpleMMC3::FreeFile()
{
	m_vRAM.clear();
	m_vPPU.clear();
	m_vTileset.clear();
	m_strInternalFilename = CString();
	return InternalFreeFile();
}

const CString & CNesSimpleMMC3::Name() const
{
	return m_strInternalFilename;
}

BYTE CNesSimpleMMC3::FindBank(BYTE bBank)
{
	size_t count = m_vPRG.size(), bank = -1;
	for(size_t n = 0; n < count; ++n)
		if ( m_vPRG[n][0] == bBank ) bank = n;

	return LOBYTE(bank);
}

BOOL CNesSimpleMMC3::SelectPRG(BYTE bBank, BOOL fLast)
{
	size_t count = m_vPRG.size(); //, bank = FindBank(bBank);

	if ( bBank / 2 < count )
	{
		RevertPRG();
		m_uSelectedBank[ fLast ? 1 : 0 ] = LOBYTE(bBank);
		ApplyPRG();
		return TRUE;
	}
	return FALSE;
}

BOOL CNesSimpleMMC3::SelectCHR(BYTE bRegister, BYTE bBank)
{
	if ( bRegister >= 6 )
		return FALSE;

	BYTE b[6] = { 
		m_uSelectedCHR[0], m_uSelectedCHR[2],
		m_uSelectedCHR[4], m_uSelectedCHR[5],
		m_uSelectedCHR[6], m_uSelectedCHR[7]
		};

	b[bRegister] = bBank;
	return SelectCHR(b);
}

BOOL CNesSimpleMMC3::SelectCHR(BYTE bBank[6])
{
	for(int i = 0; i < 6; ++i)
		if ( bBank[i] >= 0x80 )
			return FALSE;

	if ( bBank[0] >= 0x7f || bBank[1] >= 0x7f )
		return FALSE;

	m_uSelectedCHR[0] = bBank[0] & 0xFE;
	m_uSelectedCHR[1] = bBank[0] | 0x1;
	m_uSelectedCHR[2] = bBank[1] & 0xFE;
	m_uSelectedCHR[3] = bBank[1] | 0x1;
	m_uSelectedCHR[4] = bBank[2];
	m_uSelectedCHR[5] = bBank[3];
	m_uSelectedCHR[6] = bBank[4];
	m_uSelectedCHR[7] = bBank[5];

	ApplyCHR();
	return TRUE;
}

VOID CNesSimpleMMC3::Get_Tiles(std::vector<NES_TILE> & vTiles)
{
	vTiles.clear();
	for(size_t n = 0; n < 0x200; ++n)
		vTiles.push_back( Tile( LODWORD( n ) ) );
}

VOID CNesSimpleMMC3::Let_Tiles(std::vector<NES_TILE> & vTiles)
{
	if ( vTiles.size() != 0x200 )
		return;

	for(size_t n = 0; n < 0x200; ++n)
		Tile( LODWORD( n ) ) = vTiles[ n ];
}

BOOL CNesSimpleMMC3::FlushPPU( CPPU & ppu )
{
	if ( !IsFileLoaded() )
		return FALSE;

	size_t ppuPtr = 0;
	for ( size_t n = 0; n < _ARRAYSIZE( m_uSelectedCHR ); ++n )
	{
		size_t ptr = size_t( m_uSelectedCHR[ n ] ) * NES_CHR_BANK;
		for ( size_t p = 0; p < NES_CHR_BANK; ++p )
		{
			ppu.m_vPPU[ ppuPtr ] = m_vCHR[ ptr ];
			ppuPtr++;
			ptr++;
		}
	}
	return TRUE;
}

VOID CNesSimpleMMC3::DrawTile( CNesBitmap & bm, DWORD x, DWORD y, BYTE tileId, BYTE bAttribute, BOOL fSprite, BOOL fFlipHorz, BOOL fFlipVert )
{
	WORD nTile = tileId;
	if ( !fSprite ) nTile += 0x100;
	const NES_TILE & tile = Tile( nTile );
	
	if ( fSprite )
	{
		bm.BlitSprite( tile, x, y, bAttribute, fFlipHorz, fFlipVert );
	}
	else
	{
		bm.BlitTile( tile, x, y, bAttribute );
	}
}

VOID CNesSimpleMMC3::DrawTiles(HDC hDC, INT tx, INT ty)
{
	return;

	/*
	COLORREF clr[4] = { 0, 0x808080, 0xA0A0A0, 0xFFFFFF };

	CBitmap tbm[2];
	tbm[0].Create(128, 128); tbm[1].Create(128, 128);

	for(int i = 0; i < 0x200; ++i)
	{
		CBitmap & bm = tbm[i / 0x100];
		INT ntile = (i % 0x100), tx = ntile % 16, ty = ntile / 16;
		NES_TILE & tile = Tile(i);
		for(int y = 0; y < 8; ++y)
		{
			for(int x = 0; x < 8; ++x)
			{
				*((COLORREF*) &bm[8 * ty + y][8 * tx + x]) = clr[ tile.pix[y][x] ]; // clr[ m_vTileset[ m_vTiles[i] ].pix[y][x]];
			}
		}
	}

	StretchBlt(hDC, tx, ty, 2 * tbm[0].Width(), 2 * tbm[0].Height(), tbm[0], 0, 0, 
		tbm[0].Width(), tbm[0].Height(),
		SRCCOPY);

	StretchBlt(hDC, tx + 260, ty, 2 * tbm[1].Width(), 2 * tbm[1].Height(), tbm[1], 0, 0, 
		tbm[1].Width(), tbm[1].Height(),
		SRCCOPY);
	*/
}

BOOL CNesSimpleMMC3::CreateNew(LPCTSTR pszFile, BOOL fMirroring, std::vector< std::vector<BYTE> > & vPRG, std::vector<BYTE> & vCHR)
{
	if ( ( vPRG.size() != 0x08 && vPRG.size() != 0x10 ) || vCHR.size() != 0x20000 )
		return FALSE;

	m_vPRG = vPRG;
	m_vCHR = vCHR;

	NES_HEADER nHeader = { 0 };
	nHeader.fMirroring = fMirroring;
	nHeader.fMapperHi = 0x00;
	nHeader.fMapperLo = 0x04;
	SetHeader( pszFile, nHeader );
	return InitMapper();
}

VOID CNesSimpleMMC3::DrawTileset(HDC hDC, int x, int y)
{
	if ( !IsFileLoaded() )
		return;

	InternalDrawTileset(hDC, x, y);
}

#ifdef _DEBUG
VOID CNesSimpleMMC3::Protect(WORD ptr, BOOL fProtect)
{
	if ( ptr >= 0x8000 && ptr <= 0xFFFF )
	{
		DWORD dwOldValue = 0;
		VirtualProtect( &m_vRAM[ ptr - 0x8000 ], 1, ( fProtect ? PAGE_EXECUTE_READ : PAGE_EXECUTE_READWRITE ), &dwOldValue );
	}
}
#else
VOID CNesSimpleMMC3::Protect(WORD ptr, BOOL fProtect)
{

}
#endif

CNesUNROM::CNesUNROM()
	: m_bSelectedBank( 0 )
{

}

BOOL CNesUNROM::LoadFile(LPCTSTR pszFile)
{
	if ( InternalLoadFile(pszFile) )
	{
		if ( Mapper() != 0x02 || ( m_vPRG.size() != 0x08 && m_vPRG.size() != 0x10 ) )
		{
			InternalFreeFile();
			return FALSE;
		}
		
		m_bSelectedBank = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL CNesUNROM::SaveFile(LPCTSTR pszFile)
{
	if ( !IsFileLoaded() )
		return FALSE;
	
	return InternalSaveFile(pszFile);
}
	
BOOL CNesUNROM::FreeFile()
{
	return InternalFreeFile();
}

BOOL CNesUNROM::SelectBank(BYTE bBankId)
{
	if ( !IsFileLoaded() || bBankId >= m_vPRG.size() )
		return FALSE;

	m_bSelectedBank = bBankId;
	return TRUE;
}

// -- CNROM

CNesNROM::CNesNROM()
{

}

BOOL CNesNROM::LoadFile(LPCTSTR pszFile)
{
	if ( InternalLoadFile(pszFile) )
	{
		if ( Mapper() != 0x00 || ( m_vPRG.size() != 0x02 && m_vPRG.size() != 0x04 ) )
		{
			InternalFreeFile();
			return FALSE;
		}
		
		m_vRAM.clear();
		m_vRAM.insert( m_vRAM.begin(), 0x8000, 0);
		CopyMemory(&m_vRAM[ 0x0000 ], &m_vPRG[ 0 ][ 0 ], m_vPRG[ 0 ].size());
		CopyMemory(&m_vRAM[ 0x4000 ], &m_vPRG[ 1 ][ 0 ], m_vPRG[ 1 ].size());

		return TRUE;
	}
	return FALSE;
}

BOOL CNesNROM::SaveFile(LPCTSTR pszFile)
{
	if ( !IsFileLoaded() )
		return FALSE;

	CopyMemory(&m_vPRG[ 0 ][ 0 ], &m_vRAM[ 0x0000 ], m_vPRG.size());
	CopyMemory(&m_vPRG[ 1 ][ 0 ], &m_vRAM[ 0x4000 ], m_vPRG.size());

	return InternalSaveFile( pszFile );
}

BOOL CNesNROM::FreeFile()
{
	return InternalFreeFile();
}

VOID CNesNROM::DrawTile(HDC hDC, INT cx, INT cy, WORD ntile, PBYTE pbPal, BOOL fMirrorX, BOOL fMirrorY)
{
	InternalDrawTile(hDC, cx, cy, ntile, pbPal, fMirrorX, fMirrorY);
}

VOID CNesNROM::DrawTileset(HDC hDC, int x, int y)
{
	if ( !IsFileLoaded() )
		return;

	InternalDrawTileset(hDC, x, y);
}