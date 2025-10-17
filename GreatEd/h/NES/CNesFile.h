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


#define			NES_FILE_SIGNATURE		0x1A53454E
#define			NES_PRG_SIZE			0x4000
#define			NES_CHR_SIZE			0x2000
#define			NES_CHR_BANK			0x400

#pragma pack(push, 1)

typedef struct _tagNES_HEADER
{
	union
	{
		BYTE		bSignature[4];
		DWORD		uSignature;
	};

	BYTE		bPrgCount;
	BYTE		bChrCount;
	union
	{
		BYTE	bFlag6;
		struct
		{
			BYTE fMirroring:1;
			BYTE fPrgRAMPresent:1;
			BYTE fTrainerPresent:1;
			BYTE fSquareVRAM:1;
			BYTE fMapperLo:4;
		};
	};
	union
	{
		BYTE	bFlag7;
		struct
		{
			BYTE fVSUnisystem:1;
			BYTE fPlayChoice10:1;	// ROM growed up in 8 kB after chr
			BYTE fHeaderFormat:2;	// 0x02 - this is iNES2.0
			BYTE fMapperHi:4;
		};
	};
	BYTE	bPrgRamSize;

	union
	{
		BYTE	bFlag9;
		struct
		{
			BYTE fTVSystem:1;	// 0 - NTSC, 1 - PAL
			BYTE fReserved:7;
		}; 
	};

	union
	{
		BYTE	bFlag10;
		struct
		{
			BYTE fTVSystemUO:2;
			BYTE fReserved2:2;
			BYTE fPrgRamPresentUO:1;
			BYTE fBusConflicts:1;
			BYTE fReserved3:2;
		};
	};

	BYTE	bReserved[5];
} NES_HEADER, *PNES_HEADER;

#pragma pack(pop)

class CNesColors
{
	HBITMAP			m_hbm[ 64 ];
	const HDC		m_hDCDesktop;
	const HDC		m_hDC;

public:
	CNesColors();
	HBITMAP operator[]( BYTE bColor ) const;
	HBITMAP Bitmap( BYTE bColor ) const;
	~CNesColors();
};

class CPPU;

class CNesFile
{
	NES_HEADER			m_iNesHeader;
	tstring				m_sFilename;
	BYTE				m_uMapper;
	BOOL				m_fFileLoaded;

	std::vector<BYTE>				m_vFile;
	std::vector<BYTE>				m_vTrainer;

	BOOL				ParseFile();

protected:
	std::vector<std::vector<BYTE>>	m_vPRG;
	std::vector<BYTE>				m_vCHR;
	std::vector<NES_TILE>			m_vInternalTileset;

	BOOL				InternalLoadFile(LPCTSTR pszFile);
	BOOL				InternalSaveFile(LPCTSTR pszFile = NULL);
	BOOL				InternalFreeFile();
	VOID				InternalLoadTiles(size_t nOffset, size_t nCount);
	VOID				InternalStorTiles(size_t nOffset, size_t nCount);
	VOID				InternalDrawTile(HDC hDC, INT cx, INT cy, WORD ntile, PBYTE pbPal, BOOL fMirrorX, BOOL fMirrorY);
	BOOL				InternalTilesLoaded();
	VOID				InternalDrawTileset(HDC hDC, INT tx, INT ty);
	VOID				SetHeader(LPCTSTR pszFile, NES_HEADER nHeader);

	std::vector<BYTE> &	AddPrg();

public:
	CNesFile();

	NES_HEADER			GetHeader();
	BOOL				IsFileLoaded();
	BYTE				Mapper();
	operator LPCTSTR();
	operator tstring();

	BOOL				Get_CHR(std::vector<BYTE> & vCHR);
	BOOL				Get_PRG(std::vector< std::vector<BYTE> > & vPRG);
};

class CNesUNROM: public CNesFile
{
	BYTE					m_bSelectedBank;

public:
	CNesUNROM();

	BOOL				LoadFile(LPCTSTR pszFile);
	BOOL				SaveFile(LPCTSTR pszFile = NULL);
	BOOL				FreeFile();

	BOOL				SelectBank(BYTE bBankId);

	template<typename T> T & Data(UINT ref)
	{
		if ( ref + sizeof(T) > 0x10000 || ref < 0x8000 )
			throw std::exception("Invalid pointer operation");

		ref -= 0x8000;

		if ( ref < 0x4000 && ref + sizeof(T) > 0x4000 )
			throw std::exception("Cross bank pointer operation");

		if ( ref < 0x4000 )
			return *((T*) &m_vPRG[ m_bSelectedBank ][ ref ]);
		else
			return *((T*) &m_vPRG[ m_vPRG.size() - 1 ][ ref - 0x4000 ]);
	}
};

class CNesSimpleMMC3: public CNesFile
{
	std::vector<BYTE>		m_vRAM;
	std::vector<BYTE>		m_vPPU;
	std::vector<NES_TILE>	m_vTileset;
	CString					m_strInternalFilename;
	WORD					m_vTiles[0x200];
	// CBitmap					m_bmDrawTileCache;

	BYTE				m_uSelectedBank[2];
	BYTE				m_uSelectedCHR[8];
	VOID				ApplyPRG();
	VOID				RevertPRG();
	VOID				ApplyCHR();
	VOID				LoadTiles();
	VOID				StorTiles();
	BYTE				FindBank(BYTE bankId);
	BOOL				InitMapper();

public:
	CNesSimpleMMC3();
	BOOL				SetExROM();
	BOOL				IsROMExtended();
	BOOL				LoadFile(LPCTSTR pszFile);
	BOOL				SaveFile(LPCTSTR pszFile = NULL);
	BOOL				FreeFile();

