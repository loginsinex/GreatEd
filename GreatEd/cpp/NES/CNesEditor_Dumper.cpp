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

USHORT CNesEditor::WriteROMData( USHORT uPtr, std::vector<BYTE>& vData )
{
	for ( const auto & b : vData )
	{
		m_file.Data<BYTE>( uPtr++ ) = b;
	}

	return uPtr;
}

VOID CNesEditor::DumpLevels()
{
	m_loop.FlushList();
	ValidateLevelPointers( m_mLevel );
	DumpWorlds( m_mWorld, m_mHalfPages );
	DumpAreas( m_mLevel );
	m_title.DumpData();
	m_hack.DumpData();
	m_player.DumpData();
	m_loop.DumpLoopList();
	m_demo.DumpData();
	m_mTiles.Dump();
	m_objData.DumpData();
}

VOID CNesEditor::ValidateLevelPointers( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>>& mAreas )
{
	for ( auto & lvlType : mAreas )
	{
		NES_AREA_PTR aptr = { 0 };
		aptr.bType = lvlType.first;
		aptr.bAreaId = 0;
		for ( auto & pLevel : lvlType.second )
		{
			pLevel->UpdateAreaType( lvlType.first );
			pLevel->SetAreaId( aptr.bAreaId );
			aptr.bAreaId++;
		}
	}
}

VOID CNesEditor::DumpAreas( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>>& mAreas )
{
	BYTE bEnemyHOffset[ 4 ] = { 0 };
	BYTE bAreaHOffset[ 4 ] = { 0 };
	USHORT uSizeOfData = 0;

	// fetch total levels counter
	for ( auto & type : mAreas )
	{
		uSizeOfData += USHORT( type.second.size() );
	}

	// calculate offsets of pointers
	USHORT uEnemyLoPtr = m_eptr.Pointer( eEnemyPtrLo );
	USHORT uEnemyHiPtr = uEnemyLoPtr + uSizeOfData;
	USHORT uLevelLoPtr = uEnemyHiPtr + uSizeOfData;
	USHORT uLevelHiPtr = uLevelLoPtr + uSizeOfData;
	BYTE bEnemyOffset = 0, bAreaOffset = 0, bOffset = 0;
	std::map<NES_AREA_PTR, BYTE> mBank;
	const USHORT uBeginPtr = m_eptr[ eSharedEnemyPtrLo ].ptr;
	const USHORT uEOFPtr = uBeginPtr + 0x2000;
	const USHORT uBankArrayPtr = ( ( uLevelHiPtr + 0x1000 ) & 0xF000 ) - 128;
	USHORT uCurrentPtr = uBeginPtr;
	BYTE bDataBank = m_eptr[ eSharedEnemyPtrLo ].bLoBank;

	// write datas pointers
	m_eptr.Pointer( eEnemyPtrLo ) = uEnemyLoPtr;
	m_eptr.Pointer( eEnemyPtrHi ) = uEnemyHiPtr;
	m_eptr.Pointer( eLevelPtrLo ) = uLevelLoPtr;
	m_eptr.Pointer( eLevelPtrHi ) = uLevelHiPtr;

	m_file.SelectPRG( m_eptr[ eSharedEnemyPtrLo ].bHiBank, FALSE );
	m_file.SelectPRG( bDataBank, TRUE );

	for ( auto & lvlType : mAreas )
	{
		bEnemyHOffset[ bOffset ] = bEnemyOffset;
		bAreaHOffset[ bOffset ] = bAreaOffset;
		_tprintf( TEXT( "Area/Enemy offsets: %d/%d\n" ), bAreaOffset, bEnemyOffset );
		bEnemyOffset += LOBYTE( lvlType.second.size() );
		bAreaOffset += LOBYTE( lvlType.second.size() );
		bOffset++;

		for ( auto & pLevel : lvlType.second )
		{			
			const NES_AREA_PTR aptr = pLevel->AreaPtr();
			std::vector<NES_LOOP_COMMAND> vnlp;
			std::vector<BYTE> vEnemiesDump, vItemsDump;
			USHORT uDumpSize = 0;

			pLevel->GetLevelBinaryData( vItemsDump, vEnemiesDump );
			if ( pLevel->DumpLoops( vnlp ) )
			{
				m_loop.SetLoopList( pLevel, vnlp );
			}

			uDumpSize = LOWORD( vItemsDump.size() + vEnemiesDump.size() );

			if ( vItemsDump.size() > 256 || vEnemiesDump.size() > 256 )
			{
				char sException[ 256 ];
				sprintf_s( sException, "Dump for area 0x%2X does not fit to 256 bytes size", static_cast<BYTE>( aptr.bPtr ));
				throw std::exception( sException );
			}

			pLevel->Meta( TEXT( "BANK" ), bDataBank );

			if ( uCurrentPtr + uDumpSize < uBeginPtr || uCurrentPtr + uDumpSize >= uEOFPtr )
			{
				uCurrentPtr = uBeginPtr;
				bDataBank++;
				m_file.SelectPRG( bDataBank, TRUE );
			}
			
			_tprintf( TEXT( "[%d] Writing enemy pointer [0x%02X] 0x%04X at [0x%04x]:[0x%04x]\n" ), bDataBank, pLevel->AreaPtr().bPtr, uCurrentPtr, uEnemyHiPtr, uEnemyLoPtr );
			m_file.Data<BYTE>( uEnemyLoPtr++ ) = LOBYTE( uCurrentPtr );
			m_file.Data<BYTE>( uEnemyHiPtr++ ) = HIBYTE( uCurrentPtr );
			uCurrentPtr = WriteROMData( uCurrentPtr, vEnemiesDump );
			_tprintf( TEXT( "[%d] Writing items pointer [0x%02X] 0x%04X at [0x%04x]:[0x%04x]\n" ), bDataBank, pLevel->AreaPtr().bPtr, uCurrentPtr, uLevelHiPtr, uLevelLoPtr );
			m_file.Data<BYTE>( uLevelLoPtr++ ) = LOBYTE( uCurrentPtr );
			m_file.Data<BYTE>( uLevelHiPtr++ ) = HIBYTE( uCurrentPtr );
			uCurrentPtr = WriteROMData( uCurrentPtr, vItemsDump );

			if (
				uEnemyLoPtr >= uBankArrayPtr ||
				uEnemyHiPtr >= uBankArrayPtr ||
				uLevelLoPtr >= uBankArrayPtr ||
				uLevelHiPtr >= uBankArrayPtr 
				)
			{
				// check overflow for area pointers
				throw std::exception( "Not enough space for areas pointers" );
			}

//			if ( uLevelLoPtr > 0x9d18 )
	//			m_file.Protect( 0x9d18, true );
			/*
			CString sf = TEXT( "d:\\STUFF\\dev\\src\\Super_Mario_Bros\\Levels\\" ), s;
			DWORD w;
			s.Format( TEXT( "%s%02x_area.bin" ), sf.GetString(), aptr.bPtr );
			HANDLE f1 = CreateFile( s, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr );
			WriteFile( f1, vItemsDump.data(), LODWORD( vItemsDump.size() ), &w, nullptr );
			CloseHandle( f1 );
			s.Format( TEXT( "%s%02x_enemy.bin" ), sf.GetString(), aptr.bPtr );
			f1 = CreateFile( s, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr );
			WriteFile( f1, vEnemiesDump.data(), LODWORD( vEnemiesDump.size() ), &w, nullptr );
			CloseHandle( f1 );
			*/

			mBank[ aptr ] = bDataBank;
		}
	}

	// dump offsets and banks
	CopyMemory( m_file.Data<BYTE[ 4 ]>( m_eptr.Pointer( eEnemyAddrHOffset ) ), bEnemyHOffset, _ARRAYSIZE( bEnemyHOffset ) );
	CopyMemory( m_file.Data<BYTE[ 4 ]>( m_eptr.Pointer( eLevelAddrHOffset ) ), bAreaHOffset, _ARRAYSIZE( bAreaHOffset ) );

	for ( const auto & bank : mBank )
	{
		m_file.Data<BYTE>( uBankArrayPtr + bank.first.bPtr ) = bank.second;
	}
}

