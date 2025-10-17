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

#define __IF_CONVERT_TO_HEX( chr, u )	\
	if ( chr >= _T('0') && chr <= _T('9') ) u = 0x10 * u + ( chr - _T( '0' ) ); \
	else if ( chr >= _T('A') && chr <= _T('F') ) u = 0x10 * u + ( chr - _T('A') + 10 ); \
	else if ( chr >= _T('a') && chr <= _T('f') ) u = 0x10 * u + ( chr - _T('a') + 10 )

std::map<CString, NES_EPOINTERS> __lXPtrMap =
{
	{ TEXT( "NES_X_MUSIC_PTR_1" ), eMusicPtr1 },
	{ TEXT( "NES_X_MUSIC_PTR_2" ), eMusicPtr2 },
	{ TEXT( "NES_X_MUSIC_PTR_3" ), eMusicPtr3 },
	{ TEXT( "NES_X_MUSIC_PTR_4" ), eMusicPtr4 },
	{ TEXT( "NES_X_MUSIC_PTR_5" ), eMusicPtr5 },
	{ TEXT( "NES_X_MUSIC_PTR_6" ), eMusicPtr6 },
	{ TEXT( "NES_X_MUSIC_PTR_7" ), eMusicPtr7 },
	{ TEXT( "NES_X_MUSIC_FREQ_PTR_1" ), eMusicFreqPtr1 },
	{ TEXT( "NES_X_MUSIC_FREQ_PTR_2" ), eMusicFreqPtr2 },
	{ TEXT( "NES_X_MUSIC_LEN_PTR" ), eMusicLenPtr },
	{ TEXT( "NES_X_MUSIC_ENV_CASTLE_PTR" ), eMusicEnvCastlePtr },
	{ TEXT( "NES_X_MUSIC_ENV_AREA_PTR" ), eMusicEnvAreaPtr },
	{ TEXT( "NES_X_MUSIC_ENV_WATER_PTR" ), eMusicEnvWaterPtr },
	{ TEXT( "NES_X_MUSIC_NOISE_FLAME_PTR" ), eMusicNoiseFlamePtr },
	{ TEXT( "NES_X_MUSIC_ENV_BRICK_PTR" ), eMusicEnvBrickPtr },
	{ TEXT( "NES_X_EOF_MUSIC_DATA" ), eEOFMusicData },
	{ TEXT( "NES_X_RESET_ENDLESS_JMP" ), eRESETEndlessJmp },
	{ TEXT( "NES_X_SOUND_ENGINE_PTR" ), eSoundEnginePtr },
	{ TEXT( "NES_X_GET_AREA_ADDRESSES" ), eGetAreaAddressesPtr },
	{ TEXT( "NES_X_PROCESS_AREA_DATA_1" ), eProcessAreaData1Ptr },
	{ TEXT( "NES_X_PROCESS_AREA_DATA_2" ), eProcessAreaData2Ptr },
	{ TEXT( "NES_X_ENEMY_CORE" ), eEnemiesAndLoopsCorePtr },
	{ TEXT( "NES_X_ENEMY_PTR_LO" ), eEnemyPtrLo },
	{ TEXT( "NES_X_ENEMY_PTR_HI" ), eEnemyPtrHi },
	{ TEXT( "NES_X_LEVEL_PTR_LO" ), eLevelPtrLo },
	{ TEXT( "NES_X_LEVEL_PTR_HI" ), eLevelPtrHi },
	{ TEXT( "NES_X_ENEMY_ADDR_H_OFFSET" ), eEnemyAddrHOffset },
	{ TEXT( "NES_X_LEVEL_ADDR_H_OFFSET" ), eLevelAddrHOffset },
	{ TEXT( "NES_X_BRICK_Q_METATILES" ), eBrickQMetatiles },
	{ TEXT( "NES_X_WORLD_AREAS_OFFSETS" ), eWorldAreasOffsets },
	{ TEXT( "NES_X_WORLD_AREAS" ), eWorldAreas },
	{ TEXT( "NES_X_FIND_AREA_POINTER" ), eFindAreaPointer },
	{ TEXT( "NES_X_SBR_PWRUPJMP" ), ePwrUpJmpPtr },
	{ TEXT( "NES_X_SBR_SETUP_VINE" ), eSetupVinePtr },
	{ TEXT( "NES_X_METATILES_PTR_HI" ), eMetatiesPtrHi },
	{ TEXT( "NES_X_METATILES_PTR_LO" ), eMetatiesPtrLo },
	{ TEXT( "NES_X_VRAM_TBL_PTR_HI" ), eVRAMAddrTableHi },
	{ TEXT( "NES_X_VRAM_TBL_PTR_LO" ), eVRAMAddrTableLo },
	{ TEXT( "NES_X_BG_SCENE_DATA_OFFSET" ), eBgSceneDataOffset },
	{ TEXT( "NES_X_BG_SCENERY_DATA" ), eBgSceneData },
	{ TEXT( "NES_X_BG_SCENERY_MTILES" ), eBgSceneMTiles },
	{ TEXT( "NES_X_FG_SCENE_DATA_OFFSET" ), eFgSceneDataOffset },
	{ TEXT( "NES_X_FG_SCENERY_DATA" ), eFgSceneData },
	{ TEXT( "NES_X_GROUND_MTILES" ), eGroundMTiles },
	{ TEXT( "NES_X_GROUND_BITS" ), eGroundBits },
	{ TEXT( "NES_X_BLOCK_BOUNDS" ), eBlockBounds },
	{ TEXT( "NES_X_BLOCK_W_WORLD_ID" ), eBlockSpecialWaterWorldId },
	{ TEXT( "NES_X_BLOCK_W_WORLD_8" ), eBlockSpecialWaterWorld8 },
	{ TEXT( "NES_X_BLOCK_CLOUD" ), eBlockCloud },
	{ TEXT( "NES_X_BLOCK_DEFAULT" ), eBlockDefault },
	{ TEXT( "NES_X_BRICKS_METATILES" ), eBricksMetatiles },
	{ TEXT( "NES_X_SOLID_METATILES" ), eSolidMetatiles },
	{ TEXT( "NES_X_COINS_METATILES" ), eCoinsMetatiles },
	{ TEXT( "NES_X_PIPE_METATILES" ), eVertPipeMetatiles },
	{ TEXT( "NES_X_HOLE_METATILES" ), eHoleMetatiles },
	{ TEXT( "NES_X_PUL_ROPE_METATILES" ), ePulleyRopeMetatiles },
	{ TEXT( "NES_X_LPIPE_SHFT_METATILES" ), eLPipeShaftMetatiles },
	{ TEXT( "NES_X_LPIPE_TOP_METATILES" ), eLPipeTopMetatiles },
	{ TEXT( "NES_X_LPIPE_BTM_METATILES" ), eLPipeBottomMetatiles },
	{ TEXT( "NES_X_SPEC_OBJ_Y" ), eSpecObjY },
	{ TEXT( "NES_X_SPEC_OBJ_METATILES" ), eSpecObjMetatiles },
	{ TEXT( "NES_X_CASTLE_METATILES" ), eCastleMetatiles },
	{ TEXT( "NES_X_STAIRCASE_Y" ), eStaircaseY },
	{ TEXT( "NES_X_STAIRCASE_HEIGHT" ), eStaircaseHeight },
	{ TEXT( "NES_X_BG_COLORS" ), eBgColors },
	{ TEXT( "NES_X_GFX_ENEMY" ), eGFXEnemy },
	{ TEXT( "NES_X_GFX_ENEMY_OFFSETS" ), eGFXEnemyOffsets },
	{ TEXT( "NES_X_GFX_POWERUP" ), eGFXPowerUp },
	{ TEXT( "NES_X_LOOP_PAGES" ), eLoopPages },
	{ TEXT( "NES_X_LOOP_Y" ), eLoopY },
	{ TEXT( "NES_X_LOOP_HANDLER_PTR" ), eLoopHandler },
	{ TEXT( "NES_X_LOOP_REW_OFFSETS_PTR" ), eLoopRewindOffsets },
	{ TEXT( "NES_X_LOOP_COUNTER_INC_CODE" ), eLoopCounterIncCode },
	{ TEXT( "NES_X_LOOP_CODE" ), eLoopCode },
	{ TEXT( "NES_X_LOOP_AREA_OFS_CODE" ), eLoopAreaOfsCode },
	{ TEXT( "NES_X_EXEC_LOOP_BACK" ), eExecLoopBack },
	{ TEXT( "NES_X_KILL_ALL_ENEMIES" ), eKillAllEnemies },
	{ TEXT( "NES_X_PLAYER_SPRITE_TABLE" ), ePlayerSpriteTable },
	{ TEXT( "NES_X_PLAYER_SPRITE_TILES" ), ePlayerTiles },
	{ TEXT( "NES_X_HALF_PAGES_CODE" ), eHalfPagesCode },
	{ TEXT( "NES_X_HALF_PAGES_PTR" ), eHalfPagesData },
	{ TEXT( "NES_X_DEMO_ACTION_ARRAY" ), eDemoActionArray },
	{ TEXT( "NES_X_DEMO_TIMING_ARRAY" ), eDemoTimingArray },
	{ TEXT( "NES_X_PLAYER_ENTRANCE_CTRL" ), ePlayerEntranceCtrl },
	{ TEXT( "NES_X_PLAYER_ENTRANCE_X" ), ePlayerEntranceX },
	{ TEXT( "NES_X_PLAYER_TIMER_CTRL" ), ePlayerTimerCtrl },
	{ TEXT( "NES_X_COINS_FOR_1UP" ), eCoinsFor1Up },
	{ TEXT( "NES_X_HARD_MODE_WORLD" ), eHardModeWorld },
	{ TEXT( "NES_X_HARD_MODE_LEVEL" ), eHardModeLevel },
	{ TEXT( "NES_X_WARP_ZONE_WORLDS" ), eWarpWorlds },
	{ TEXT( "NES_X_BOWSER_HAMMERS" ), eBowserHammersWorld },
	{ TEXT( "NES_X_PAL_ROTATE_COL_PTR" ), ePalRotateColors },
	{ TEXT( "NES_X_SPINY_EGG_PATCH_PLACE" ), eSpinyEggPatchPlace },
	{ TEXT( "NES_X_SMALLBBOX" ), eSmallBBox },
	{ TEXT( "NES_X_BOWSER_IDENTITIES" ), eBowserIdentities },
	{ TEXT( "NES_X_BOWSER_HITS" ), eBowserHits },
	{ TEXT( "NES_X_TILE_BRICK_ROW_BLOCK_ID" ), eTileListBricksRowTileId },
	{ TEXT( "NES_X_TILE_BRIDGE_TILE1" ), eTileListBridgeTile1 },
	{ TEXT( "NES_X_TILE_BRIDGE_TILE2" ), eTileListBridgeTile2 },
	{ TEXT( "NES_X_TILE_BRIDGE_HEIGHT0" ), eTileListBridgeHeight0 },
	{ TEXT( "NES_X_TILE_BRIDGE_HEIGHT1" ), eTileListBridgeHeight1 },
	{ TEXT( "NES_X_TILE_BRIDGE_HEIGHT2" ), eTileListBridgeHeight2 },
	{ TEXT( "NES_X_TILE_WATER_TILE1" ), eTileListWaterTile1 },
	{ TEXT( "NES_X_TILE_WATER_TILE2" ), eTileListWaterTile2 },
	{ TEXT( "NES_X_TILE_QBLOCK_ROW" ), eTileListQBlockRow },
	{ TEXT( "NES_X_TILE_QBLOCK_ROW_HEIGHT1" ), eTileListQBlockHeight1 },
	{ TEXT( "NES_X_TILE_QBLOCK_ROW_HEIGHT2" ), eTileListQBlockHeight2 },
	{ TEXT( "NES_X_TILE_ROPE" ), eTileListRope },
	{ TEXT( "NES_X_TILE_RESIDUAL" ), eTileListResidualBall },
	{ TEXT( "NES_X_TILE_STAIRCASE" ), eTileListStaircase },
	{ TEXT( "NES_X_TILE_TREE_LEDGE_LEFT" ), eTileListTreeLedgeLeft },
	{ TEXT( "NES_X_TILE_TREE_LEDGE_MDL" ), eTileListTreeLedgeMiddle },
	{ TEXT( "NES_X_TILE_TREE_LEDGE_RIGHT" ), eTileListTreeLedgeRight },
	{ TEXT( "NES_X_TILE_TREE_LEDGE_BTM" ), eTileListTreeLedgeBottom },
	{ TEXT( "NES_X_TILE_MUSH_LEDGE_LEFT" ), eTileListMushroomLedgeLeft },
	{ TEXT( "NES_X_TILE_MUSH_LEDGE_RIGHT" ), eTileListMushroomLedgeRight },
	{ TEXT( "NES_X_TILE_MUSH_LEDGE_MDL" ), eTileListMushroomLedgeMiddle },
	{ TEXT( "NES_X_TILE_MUSH_LEDGE_TOP" ), eTileListMushroomLedgeTop },
	{ TEXT( "NES_X_TILE_MUSH_LEDGE_BTM" ), eTileListMushroomLedgeBottom },
	{ TEXT( "NES_X_TILE_BULL_BILL_TOP" ), eTileListBulletBillTop },
	{ TEXT( "NES_X_TILE_BULL_BILL_MDL" ), eTileListBulletBillMiddle },
	{ TEXT( "NES_X_TILE_BULL_BILL_BTM" ), eTileListBulletBillBottom },
	{ TEXT( "NES_X_TILE_WATER_PIPE_BTM" ), eTileListWaterPipeBottom },
	{ TEXT( "NES_X_TILE_WATER_PIPE_TOP" ), eTileListWaterPipeTop },
	{ TEXT( "NES_X_TILE_JUMPSPRING_TOP" ), eTileListJumpspringTop },
	{ TEXT( "NES_X_TILE_JUMPSPRING_BTM" ), eTileListJumpspringBottom },
	{ TEXT( "NES_X_TILE_FLAGPOLE_TOP" ), eTileListFlagpoleTop },
	{ TEXT( "NES_X_TILE_FLAGPOLE_ROPE" ), eTileListFlagpoleRope },
	{ TEXT( "NES_X_TILE_FLAGPOLE_BOTTOM" ), eTileListFlagpoleBottom },
	{ TEXT( "NES_X_TILE_STONE_BLOCK" ), eTileListStoneBlock },
	{ TEXT( "NES_X_PATCH_PAL" ), ePatchPal },
	{ TEXT( "NES_X_BLANK_PALETTE" ), eBlankPal },
	{ TEXT( "NES_X_STYLE_VRAM_TABLES" ), eStyleVRAMTables },
	{ TEXT( "NES_X_PLAYER_COLORS" ), ePlayerColors },
	{ TEXT( "NES_X_CHR_TITLE_DATA_PTR_HI" ), eTitleScreenCHRPtrHi },
	{ TEXT( "NES_X_CHR_TITLE_DATA_PTR_LO" ), eTitleScreenCHRPtrLo },
	{ TEXT( "NES_X_TITLE_DATA_START_PTR_HI" ), eTitleScreenStartPtrHi },
	{ TEXT( "NES_X_TITLE_DATA_START_PTR_LO" ), eTitleScreenStartPtrLo },
	{ TEXT( "NES_X_TITLE_DATA_END_PTR_HI" ), eTitleScreenEndPtrHi },
	{ TEXT( "NES_X_TITLE_DATA_END_PTR_LO" ), eTitleScreenEndPtrLo },
	{ TEXT( "NES_S_MUSIC_PTR" ), eSharedMusicPtr },
	{ TEXT( "NES_S_NMI" ), eSharedNMIPtr },
	{ TEXT( "NES_S_RESET" ), eSharedRESETPtr },
	{ TEXT( "NES_S_IRQ" ), eSharedIRQPtr },
	{ TEXT( "NES_S_MMC3_NMI" ), eSharedMMC3NMI },
	{ TEXT( "NES_S_MMC3_RESET" ), eSharedMMC3RESET },
	{ TEXT( "NES_S_MMC3_IRQ" ), eSharedMMC3IRQ },
	{ TEXT( "NES_S_SOUND_ENGINE_PTR" ), eSharedSoundEnginePtr },
	{ TEXT( "NES_S_RESET_ENDLESS_JMP" ), eSharedRESETEndlessJmp },
	{ TEXT( "NES_S_ENEMY_PTR_LO" ), eSharedEnemyPtrLo },
	{ TEXT( "NES_S_SBR_PWRUPJMP" ), eSharedAltPwrUpJmpPtr },
	{ TEXT( "NES_S_SBR_SETUP_VINE" ), eSharedAltSetupVinePtr },
	{ TEXT( "NES_S_SAVE_AREA_PTR" ), eSharedSaveAreaPtr },
	{ TEXT( "NES_S_LOOP_COMMANDS" ), eSharedLoopCommands },
	{ TEXT( "NES_S_LOOP_HANDLER_PTR" ), eSharedLoopHandler },
	{ TEXT( "NES_S_AREA_HALFPAGES" ), eSharedHalfPages },
	{ TEXT( "NES_S_DEMO_DATA" ), eSharedDemoData },
	{ TEXT( "NES_S_INIT_GAME_POINTER" ), eInitGameSubPtr }
};

