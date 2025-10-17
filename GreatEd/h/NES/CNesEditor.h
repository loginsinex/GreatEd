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


class CNesLevel;

typedef struct _tagNES_LEVEL_SNAPSHOT
{
	std::vector<BYTE>					vAreaData;
	std::vector<BYTE>					vObjectData;
	std::vector<NES_LOOP_COMMAND>		vLoop;
	BYTE								uPages;
	std::map<NES_AREA_PTR, CNesLevel*>	mAreaSnap;
} NES_LEVEL_SNAPSHOT, *PNES_LEVEL_SNAPSHOT;


typedef std::map<CNesLevel*, std::vector<NES_LEVEL_SNAPSHOT>>		LEVEL_SNAPSHOT_LIST;

class CNesEditor
{
	CNesSimpleMMC3										m_file;
	CNesPointers										m_eptr;
	CNesObjectData										m_objData;
	CNesGameLinks										m_gLinks;
	CNesGameLoopCommand									m_loop;
	CNesDemoPlay										m_demo;
	NES_VERSION											m_ver;
	CNesPlayerEntrance									m_player;
	CNesGameEngineHack									m_hack;
	CNesTitleScreen										m_title;

	NES_AREA_GROUPS				m_mLevel;
	NES_GAME_AREA_LIST			m_mWorld;
	NES_GAME_AREA_HP_LIST		m_mHalfPages;

	CNesMetatiles				m_mTiles;

	LEVEL_SNAPSHOT_LIST			m_mSnap;

	VOID						LoadLevels();
	VOID						DumpLevels();
	VOID						FreeLevels();
	VOID						ReadROMData( USHORT uPtr, std::vector<BYTE> & vData, BYTE bMarker, BOOL fObjectMarker );
	USHORT						WriteROMData( USHORT uPtr, std::vector<BYTE> & vData );
	VOID						LoadAreas( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>> & mAreas );
	VOID						LoadWorlds( NES_GAME_AREA_LIST & mWorlds, NES_GAME_AREA_HP_LIST & mHalfPages );
	VOID						LoadHalfpages( NES_GAME_AREA_LIST & mWorlds, NES_GAME_AREA_HP_LIST & mHalfPages );
	BOOL						CheckWorldsArrays( const NES_GAME_AREA_LIST & mWorlds, const NES_GAME_AREA_HP_LIST & mHalfPages );
	VOID						DumpAreas( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>>& mAreas );
	VOID						ValidateLevelPointers( std::map<NES_LEVEL_TYPE, std::vector<CNesLevel*>>& mAreas );
	VOID						DumpWorlds( const NES_GAME_AREA_LIST & mWorlds, const NES_GAME_AREA_HP_LIST & mHalfPages );
	VOID						DumpHalfPages( const NES_GAME_AREA_HP_LIST & mHalfPages );
	CNesLevel *					LevelFromPtr( const NES_AREA_PTR & aptr );
	VOID						LoadFileVer( LPCTSTR pszFile, NES_VERSION ver );
	VOID						ParseLinks( std::map<NES_AREA_PTR, std::vector<NES_LINK>> & mLink );
	BOOL						IsValidLevel( CNesLevel * pLevel );
	VOID						TakeAreasSnapshot( std::map<NES_AREA_PTR, CNesLevel*> & mAreaSnap );
	VOID						ValidateAreasSnapshot( std::map<NES_AREA_PTR, CNesLevel*> & mAreaSnap );
	VOID						PushAreaReferences( std::vector<CNesLevel*> & vGameAreas, CNesLevel * pLevel );

public:
	CNesEditor();
	~CNesEditor();

	VOID						LoadFile( LPCTSTR pszFile );
	VOID						SaveFile( LPCTSTR pszFile );
	BOOL						IsFileLoaded();
	LPCTSTR						Filename();

	CNesLevelInterface *		GetInterface( CPaletteBitmap & bmView, BYTE bWorld, CNesLevel * pLevel, size_t nArea = 0, BOOL fRenderTitle = FALSE );
	CNesLevelsKeeper *			GetAreaList();
	BOOL						GetAreaList( NES_GAME_AREA_LIST & nAreaList );
	BOOL						GetAreaList( NES_GAME_AREA_LIST & nAreaList, NES_GAME_AREA_HP_LIST & nHalfPageList );
	BOOL						SetAreaList( const NES_GAME_AREA_LIST & nAreaList, const NES_GAME_AREA_HP_LIST & nHalfPageList );

	BOOL						AddPage( CNesLevel * pLevel, UINT uPage );
	BOOL						DelPage( CNesLevel * pLevel, UINT uPage );
	BOOL						IsPageReferenced( CNesLevel * pLevel, UINT uPage );
	BOOL						UpdateAreaType( CNesLevel * pLevel, NES_LEVEL_TYPE type );

	CNesLevel*					CreateLevel( NES_LEVEL_TYPE uType, BOOL fAutoLevel );
	CNesLevel*					CreateLevel( CNesLevel * pBaseLevel );

	BOOL						GetPalette( NES_LEVEL_TYPE nlt, NES_AREA_STYLE nas, NES_BG_COLOR_CTL bgModifier, NES_PAL_ARRAY & npa );
	BOOL						SetPalette( NES_LEVEL_TYPE nlt, NES_AREA_STYLE nas, NES_BG_COLOR_CTL bgModifier, const NES_PAL_ARRAY & npa );
	BOOL						GetPlayerPalette( NES_PLAYER_PAL & npc );
	BOOL						SetPlayerPalette( const NES_PLAYER_PAL & npc );

	BOOL						GetBgColors( NES_BG_COLORS & nbg );
	BOOL						SetBgColors( const NES_BG_COLORS & nbg );

	BOOL						TakeLevelSnapshot( CNesLevel * pLevel );
	BOOL						PopLevelSnapshot( CNesLevel * pLevel );
	BOOL						RevertLevelState( CNesLevel * pLevel );
	BOOL						IsLevelCanReverted( CNesLevel * pLevel );

	BOOL						GetSourceLinkList( CNesLevel * pLevel, UINT uPage, std::vector<std::pair<BYTE, CNesLevel*>> & vLinks );
	BOOL						SetSourcePageLink( CNesLevel * pLevel, UINT uPage, UINT uNewPage );
	VOID						PointersChanged();

	BOOL						UnreferencedAreaList( std::vector<CNesLevel*> & vpUnusedAreas );
	BOOL						DeleteArea( CNesLevel * pLevel );
	BOOL						GetAreaLevel( std::vector<std::pair<size_t, size_t>> & vLevels, CNesLevel * pLevel );
	BOOL						GetDemoData( std::vector<std::pair<BYTE, BYTE>> & vDemoData );
	BOOL						SetDemoData( const std::vector<std::pair<BYTE, BYTE>> & vDemoData );

	BYTE						GetTimerControl( NES_TIMER_CTRL tmrCtrl );
	BYTE						GetEntranceControl( NES_PLAYER_CTRL entrCtrl );
	VOID						SetTimerControl( NES_TIMER_CTRL tmrCtrl, BYTE bCtrl );
	VOID						SetEntranceControl( NES_PLAYER_CTRL plrCtrl, BYTE bCtrl );
	BOOL						GetHack( NES_ENGINE_HACK & hack );
	BOOL						SetHack( const NES_ENGINE_HACK & hack );
	VOID						SetPlayerPalOption( NES_PLAYER_COL_T npcOption );

	NES_VERSION					GetVersion();

	BOOL						MakeTestROM( const CString & strFilename, USHORT bTargetWorldLevel, const CNesLevel & lvl, BYTE bEntrancePage, BYTE bTimer );
};
