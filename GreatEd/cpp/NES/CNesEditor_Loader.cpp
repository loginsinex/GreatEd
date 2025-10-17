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

VOID CNesEditor::ReadROMData( USHORT uPtr, std::vector<BYTE>& vData, BYTE bMarker, BOOL fObjectMarker )
{
	int uSize = 0;
	BOOL fLongObject = 0;
	while ( true )
	{
		BYTE bData = m_file.Data<BYTE>( uPtr++ );

		if ( !fObjectMarker )
		{
			if ( uSize >= 2 && ( !( uSize % 2 ) && bData == bMarker ) )
			{
				break;
			}
		}
		else
		{
			if ( !uSize && bData == bMarker )
			{
				break;
			}

			if ( 0 == uSize )
			{
				if ( NES_SPECIAL_OBJECT_E == ( bData & 0x0F )  )
				{
					fLongObject = TRUE;
				}
				else
				{
					fLongObject = FALSE;
				}
			}

			if ( fLongObject )
			{
				if ( uSize >= 2 )
				{
					uSize = -1;
				}
			}
			else
			{
				if ( uSize >= 1 )
				{
					uSize = -1;
				}
			}
		}

		vData.push_back( bData );
		uSize++;
	}
}

VOID CNesEditor::LoadAreas( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>>& mAreas )
{
	BYTE bEnemyHOffset[ 4 ];
	BYTE bAreaHOffset[ 4 ];
	USHORT uEnemyLoPtr = m_eptr.Pointer( eEnemyPtrLo );
	USHORT uEnemyHiPtr = m_eptr.Pointer( eEnemyPtrHi );
	USHORT uLevelLoPtr = m_eptr.Pointer( eLevelPtrLo );
	USHORT uLevelHiPtr = m_eptr.Pointer( eLevelPtrHi );
	USHORT uEnemiesCount = uEnemyHiPtr - uEnemyLoPtr;
	USHORT uLevelsCount = uLevelHiPtr - uLevelLoPtr;
	std::vector<WORD> vEnemyType( uEnemiesCount, 0xffff ), vAreaType( uLevelsCount, 0xffff );
	std::map<NES_LEVEL_TYPE, std::vector<USHORT>> mEnemies, mLevels;
	std::vector<CNesLevel*> vLoadedLevels;	// this array requires to fast get all loaded levels, when exceptions occured while loading another levels
	std::map<NES_AREA_PTR, std::vector<NES_LINK>> mLinks;

	if ( uEnemiesCount != uLevelsCount )
	{
		throw std::exception( "Areas does not matched" );
	}

	CopyMemory( bEnemyHOffset, m_file.Data<BYTE[ 4 ]>( m_eptr.Pointer( eEnemyAddrHOffset ) ), _ARRAYSIZE( bEnemyHOffset ) );
	CopyMemory( bAreaHOffset, m_file.Data<BYTE[ 4 ]>( m_eptr.Pointer( eLevelAddrHOffset ) ), _ARRAYSIZE( bAreaHOffset ) );

	// fill the vectors by types
	for ( USHORT i = 0; i < _ARRAYSIZE( bAreaHOffset ); ++i )
	{
		vEnemyType[ bEnemyHOffset[ i ] ] = i;
		vAreaType[ bAreaHOffset[ i ] ] = i;
		_tprintf( TEXT( "Enemy/Area offset: %d/%d\n" ), bEnemyHOffset[ i ], bAreaHOffset[ i ] );
	}

	for ( USHORT i = 0; i < _ARRAYSIZE( bAreaHOffset ); ++i )
	{
		for ( size_t k = bEnemyHOffset[ i ]; k < uEnemiesCount; ++k )
		{
			if ( vEnemyType[ k ] != 0xffff && vEnemyType[ k ] != i )
			{
				break;
			}
			vEnemyType[ k ] = i;
		}

		for ( size_t k = bAreaHOffset[ i ]; k < uLevelsCount; ++k )
		{
			if ( vAreaType[ k ] != 0xffff && vAreaType[ k ] != i )
			{
				break;
			}

			vAreaType[ k ] = i;
		}
	}

	// check arrays that its filled correctly
	if ( std::find( vEnemyType.begin(), vEnemyType.end(), 0xffff ) != vEnemyType.end() ||
		std::find( vAreaType.begin(), vAreaType.end(), 0xffff ) != vAreaType.end() )
	{
		throw std::exception( "Areas types are inconsistent" );
	}

	// now load pointers
	for ( USHORT i = 0; i < uLevelsCount; ++i )
	{
		mEnemies[ static_cast<NES_LEVEL_TYPE>( vEnemyType[ i ] ) ].push_back( MAKEWORD( m_file.Data<BYTE>( uEnemyLoPtr + i ), m_file.Data<BYTE>( uEnemyHiPtr + i ) ) );
		mLevels[ static_cast<NES_LEVEL_TYPE>( vAreaType[ i ] ) ].push_back( MAKEWORD( m_file.Data<BYTE>( uLevelLoPtr + i ), m_file.Data<BYTE>( uLevelHiPtr + i ) ) );
		_tprintf( TEXT( "Got enemy pointer [0x%04X]:[0x%04X]: 0x%04X\n" ), uEnemyHiPtr, uEnemyLoPtr, mEnemies[ static_cast<NES_LEVEL_TYPE>( vEnemyType[ i ] ) ].back() );
		_tprintf( TEXT( "Got items pointer [0x%04X]:[0x%04X]: 0x%04X\n" ), uLevelHiPtr, uLevelLoPtr, mLevels[ static_cast<NES_LEVEL_TYPE>( vAreaType[ i ] ) ].back() );
	}

	// calculate LevelBanksPtr
	USHORT uLevelBanksPtr = ( ( uLevelHiPtr + 0x1000 ) & 0xF000 ) - 128;

	// so, we're ready to load levels
	for ( USHORT type = 0; type < 4; ++type )
	{
		auto itEnemies = mEnemies.find( static_cast<NES_LEVEL_TYPE>( type ) );
		auto itLevels = mLevels.find( static_cast<NES_LEVEL_TYPE>( type ) );
		if ( itEnemies == mEnemies.end() || itLevels == mLevels.end() )
		{
			continue;
		}

		size_t cEnemies = itEnemies->second.size(), cLevels = itLevels->second.size();

		// levels and enemies shall to be matched in each group
		// also maximum levels in each groups can't be more than 32
		if ( cEnemies != cLevels || cLevels > 32 )
		{
			throw std::exception( "Area types are not matched" );
		}

		for ( size_t i = 0; i < cLevels; ++i )
		{
			// calculate area ptr (see disasm) to fetch level bank
			NES_AREA_PTR uAreaPtr = { ( static_cast<BYTE>( LOBYTE( type ) ) << 5 ) | static_cast<BYTE>( LOBYTE( i ) ) };
			BYTE bBankId = m_file.Data<BYTE>( uLevelBanksPtr + uAreaPtr.bPtr );
			USHORT uEnemyPtr = itEnemies->second[ i ], uLevelPtr = itLevels->second[ i ];
			std::vector<BYTE> vEnemyData, vLevelData;
			m_file.SelectPRG( bBankId, TRUE );
			_tprintf( TEXT( "Got bank [%d], Enemy pointer 0x%04X, Items pointer 0x%04X\n" ), bBankId, uEnemyPtr, uLevelPtr );

			ReadROMData( uEnemyPtr, vEnemyData, 0xFF, TRUE );
			ReadROMData( uLevelPtr, vLevelData, 0xFD, FALSE );

			CNesLevel * pLevel = new CNesLevel( m_file, uAreaPtr, m_eptr, m_mTiles, m_objData, m_loop, m_gLinks, m_title, m_player );
			pLevel->Meta( TEXT( "BANK" ), bBankId );

			try
			{
				pLevel->LoadLevelData( vLevelData, vEnemyData, mLinks[ uAreaPtr ] );
#ifdef _DEBUG0
				CString sFileDat, sFileEnm;
				sFileDat.Format( TEXT( "Area_%d_%Iu.dat" ), type, i );
				sFileEnm.Format( TEXT( "Enemy_%d_%Iu.dat" ), type, i );
				HANDLE hFile[ 2 ] =
				{
					CreateFile( sFileDat, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr ),
					CreateFile( sFileEnm, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr )
				};

				if ( hFile[ 0 ] != INVALID_HANDLE_VALUE && hFile[ 1 ] != INVALID_HANDLE_VALUE )
				{
					DWORD w = 0;
					WriteFile( hFile[ 0 ], vLevelData.data(), LODWORD( vLevelData.size() ), &w, nullptr );
					WriteFile( hFile[ 1 ], vEnemyData.data(), LODWORD( vEnemyData.size() ), &w, nullptr );
					CloseHandle( hFile[ 1 ] );
					CloseHandle( hFile[ 0 ] );
				}
				else
				{
					if ( INVALID_HANDLE_VALUE != hFile[ 0 ] ) CloseHandle( hFile[ 0 ] );
					if ( INVALID_HANDLE_VALUE != hFile[ 1 ] ) CloseHandle( hFile[ 1 ] );
				}
#endif
				mAreas[ static_cast<NES_LEVEL_TYPE>( type ) ].push_back( pLevel );
				vLoadedLevels.push_back( pLevel );
			}
			catch ( std::exception & e )
			{
				delete pLevel;
				mAreas.clear();
				// also delete all previously loaded levels
				while ( vLoadedLevels.size() > 0 )
				{
					delete vLoadedLevels.back();
					vLoadedLevels.pop_back();
				}

				// rethrow exception to pass it to GUI
				throw e;
			}
		}
	}

	ParseLinks( mLinks );
}

