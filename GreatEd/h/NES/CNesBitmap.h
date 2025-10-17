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

typedef struct _tagNES_PIXEL
{
	union
	{
		BYTE	bRawColor;
		struct
		{
			BYTE	bColor : 2;
			BYTE	bAttr : 2;
			BYTE	fSprite : 1;
			BYTE	bExtended : 3;
		};
	};

	operator BYTE&( )
	{
		return bRawColor;
	};

	_tagNES_PIXEL() : bRawColor( 0 )
	{
	}
	_tagNES_PIXEL( BYTE bInit ) : bRawColor( bInit )
	{
	};
} NES_PIXEL, *PNES_PIXEL;

class CNesBitmap
{
	struct
	{
		BOOL							fInit;
		std::vector<NES_PIXEL>			vBitmap;
		struct
		{
			size_t						cx;
			size_t						cy;
		} sz;
	} m_bm;

	VOID							UnsafeBlitTile( const NES_TILE & tile, const RECT & rc, BYTE bAttr, BOOL fSprite, PNES_PIXEL pBits, BOOL fFlipHorz, BOOL fFlipVert );

public:
	CNesBitmap();
	VOID							Create( int cx, int cy );
	VOID							Destroy();
	PNES_PIXEL						Bits() const;

	VOID							BlitTile( const NES_TILE & tile, int x, int y, BYTE bAttr );
	VOID							BlitSprite( const NES_TILE & tile, int x, int y, BYTE bAttr, BOOL fFlipHorz, BOOL fFlipVert );
	int								Width() const;
	int								Height() const;
	VOID							Blit( CNesBitmap & bmSource, int x, int y, int width, int height, int src_x, int src_y );
	VOID							BlitRaw( PBYTE pSourceBits, int x, int y, int width, int height, int src_x, int src_y, int source_width, int source_height );
	VOID							RBlitRaw(
		PBYTE pTargetBits,
		int x, int y, int width, int height,
		int tgt_x, int tgt_y, int target_width, int target_height,
		BOOL fFlipHorz = FALSE, BOOL fFlipVert = FALSE
	) const;
};