VOID __listSymbolFiles( const CString & strMask, std::vector<CString> & vstrSymbolFiles )
{
	WIN32_FIND_DATA ff = { 0 };
	CString strDirname;
	LPTSTR pszFilename;
	DWORD dwGFPNResult = GetFullPathName( strMask, strMask.GetLength() + 1, strDirname.GetBuffer( strMask.GetLength() + 1 ), &pszFilename );
	if ( dwGFPNResult )
	{
		*( pszFilename - 1 ) = 0;
	}

	strDirname.ReleaseBuffer();
	if ( !dwGFPNResult ) return;

	HANDLE hFind = FindFirstFile( strMask, &ff );
	if ( INVALID_HANDLE_VALUE == hFind ) return;

	CString strFile;
	do
	{
		if ( FILE_ATTRIBUTE_DIRECTORY & ff.dwFileAttributes ) continue;
		strFile.Format( TEXT( "%s\\%s" ), strDirname.GetString(), ff.cFileName );
		vstrSymbolFiles.push_back( strFile );
	} while ( FindNextFile( hFind, &ff ) );

	FindClose( hFind );
}

VOID __parseLabel( const CString & strLabel, USHORT uAddr, std::map<CString, DWORD> & mlPointers )
{
	// label format must match EDITOR.<LABEL>[.<HIBANK><LOBANK>]
#define __EDITOR_LABEL_PREFIX		TEXT("EDITOR.")

	if ( strLabel.Left( _ARRAYSIZE( __EDITOR_LABEL_PREFIX ) - 1 /* zero terminator */ ) == __EDITOR_LABEL_PREFIX )
	{
		CString strSubLabel = strLabel.Mid( _ARRAYSIZE( __EDITOR_LABEL_PREFIX ) - 1 );
		CString strBankNumbers;
		int uBankNumPos = strSubLabel.Find( _T( '.' ) );
		if ( uBankNumPos < 0 )
		{
			mlPointers[ strSubLabel ] = MAKEWPARAM( uAddr , 0xFFFF ) ;
		}
		else if ( uBankNumPos > 0 )
		{
			strBankNumbers = strSubLabel.Mid( uBankNumPos + 1 );
			strSubLabel = strSubLabel.Left( uBankNumPos );
			USHORT uBankNum = 0;
			uBankNumPos = strBankNumbers.GetLength();
			for ( int i = 0; i < uBankNumPos; ++i )
			{
				__IF_CONVERT_TO_HEX( strBankNumbers[ i ], uBankNum );
				else return;
			}

			mlPointers[ strSubLabel ] = MAKEWPARAM( uAddr, uBankNum );
		}
	}
}