VOID CNesEditor::ParseLinks( std::map<NES_AREA_PTR, std::vector<NES_LINK>>& mLink )
{
	for ( auto & lnk : mLink )
	{
		CNesLevel * pLevel = LevelFromPtr( lnk.first );
		if ( pLevel )
		{
			auto t = pLevel->AreaPtr();
//			if ( t.bAreaId == 5 && t.bType == nltCastle ) __debugbreak();

			std::vector<NES_LINK> & vLink = lnk.second;
			std::sort( vLink.begin(), vLink.end(),
				[]( const auto & it1, const auto & it2 )
			{
				return it1.ptLink.x < it2.ptLink.x;
			} );

			for ( const auto & l : vLink )
			{
				CNesLevel * pTargetLevel = LevelFromPtr( l.aptrTarget );
				if ( pTargetLevel )
				{
					CNesLink * pLink = m_gLinks.AddLink( l.uSourceWorld, pLevel );
					pLink->SetTarget( pTargetLevel, l.uTargetPage );

					if ( !pLevel->AssignLink( pLink, l.ptLink.x ) )
					{
						if ( !pLevel->AssignGroundLink( pLink ) )
						{
							m_gLinks.RemoveLink( pLink );
							_tprintf( TEXT( "WARNING: Removed link for area 0x%0X (point %d). Target is: 0x%0X\n" ),
								lnk.first.bPtr, l.ptLink.x, l.aptrTarget.bPtr );
						}
					}
				}
			}
		}
	}

	return;
}