	BOOL				SelectPRG(BYTE bBank, BOOL fLast = FALSE);
	BOOL				SelectCHR(BYTE bBank[6]);
	BOOL				SelectCHR(BYTE bRegister, BYTE bBank);

	VOID				DrawTiles(HDC hDC, INT tx, INT ty);
	VOID				DrawTile( CNesBitmap & bm, DWORD x, DWORD y, BYTE tileId, BYTE bAttribute, BOOL fSprite, BOOL fFlipHorz, BOOL fFlipVert );
	VOID				Get_Tiles(std::vector<NES_TILE> & vTiles);
	VOID				Let_Tiles(std::vector<NES_TILE> & vTiles);

	VOID				Protect(WORD ptr, BOOL fProtect);

	VOID				DrawTileset(HDC hDC, int x, int y);
	BOOL				CreateNew(LPCTSTR pszFile, BOOL fMirroring, std::vector< std::vector<BYTE> > & vPRG, std::vector<BYTE> & vCHR);
	BOOL				FlushPPU( CPPU & ppu );
	const CString &		Name() const;

	template<typename T> T & Data(UINT ref)
	{
		if ( ref + sizeof(T) > 0x10000 || ref < 0x8000 )
			throw std::exception("Invalid pointer operation");

		return *((T*) &m_vRAM[ref - 0x8000]);
	}

	NES_TILE &				Tile(UINT id)
	{
		if ( id >= 0x200 )
			throw std::exception("Invalid tile requested");

		return m_vTileset[ m_vTiles[id] ];
	}
};

class CNesNROM: public CNesFile
{
	std::vector<BYTE>		m_vRAM;

public:
	CNesNROM();

	VOID				DrawTile(HDC hDC, INT cx, INT cy, WORD ntile, PBYTE pbPal, BOOL fMirrorX, BOOL fMirrorY);
	VOID				DrawTileset(HDC hDC, INT tx, INT ty);

	BOOL				LoadFile(LPCTSTR pszFile);
	BOOL				SaveFile(LPCTSTR pszFile = NULL);
	BOOL				FreeFile();

	NES_TILE &				Tile(UINT id)
	{
		if ( id >= 0x200 )
			throw std::exception("Invalid tile requested");

		return m_vInternalTileset[ id ];
	}

	template<typename T> T & Data(UINT ref)
	{
		if ( ref + sizeof(T) > 0x10000 || ref < 0x8000 )
			throw std::exception("Invalid pointer operation");

		return *((T*) &m_vRAM[ref - 0x8000]);
	}

};

class CPPU
{
	friend						CNesSimpleMMC3;

	std::vector<BYTE>			m_vPPU;
	BOOL						m_fTilesPage2;
	CBitmap						m_bmTiles;

	VOID						DecodeTiles( std::vector<NES_TILE> & tile );
	VOID						CacheTiles();
	VOID						DrawTile( RGBQUAD * prgb, const PBYTE pbPal, NES_TILE & tile, BYTE nTile, BYTE nAttr );
	VOID						DrawTileGroup( CBitmap & bm, int x, int y, WORD nTiles, WORD nAttr );

public:
	CPPU();

	VOID						TilesPage2( BOOL fTilesPage2 );
	BOOL						TilesPage2();

	template<typename T> T & ROM(int page, WORD ref)
	{
		if ( page < 0 || page > 1 || int(ref) + sizeof(T) > 0x1000 )
			throw std::exception( "PPU ROM page access error" );

		return *( (T*)&m_vPPU[ page * 0x1000 + int( ref ) ] );
	}

	template<typename T> T & Screen(WORD ref)
	{
		if ( ref >= 0x3000 && ref <= 0x3EFF )
			ref -= 0x1000;

		if ( sizeof( T ) > 0x3C0 || ref < 0x2000 || ref >= 0x3000 )
			throw std::exception( "PPU Screen page access error" );

		int page = ( ref - 0x2000 ) / 0x400;
		int bound = 0x2000 + 0x400 * page;
		if ( static_cast<int>( ref ) < bound || static_cast<int>( ref ) + sizeof( T ) > bound + 0x3C0 )
			throw std::exception( "PPU Screen page access error" );
		
		return *( (T*)&m_vPPU[ ref ] );
	}

	template<typename T> T & Attribute( WORD ref )
	{
		if ( ref >= 0x3000 && ref <= 0x3EFF )
			ref -= 0x1000;

		if ( sizeof( T ) >= 0x40 || ref < 0x2000 || ref >= 0x3000 )
			throw std::exception( "PPU attributes page access error" );

		int page = ( ref - 0x2000 ) / 0x400;
		int bound = 0x2000 + 0x400 * page + 0x3C0;
		if ( int( ref ) < bound || int( ref ) + sizeof( T ) > bound + 0x40 )
			throw std::exception( "PPU attributes page access error" );

		return *( (T*)&m_vPPU[ ref ] );
	}

	template<typename T> T & Data( UINT ref )
	{
		if ( ref < 0x2000 || ref + sizeof( T ) > 0x4000 )
			throw std::exception( "Raw PPU access error" );

		return *( (T*)&m_vPPU[ ref ] );
	}

	NES_PPU_PALETTE & Palette()
	{
		return *( (PNES_PPU_PALETTE)&m_vPPU[ 0x3F00 ] );
	}

	NES_PPU_PALETTE & SpritePalette()
	{
		return *( (PNES_PPU_PALETTE)&m_vPPU[ 0x3F10 ] );
	}

	BOOL	DrawPage( int page, CBitmap & bm );
	BOOL	DrawPageWithScroll( int scrollX, int scrollY, CBitmap & bm );
};