VOID __parseLine( const CString & strLine, std::map<CString, DWORD> & mlPointers )
{
	LPCTSTR p = strLine.GetString();

	// line format $<ADDRESS>#<NAME>#
	if ( _T( '$' ) == *p )
	{
		USHORT uAddr = 0;
		CString strLabel;
		p++;
		while ( *p && *p != _T( '#' ) )
		{
			const auto & chr = *p;
			__IF_CONVERT_TO_HEX( chr, uAddr );
			else return;

			p++;
		}

		if ( !*p ) return;
		p++;

		while ( *p && *p != _T( '#' ) )
		{
			const auto & chr = *p;
			if ( chr >= _T( ' ' ) ) strLabel += chr;
			else return;
			p++;
		}

		if ( *p != _T( '#' ) ) return;

		__parseLabel( strLabel, uAddr + 1, mlPointers );
	}
}

VOID __parseSymbols( const CString & strFileContent, std::map<CString, DWORD> & mlPointers )
{
	int len = strFileContent.GetLength();
	std::vector<TCHAR> vstrLine;
	vstrLine.reserve( 1024 );

	for ( int i = 0; i < len; ++i )
	{
		const auto & chr = strFileContent[ i ];
		switch ( chr )
		{
			case _T( '\r' ): break;
			case _T( '\n' ):
				{
					vstrLine.push_back( 0 );
					__parseLine( vstrLine.data(), mlPointers );
					vstrLine.clear();
					break;
				}
			default:
				{
					if ( chr >= _T( ' ' ) ) vstrLine.push_back( chr );
					break;
				}
		}
	}
}