VOID CNesEditor::LoadWorlds( NES_GAME_AREA_LIST & mWorlds, NES_GAME_AREA_HP_LIST & mHalfPages )
{
	BYTE bWorldOffset[ NES_WORLDS_COUNT ];
	USHORT uWorldsPtr = m_eptr.Pointer( eWorldAreas );
	USHORT uAreasCount = m_eptr.Pointer( eEnemyAddrHOffset ) - uWorldsPtr;

	std::vector<WORD> vWorldOffset( uAreasCount, 0xffff );
	std::map<size_t, std::vector<NES_AREA_PTR>> mWorldMap;

	USHORT uWorld = 0xffff;

	// fetch worlds offsets
	CopyMemory( bWorldOffset, m_file.Data<BYTE[ NES_WORLDS_COUNT ]>( m_eptr.Pointer( eWorldAreasOffsets ) ), NES_WORLDS_COUNT );

	// check worlds data consistency
	for ( size_t i = 0; i < NES_WORLDS_COUNT; ++i )
	{
		if ( bWorldOffset[ i ] >= vWorldOffset.size() )
		{
			throw std::exception( "Load worlds failed" );
		}

		vWorldOffset[ bWorldOffset[ i ] ] = LOBYTE( i );
	}

	// fill array with worlds ids
	uWorld = vWorldOffset[ 0 ];
	for ( size_t i = 1; i < vWorldOffset.size(); ++i )
	{
		if ( vWorldOffset[ i ] == 0xffff )
		{
			vWorldOffset[ i ] = uWorld;
		}
		else
		{
			uWorld = vWorldOffset[ i ];
		}
	}

	if ( std::find( vWorldOffset.begin(), vWorldOffset.end(), 0xffff ) != vWorldOffset.end() )
	{
		throw std::exception( "Parse worlds failed" );
	}

	// fill worlds map 
	for ( size_t i = 0; i < vWorldOffset.size(); ++i )
	{
		mWorldMap[ vWorldOffset[ i ] ].push_back( m_file.Data<NES_AREA_PTR>( uWorldsPtr + LOWORD( i ) ) );
	}

	// so split worlds by areas
	mWorlds.clear(); 
	mWorlds.insert( mWorlds.end(), NES_WORLDS_COUNT, std::vector<std::vector<CNesLevel*>>() );
	
	for ( auto it = mWorldMap.begin(); it != mWorldMap.end(); ++it )
	{
		const auto & v = it->second;
		const BYTE bWorldId = LOBYTE( it->first );
		const USHORT uCount = LOWORD( v.size() );
		USHORT uAreaNumber = 0;
		std::vector<CNesLevel*> vpLevel;

		for ( USHORT k = 0; k < uCount; ++k )
		{
			CNesLevel * pLevel = LevelFromPtr( v[ k ] );
			if ( pLevel )
			{
				vpLevel.push_back( pLevel );
				if ( !( pLevel->GetLevelStatus() & 2 ) )
				{
					mWorlds[ bWorldId ].push_back( vpLevel );
					vpLevel.clear();
				}
			}
		}

		if ( vpLevel.size() > 0 )
		{
			mWorlds[ bWorldId ].push_back( vpLevel );
		}
	}

	LoadHalfpages( mWorlds, mHalfPages );
	
	if ( !CheckWorldsArrays( mWorlds, mHalfPages ) )
	{
		throw std::exception( "Internal error. Worlds arrays does not matched!" );
	}
}

