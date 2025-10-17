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

#include "h/NES/NES.h"

CNesTitleScreenObject::CNesTitleScreenObject( const NES_TITLE_SCREEN_OBJ & obj, const std::vector<BYTE>& data )
	: m_fRepeat( FALSE ), m_fVertical( FALSE ), m_uCounter( 0 ), m_type( ntsoTilesLine )
{
	ParseObject( obj, data );
}

VOID CNesTitleScreenObject::ParseObject( const NES_TITLE_SCREEN_OBJ & obj, const std::vector<BYTE>& data )
{
	// retreive a type of object

	if ( obj.uPPUPtr < 0x2000 || obj.uPPUPtr >= 0x3000 && obj.uPPUPtr < 0x3F00 || obj.uPPUPtr >= 0x3F20 )
	{
		throw std::exception( "Title screen data have incorrect pointer data" );
	}

	m_fVertical = obj.fVert;
	m_fRepeat = obj.fRepeat;
	m_type = ( ( ( obj.uPPUPtr & 0x3C0 ) == 0x3C0 ) ? ntsoAttrLine : ntsoTilesLine );
	if ( obj.uPPUPtr >= 0x3F00 && obj.uPPUPtr <= 0x3F1F ) m_type = ntsoPaletteLine;

	_tprintf( TEXT( "PTR: 0x%04X, REP: %s, VERT: %s, counter: %d\n" ), obj.uPPUPtr, ( obj.fRepeat ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ), ( obj.fVert ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ), obj.counter );

	switch ( m_type )
	{
		case ntsoTilesLine:
			{
				USHORT uPage = ( ( obj.uPPUPtr & 0x2C00 ) - 0x2000 ) / 0x400;
				USHORT uOffset = ( obj.uPPUPtr - ( obj.uPPUPtr & 0x2C00 ) ) & 0x3ff;
				m_pt.x = uOffset % 32; m_pt.y = uOffset / 32;
				if ( uPage % 2 ) m_pt.x += 32 * ( uPage % 2 );
				if ( uPage / 2 )  m_pt.y += 32 * ( uPage / 2 );
				break;
			}
		case ntsoAttrLine:
			{
				USHORT uPage = ( ( obj.uPPUPtr & 0x2C00 ) - 0x2000 ) / 0x400;
				USHORT uOffset = ( obj.uPPUPtr - ( obj.uPPUPtr & 0x2C00 ) - 0x3C0 );
				m_pt.x = uOffset % 8; m_pt.y = uOffset / 8;
				if ( uPage % 2 ) m_pt.x += 8 * ( uPage % 2 );
				if ( uPage / 2 )  m_pt.y += 8 * ( uPage / 2 );
				break;
			}
		case ntsoPaletteLine:
			{
				m_pt.x = obj.uPPUPtr % 0x10;
				m_pt.y = obj.uPPUPtr / 0x10;
				break;
			}
	}

	if ( m_fRepeat && data.size() >= 1 )
	{
		m_data.insert( m_data.begin(), obj.counter, data[ 0 ] );
	}
	else if ( !m_fRepeat && data.size() >= obj.counter )
	{
		m_data.insert( m_data.begin(), data.begin(), data.begin() + obj.counter );
	}
}

BOOL CNesTitleScreenObject::Split( std::vector<CNesTitleScreenObject>& obj ) const
{
	// will split to objects with repeating
	return 0;
}