VOID __loadSymbols( const CString & strFilename, std::map<CString, DWORD> & mlPointers )
{
	std::vector<char> vFileBuffer;
	HANDLE hFile = CreateFile( strFilename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		_tprintf( TEXT( "Error: %d\n" ), GetLastError() ); return;
	}

	LARGE_INTEGER li;
	if ( GetFileSizeEx( hFile, &li ) )
	{
		if ( li.QuadPart < 10 * 1024 * 1024 )	// 10 MB
		{
			DWORD dw = 0;
			vFileBuffer.resize( li.QuadPart );
			if ( ReadFile( hFile, vFileBuffer.data(), LODWORD( vFileBuffer.size() ), &dw, nullptr ) )
			{
				CString strFileContent = vFileBuffer.data();
				__parseSymbols( strFileContent, mlPointers );
			}
		}
	}

	CloseHandle( hFile );

}

VOID CNesPointers::LoadPointersX()
{
#define __EDITOR_SHARED_PTR		TEXT("NES_S")
	const auto & strFilename = m_file.Name();
	CString strMask;
	strMask.Format( TEXT( "%s.*.nl" ), strFilename.GetString() );
	std::map<CString, DWORD> mlPointers;

	std::vector<CString> vstrSymbolFiles;
	__listSymbolFiles( strMask, vstrSymbolFiles );

	for ( const auto & sym : vstrSymbolFiles )
	{
		__loadSymbols( sym, mlPointers );
	}

	auto _itPtrEnd = __lXPtrMap.end();

	m_eptr.clear();

	for ( const auto & ptr : mlPointers )
	{
		auto _itPtr = __lXPtrMap.find( ptr.first );
		if ( _itPtr != _itPtrEnd )
		{
			const auto & _label = ptr.first;
			USHORT delta = 0;
			// shared ptrs are DIRECT pointers, so decrement it
			if ( _label.Left( _ARRAYSIZE( __EDITOR_SHARED_PTR ) - 1 ) == __EDITOR_SHARED_PTR )
			{
				delta = 1;
			}
			m_eptr[ _itPtr->second ].eptr = ptr.second - delta;
		}
	}

	return;
}