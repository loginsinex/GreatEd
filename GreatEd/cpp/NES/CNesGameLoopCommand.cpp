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

CNesGameLoopCommand::CNesGameLoopCommand( CNesSimpleMMC3 & file, CNesPointers & eptr )
	: m_file( file ), m_eptr( eptr )
{

}

VOID CNesGameLoopCommand::FlushList()
{
	m_lpList.clear();
}

VOID CNesGameLoopCommand::LoadLoopList()
{
	const NES_LEVEL_TYPE lvlType[] = { nltWater, nltGround, nltUnderground, nltCastle };
	const USHORT uLoopPointers = m_eptr[ eSharedLoopCommands ].ptr;

	FlushList();
	m_file.SelectPRG( m_eptr[ eSharedLoopCommands ].bLoBank, TRUE );

	for ( const auto & type : lvlType )
	{
		NES_AREA_PTR aptr = { 0 };
		aptr.bType = type;
		for ( BYTE x = 0; x < 32; ++x, aptr.bAreaId++ )
		{
			USHORT uAreaLoopsPtr = m_file.Data<USHORT>( uLoopPointers + sizeof( USHORT ) * static_cast<USHORT>( aptr.bPtr ) );
			if ( uAreaLoopsPtr )
			{
				std::vector<NES_LOOP_COMMAND> & vlp = m_lpList[ type ][ x ];
				while ( 0xFF != m_file.Data<BYTE>( uAreaLoopsPtr ) )
				{
					vlp.push_back( m_file.Data<NES_LOOP_COMMAND>( uAreaLoopsPtr ) );
					uAreaLoopsPtr += sizeof( NES_LOOP_COMMAND );
					if ( uAreaLoopsPtr >= uLoopPointers + 0x2000 )
					{
						throw std::exception( "Too many loops commands." );
					}
				}
			}
		}
	}

	return;
}

VOID CNesGameLoopCommand::DumpLoopList()
{
	const NES_LEVEL_TYPE lvlType[] = { nltWater, nltGround, nltUnderground, nltCastle };
	const USHORT uLoopPointers = m_eptr[ eSharedLoopCommands ].ptr;
	USHORT uLoopData = uLoopPointers + 0x80 * sizeof( USHORT );

	m_file.SelectPRG( m_eptr[ eSharedLoopCommands ].bLoBank, TRUE );

	// clear pointers
	for ( USHORT i = 0; i < 0x80; ++i )
	{
		m_file.Data<USHORT>( uLoopPointers + sizeof( USHORT ) * i ) = 0;
	}

	// dump data and pointers
	for ( const auto & lpType : m_lpList )
	{
		NES_AREA_PTR aptr = { 0 };
		aptr.bType = lpType.first;

		for ( const auto & lpArea : lpType.second )
		{
			aptr.bAreaId = lpArea.first;
			if ( lpArea.second.size() > 0 )
			{
				m_file.Data<USHORT>( uLoopPointers + sizeof( USHORT ) * static_cast<USHORT>( aptr.bPtr ) ) = uLoopData;
				for ( const auto & lp : lpArea.second )
				{
					m_file.Data<NES_LOOP_COMMAND>( uLoopData ) = lp;
					uLoopData += sizeof( NES_LOOP_COMMAND );
				}

				m_file.Data<BYTE>( uLoopData++ ) = 0xFF;
			}
		}
	}
}

BOOL CNesGameLoopCommand::LoopList( CNesLevel * pLevel, std::vector<NES_LOOP_COMMAND>& vlp )
{
	NES_AREA_PTR aptr = pLevel->AreaPtr();
	auto it = m_lpList.find( aptr.bType );
	if ( it != m_lpList.end() )
	{
		auto itArea = it->second.find( aptr.bAreaId );
		if ( itArea != it->second.end() )
		{
			vlp = itArea->second;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CNesGameLoopCommand::SetLoopList( CNesLevel * pLevel, const std::vector<NES_LOOP_COMMAND>& vlp )
{
	NES_AREA_PTR aptr = pLevel->AreaPtr();
	std::vector<NES_LOOP_COMMAND> lvlp = vlp;
	std::sort( lvlp.begin(), lvlp.end(),
		[]( const NES_LOOP_COMMAND & nlp1, const NES_LOOP_COMMAND & nlp2 ) { return nlp1.bPage < nlp2.bPage; } );

	m_lpList[ aptr.bType ][ aptr.bAreaId ] = lvlp;
	return TRUE;
}