VOID CNesEditor::DumpWorlds( const NES_GAME_AREA_LIST & mWorlds, const NES_GAME_AREA_HP_LIST & mHalfPages )
{
	BYTE bWorldsOffset[ NES_WORLDS_COUNT ] = { 0 };
	USHORT uWorldsPtr = m_eptr.Pointer( eWorldAreas );
	BYTE bOffset = 0;
	BYTE bWorld = 0;

	for ( const auto & world : mWorlds )
	{
		bWorldsOffset[ bWorld++ ] = bOffset;

		for ( const auto & areas : world )
		{
			for ( auto pLevel : areas )
			{
				m_file.Data<BYTE>( uWorldsPtr++ ) = pLevel->AreaPtr().bPtr;
				bOffset++;
			}
		}
	}

	// dump offsets
	CopyMemory( m_file.Data<BYTE[ NES_WORLDS_COUNT ]>( m_eptr.Pointer( eWorldAreasOffsets ) ), bWorldsOffset, NES_WORLDS_COUNT );
	
	// write new pointers for items/levels offsets and qbricks pointers
	// also new place for areas pointers
	m_eptr.Pointer( eEnemyAddrHOffset ) = uWorldsPtr; uWorldsPtr += NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET;
	m_eptr.Pointer( eLevelAddrHOffset ) = uWorldsPtr;  uWorldsPtr += NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET;

	// write qbricks metatiles to the new place
	m_file.Data<NES_QBLOCK_METATILES>( uWorldsPtr ) = m_objData.QBlockMetatiles();
	m_eptr.Pointer( eBrickQMetatiles ) = uWorldsPtr; uWorldsPtr += NES_LENGTH_OF_QBRICKS_METATILES;

	// set place only for low enemy pointer, because next pointers will be calculated at DumpAreas
	m_eptr.Pointer( eEnemyPtrLo ) = uWorldsPtr;

	// dump half pages
	DumpHalfPages( mHalfPages );
}

VOID CNesEditor::DumpHalfPages( const NES_GAME_AREA_HP_LIST & mHalfPages )
{
	USHORT uHalfPagesOffsetPtr = m_eptr[ eSharedHalfPages ].ptr;
	USHORT uHalfPagesPtr = uHalfPagesOffsetPtr + NES_WORLDS_COUNT;
	m_file.SelectPRG( m_eptr[ eSharedHalfPages ].bLoBank, TRUE );
	BYTE bWorldsCount = 0, bOffset = 0;
	BYTE bWorldOffset[ NES_WORLDS_COUNT ] = { 0 };

	for ( const auto & hp : mHalfPages )
	{
		const BYTE bWorld = bWorldsCount++;
		bWorldOffset[ bWorld ] = bOffset;

		for ( const auto & b : hp )
		{
			m_file.Data<BYTE>( uHalfPagesPtr++ ) = b;
			bOffset++;
		}
	}

	CopyMemory( m_file.Data <BYTE[ NES_WORLDS_COUNT ]>( uHalfPagesOffsetPtr ), bWorldOffset, NES_WORLDS_COUNT );
}