VOID CNesTitleScreenObject::DrawObject( CNesLevelView & view ) const
{
	POINT pt = m_pt;
	POINT pt0 = { 32 * ( pt.x / 32 ), 32 * ( pt.y / 32 ) };
	POINT pt1 = { pt0.x + 32, pt0.y + 32 };
	POINT apt0 = { 8 * ( pt.x / 8 ), 8 * ( pt.y / 8 ) };
	POINT apt1 = { apt0.x + 8, apt0.y + 8 };

	switch ( m_type )
	{
		case ntsoTilesLine:
			{
				for ( const auto & tile : m_data )
				{
					view.DrawTile( pt.x % 64, pt.y % 32, tile );
					if ( m_fVertical ) pt.y++;
					else pt.x++;

					if ( pt.x >= pt1.x )
					{
						pt.x = pt0.x;
						pt.y++;
					}

					if ( pt.y >= pt1.y ) pt.y = pt0.y;

				}
				break;
			}
		case ntsoAttrLine:
			{
				for ( const auto & attr : m_data )
				{
					view.DrawAttr( pt.x % 16, pt.y % 8, attr );
					pt.x++;
					if ( pt.x >= apt1.x )
					{
						pt.x = apt0.x;
						pt.y++;
					}
				}
				break;
			}
	}
}

// Title screen implementation

CNesTitleScreen::CNesTitleScreen( CNesSimpleMMC3 & file, CNesPointers & eptr )
	: m_file( file ), m_eptr( eptr )
{

}

CNesTitleScreen::~CNesTitleScreen()
{

}

VOID CNesTitleScreen::LoadData()
{
	// we have to load data as raw bits from CHR and get size from difference from ram ptr start and an end
	std::vector<BYTE> chr;
	m_eptr.Select( eTitleScreenCHRPtrHi );
	USHORT uCHRPtr = MAKEWORD( m_file.Data<BYTE>( m_eptr[ eTitleScreenCHRPtrLo ].ptr ), m_file.Data<BYTE>( m_eptr[ eTitleScreenCHRPtrHi ].ptr ) );
	USHORT uStartPTr = MAKEWORD( m_file.Data<BYTE>( m_eptr[ eTitleScreenStartPtrLo ].ptr ), m_file.Data<BYTE>( m_eptr[ eTitleScreenStartPtrHi ].ptr ) );
	USHORT uEndPTr = MAKEWORD( m_file.Data<BYTE>( m_eptr[ eTitleScreenEndPtrLo ].ptr ), m_file.Data<BYTE>( m_eptr[ eTitleScreenEndPtrHi ].ptr ) );

	m_file.Get_CHR( chr );
	std::vector<BYTE> title( chr.begin() + uCHRPtr, chr.begin() + uCHRPtr + ( uEndPTr - uStartPTr + 1 ) );

	// now parse it
	BYTE state = 0, uCounter = 0; // 0 - hi ppu ptr, 1 - lo ppu ptr, 2 - counter, 3 - data...
	NES_TITLE_SCREEN_OBJ obj;
	std::vector<BYTE> data;
	data.reserve( 32 );
	for ( const auto & c : title )
	{
		switch ( state )
		{
			case 0:
				{
					data.clear();
					obj.behavior = 0;
					obj.counter = 0;
					obj.fRepeat = FALSE;
					obj.fVert = FALSE;
					obj.uPPUPtr = ( c << 8 );
					if ( c < 0x20 || 0xFF == c ) state = 4;
					state++;
					break;
				}
			case 1:
				{
					obj.uPPUPtr |= c;
					state++;
					break;
				}
			case 2:
				{
					obj.counter = c & 0x3f;
					if ( c & 0x80 ) obj.fVert = TRUE;
					if ( c & 0x40 ) obj.fRepeat = TRUE;
					uCounter = obj.counter;
					state++;
					break;
				}
			case 3:
				{
					data.push_back( c );
					if ( obj.fRepeat )
					{
						m_obj.emplace_back( obj, data );
						state = 0;
					}
					else
					{
						if ( uCounter > 1 ) uCounter--;
						else
						{
							m_obj.emplace_back( obj, data );
							state = 0;
						}
					}
					break;
				}
		}
	}

	return;
}

VOID CNesTitleScreen::DumpData()
{
	return VOID();
}

VOID CNesTitleScreen::DrawTitleScreen( CNesLevelView & view )
{
	for ( const auto & obj : m_obj )
	{
		obj.DrawObject( view );
	}
}