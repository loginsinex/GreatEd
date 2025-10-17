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

#define		NES_LEVEL_VIEW_RESERVED_HEIGHT				0

typedef std::vector<std::vector<NES_MAP_METATILE>>		NES_MAP;
typedef std::vector<std::vector<BOOL>>					NES_MAP_BEHIND;
typedef std::vector<std::vector<NES_SPRITE>>			NES_SPRITE_MAP;
typedef std::map<INT, std::map<INT, BYTE>>				NES_RAW_TILE_MAP;


class CNesLevelView
{
	int										m_uReservedSpace;
	CNesMetatiles							& m_mTiles;
	CPaletteBitmap							& m_bm;
	NES_MAP									m_nmap;
	NES_SPRITE_MAP							m_nsprmap;
	NES_RAW_TILE_MAP						m_mTileMap;
	NES_RAW_TILE_MAP						m_mAttrMap;
	BOOL									m_fBehind;
	BOOL									m_fRenderTitle;

	std::vector<int>						m_vGroundBounds;

	struct
	{
		size_t								cx;
		size_t								cy;
	} m_szMap;

	VOID									DrawPageBorders( HDC dc );
	VOID									RenderTiles();

public:
	CNesLevelView( CPaletteBitmap & bm, CNesMetatiles & mtiles, BOOL fDrawTitle = FALSE );

	operator HDC();
	CPaletteBitmap				& Bitmap();

	VOID									ResetBounds();
	VOID									SetMapSize( size_t cx, size_t cy );
	VOID									RenderMap();
	VOID									DrawMetatile( INT x, INT y, const NES_METATILE & uMTileId );
	VOID									DrawSprite( INT x, INT y, const NES_SPRITE & uMTileId );
	VOID									DrawTile( INT x, INT y, BYTE tile );
	VOID									DrawAttr( INT x, INT y, BYTE attr );
	NES_METATILE							GetMetatile( INT x, INT y );
	VOID									SelectBg( BYTE bBgCol );
	VOID									SelectPalette( NES_LEVEL_TYPE nlt, NES_AREA_STYLE nas, NES_BG_COLOR_CTL bgModifier );
	VOID									AddBound( int x );
	VOID									EraseSpriteMap();
	VOID									EraseAll();
	BYTE									PagesCount();
	int										ReservedSpace();
	int										ReservedLogicalSpace();
	VOID									UpdateReservedLogicalSpace( int uReservedLogicalSpace );
	VOID									SetBehindFlag( BOOL fBehind );
	BOOL									IsViewBehindMode();
	BOOL									SetTitleVisible( BOOL fVisible );
};