VOID CNesEditor::LoadHalfpages( NES_GAME_AREA_LIST & mWorlds, NES_GAME_AREA_HP_LIST & mHalfPages )
{
	mHalfPages.clear();
	mHalfPages.insert( mHalfPages.end(), NES_WORLDS_COUNT, std::vector<BYTE>() );
	USHORT uHalfPagesPtr = m_eptr[ eSharedHalfPages ].ptr + NES_WORLDS_COUNT;
	m_file.SelectPRG( m_eptr[ eSharedHalfPages ].bLoBank, TRUE );

	BYTE bWorld = 0;
	for ( const auto & world : mWorlds )
	{
		std::vector<BYTE> & vHP = mHalfPages[ bWorld++ ];
		for ( const auto & area : world )
		{
			vHP.push_back( m_file.Data<BYTE>( uHalfPagesPtr++ ) );
		}
	}
}

BOOL CNesEditor::CheckWorldsArrays( const NES_GAME_AREA_LIST & mWorlds, const NES_GAME_AREA_HP_LIST & mHalfPages )
{
	size_t cWorlds = mWorlds.size(), cHalfPages = mHalfPages.size();

	if ( cWorlds != cHalfPages || cWorlds != NES_WORLDS_COUNT )
	{
		return FALSE;
	}

	for ( size_t i = 0; i < cWorlds; ++i )
	{
		if ( mWorlds[ i ].size() != mHalfPages[ i ].size() )
		{
			return FALSE;
		}
	}

	return TRUE;
}

VOID CNesEditor::LoadLevels()
{
	BYTE bCHR[ 6 ] = { 0,2,4,5,6,7 };

	m_file.SelectCHR( bCHR );
	m_mTiles.Load();
	m_objData.LoadData();
	m_loop.LoadLoopList();
	m_demo.LoadData();
	m_player.LoadData();
	m_hack.LoadData();
	m_title.LoadData();
	LoadAreas( m_mLevel );
	LoadWorlds( m_mWorld, m_mHalfPages );	
	/*
		CBitmap bm;
		CNesLevelView view( bm, m_mTiles );

		for ( const auto & world : m_mWorld )
		{
			for ( const auto & level : world.second )
			{
				const auto & vaptr = level.second;

				for ( size_t i = 0; i < vaptr.size(); ++i )
				{
					CString str;
					// str.Format( TEXT( "d:\\STUFF\\dev\\src\\Super_Mario_Bros\\Levels\\%u-%u_%02u.png" ), world.first + 1, level.first + 1, i );
					str.Format(TEXT("c:\\Users\\AGATHA\\Documents\\STUFF\\src\\Super_Mario_Bros\\Levels\\%u-%u_%02u.png" ), world.first + 1, level.first + 1, i );

					const auto lvl = LevelFromPtr( vaptr[ i ] );
					lvl->DrawLevel( view, LOBYTE( world.first ) );
					view.RenderMap();

					CPngLoader png( view.Bitmap() );
					png.SavePng( str );
				}
			}
		}

	*/
	return;
}
