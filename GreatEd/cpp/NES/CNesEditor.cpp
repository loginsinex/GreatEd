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

CNesEditor::CNesEditor()
	: m_eptr( m_file ), m_mTiles( m_file, m_eptr, m_objData ), m_objData( m_file, m_eptr ), m_loop( m_file, m_eptr ),
	m_demo( m_file, m_eptr ), m_player( m_file, m_eptr ), m_hack( m_file, m_eptr ), m_title( m_file, m_eptr ), m_ver( ver_Unknown )
{

}

CNesEditor::~CNesEditor()
{
	FreeLevels();
}

VOID CNesEditor::LoadFile( LPCTSTR pszFile )
{
	NES_VERSION vers[] = { ver_X, ver_E, ver_I };
	BOOL fLoadFail = FALSE;
	std::exception ret_e;

	for ( const auto & ver : vers )
	{
		try
		{
			fLoadFail = FALSE;
			LoadFileVer( pszFile, ver );
			m_ver = ver;
			break;
		}
		catch ( std::exception & e )
		{
			m_file.FreeFile();
			fLoadFail = TRUE;
			ret_e = e;
		}
	}

	if ( fLoadFail )
	{
		throw ret_e;
	}
}

VOID CNesEditor::LoadFileVer( LPCTSTR pszFile, NES_VERSION ver )
{
	// try to load file as mmc3 by default

	if ( !m_file.LoadFile( pszFile ) )		// failed. Now try to load as NROM and convert it to MMC3
	{
		if ( ver_X == ver )	// don't try to convert NROM for X version
		{
			throw std::exception( "Not X version" );
		}

		m_eptr.Version( ver );

		CNesNROM nrom;
		CNesConverter conv( nrom, m_file, m_eptr );

		if ( !nrom.LoadFile( pszFile ) )
		{
			m_file.FreeFile();
			throw std::exception( "NROM mapper failed" );
		}

		try
		{
			// try to convert
			conv.Convert();
		}
		catch ( std::exception & e )
		{
			m_file.FreeFile();
			throw e;
		}
	}

	// here we're loaded OK
	// loading levels
	try
	{
		m_eptr.Version( ver );
		LoadLevels();
	}
	catch ( std::exception & e )
	{
		FreeLevels();
		throw e;
	}
}

VOID CNesEditor::SaveFile( LPCTSTR pszFile )
{
	CNesSimpleMMC3 savedState = m_file;

	try
	{
		DumpLevels();
		m_file.SaveFile( pszFile );
	}
	catch ( std::exception & e )
	{
		// revert state of file
		m_file = savedState;
		throw e;
	}
}

BOOL CNesEditor::IsFileLoaded()
{
	return m_file.IsFileLoaded();
}

LPCTSTR CNesEditor::Filename()
{
	return m_file;
}


CNesLevel * CNesEditor::LevelFromPtr( const NES_AREA_PTR & aptr )
{
	const auto it = m_mLevel.find( aptr.bType );
	if ( it == m_mLevel.end() )
	{
		return nullptr;
	}

	if ( aptr.bAreaId >= it->second.size() )
	{
		return nullptr;
	}

	return m_mLevel[ aptr.bType ][ aptr.bAreaId ];
}


VOID CNesEditor::FreeLevels()
{
	for ( auto it = m_mLevel.begin(); it != m_mLevel.end(); ++it )
	{
		while ( it->second.size() > 0 )
		{
			delete it->second.back();
			it->second.pop_back();
		}
	}

	m_mLevel.clear();
	m_mWorld.clear();
}

CNesLevelInterface * CNesEditor::GetInterface( CPaletteBitmap & bmView, BYTE bWorld, CNesLevel * pLevel, size_t nArea, BOOL fRenderTitle )
{
	if ( !m_file.IsFileLoaded() || !pLevel )
		return nullptr;

	if ( pLevel )
	{
		return new CNesLevelInterface( m_mTiles, m_hack, bmView, *pLevel, bWorld, fRenderTitle );
	}

	return nullptr;
}

CNesLevelsKeeper * CNesEditor::GetAreaList()
{
	if ( !m_file.IsFileLoaded() )
		return nullptr;

	return new CNesLevelsKeeper( m_gLinks, m_hack, m_mLevel, m_mTiles );
}

BOOL CNesEditor::GetAreaList( NES_GAME_AREA_LIST & nAreaList )
{
	if ( !m_file.IsFileLoaded() )
		return FALSE;

	nAreaList = m_mWorld;
	return TRUE;
}

BOOL CNesEditor::GetAreaList( NES_GAME_AREA_LIST & nAreaList, NES_GAME_AREA_HP_LIST & nHalfPageList )
{
	if ( GetAreaList( nAreaList ) )
	{
		nHalfPageList = m_mHalfPages;
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::SetAreaList( const NES_GAME_AREA_LIST & nAreaList, const NES_GAME_AREA_HP_LIST & nHalfPageList )
{
	if ( !m_file.IsFileLoaded() || !CheckWorldsArrays( nAreaList, nHalfPageList ) )
	{
		return FALSE;
	}
	
	m_mWorld = nAreaList;
	m_mHalfPages = nHalfPageList;
	return TRUE;
}

BOOL CNesEditor::AddPage( CNesLevel * pLevel, UINT uPage )
{
	if ( m_file.IsFileLoaded() && LevelFromPtr( pLevel->AreaPtr() ) )
	{
		pLevel->AddPage( uPage );
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::DelPage( CNesLevel * pLevel, UINT uPage )
{
	if ( m_file.IsFileLoaded() && LevelFromPtr( pLevel->AreaPtr() ) && !IsPageReferenced( pLevel, uPage ) )
	{
		pLevel->DelPage( uPage );
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::IsPageReferenced( CNesLevel * pLevel, UINT uPage )
{
	if ( m_file.IsFileLoaded() )
	{
		return m_gLinks.IsPageReferenced( pLevel, uPage );
	}

	return FALSE;
}

BOOL CNesEditor::UpdateAreaType( CNesLevel * pLevel, NES_LEVEL_TYPE type )
{
	if ( m_file.IsFileLoaded() )
	{
		NES_AREA_PTR aptr = pLevel->AreaPtr();

		if ( aptr.bType == type )
		{
			return TRUE;
		}

		auto itType = m_mLevel.find( aptr.bType );
		auto itNewType = m_mLevel.find( type );
		if ( itType != m_mLevel.end() && itNewType != m_mLevel.end() )
		{
			auto itArea = std::find( itType->second.begin(), itType->second.end(), pLevel );
			if ( itArea != itType->second.end() && itNewType->second.size() < 32 )
			{
				itType->second.erase( itArea );
				itNewType->second.push_back( pLevel );
				ValidateLevelPointers( m_mLevel );
				pLevel->InitObjects();
				return TRUE;
			}
		}
	}

	return FALSE;
}

CNesLevel * CNesEditor::CreateLevel( NES_LEVEL_TYPE uType, BOOL fAutoLevel )
{
	CNesLevel * pLevel = nullptr;
	if ( m_file.IsFileLoaded() )
	{
		auto itType = m_mLevel.find( uType );
		if ( itType != m_mLevel.end() )
		{
			if ( itType->second.size() < 32 )
			{
				CNesLevelHeader header;
				NES_AREA_PTR aptr = { 0 };
				std::vector<BYTE> vDummy1( 2, 0 ), vDummy2;
				std::vector<NES_LINK> vDummy3;
				aptr.bType = uType;
				vDummy1.push_back( 0x0D );
				vDummy1.push_back( 0x02 );	// set page skipper to value 2 for 2 pages
				pLevel = new CNesLevel( m_file, aptr, m_eptr, m_mTiles, m_objData, m_loop, m_gLinks, m_title, m_player );
				pLevel->LoadLevelData( vDummy1, vDummy2, vDummy3 );
				header.UpdatePlayerCtrl( plrCtrl2 );

				switch ( uType )
				{
					case nltWater:
					case nltGround:
					case nltUnderground:
						{
							header.UpdateGroundCtrl( 1 );
							break;
						}
					case nltCastle:
						{
							header.UpdateGroundCtrl( 7 );
							header.UpdateFgScenery( fgScenery3 );
							header.UpdatePlayerCtrl( plrCtrl3 );
							break;
						}
				}

				if ( fAutoLevel )
				{
					header.UpdatePlayerCtrl( plrCtrlAuto2 );
				}

				header.UpdateTimerCtrl( tmrCtrl1 );
				pLevel->UpdateHeader( header );

				itType->second.push_back( pLevel );
				ValidateLevelPointers( m_mLevel );
			}
		}
	}

	return pLevel;
}

CNesLevel * CNesEditor::CreateLevel( CNesLevel * pBaseLevel )
{
	CNesLevel * pLevel = nullptr;
	if ( m_file.IsFileLoaded() )
	{
		auto itType = m_mLevel.find( pBaseLevel->AreaPtr().bType );
		if ( itType != m_mLevel.end() )
		{
			if ( itType->second.size() < 32 )
			{
				pLevel = pBaseLevel->Copy();
				itType->second.push_back( pLevel );
				ValidateLevelPointers( m_mLevel );
			}
		}
	}

	return pLevel;
}


BOOL CNesEditor::GetPalette( NES_LEVEL_TYPE nlt, NES_AREA_STYLE nas, NES_BG_COLOR_CTL bgModifier, NES_PAL_ARRAY & npa )
{
	if (m_file.IsFileLoaded())
	{
		m_mTiles.ReadPalette( npa, nlt, nas, bgModifier );
	}
	return FALSE;
}

BOOL CNesEditor::SetPalette( NES_LEVEL_TYPE nlt, NES_AREA_STYLE nas, NES_BG_COLOR_CTL bgModifier, const NES_PAL_ARRAY & npa )
{
	if (m_file.IsFileLoaded())
	{
		m_mTiles.UpdatePalette( npa, nlt, nas, bgModifier );
		return TRUE;
	}
	return FALSE;
}

BOOL CNesEditor::GetPlayerPalette( NES_PLAYER_PAL & npc )
{
	if ( m_file.IsFileLoaded() )
	{
		npc = m_objData.PlayerPal();
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::SetPlayerPalette( const NES_PLAYER_PAL & npc )
{
	if ( m_file.IsFileLoaded() )
	{
		m_objData.SetPlayerPal( npc );
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::GetBgColors( NES_BG_COLORS & nbg )
{
	if ( m_file.IsFileLoaded() )
	{
		nbg = m_objData.BgColors();
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::SetBgColors( const NES_BG_COLORS & nbg )
{
	if ( m_file.IsFileLoaded() )
	{
		m_objData.SetBgColors( nbg );
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::IsValidLevel( CNesLevel * pLevel )
{
	if ( m_file.IsFileLoaded() )
	{
		for ( auto & levels : m_mLevel )
		{
			auto & levelList = levels.second;
			if ( levelList.end() != std::find( levelList.begin(), levelList.end(), pLevel ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

VOID CNesEditor::TakeAreasSnapshot( std::map<NES_AREA_PTR, CNesLevel*>& mAreaSnap )
{
	for ( auto & levels : m_mLevel )
	{
		for ( auto level : levels.second )
		{
			mAreaSnap[ level->AreaPtr() ] = level;
		}
	}
}

VOID CNesEditor::ValidateAreasSnapshot( std::map<NES_AREA_PTR, CNesLevel*>& mAreaSnap )
{
	std::vector<NES_AREA_PTR> vStaleAreas;

	for ( auto & level : mAreaSnap )
	{
		if ( !IsValidLevel( level.second ) )
		{
			vStaleAreas.push_back( level.first );
		}
	}

	// remove stale areas
	for ( auto & area : vStaleAreas )
	{
		mAreaSnap.erase( mAreaSnap.find( area ) );
	}
}

BOOL CNesEditor::TakeLevelSnapshot( CNesLevel * pLevel )
{
	if ( m_file.IsFileLoaded() && IsValidLevel( pLevel ) )
	{
		std::vector<NES_LEVEL_SNAPSHOT> & vLevelSnap = m_mSnap[ pLevel ];
		NES_LEVEL_SNAPSHOT snap;
		snap.uPages = pLevel->PagesCount();
		pLevel->GetLevelBinaryData( snap.vAreaData, snap.vObjectData );
		snap.vAreaData.pop_back();	// remove terminator 0xFD
		snap.vObjectData.pop_back();	// remove terminator 0xFF
		pLevel->TakeLoopsSnapshot( snap.vLoop );

		if ( vLevelSnap.size() > 0 )
		{
			NES_LEVEL_SNAPSHOT & prev = vLevelSnap.back();

			if ( snap.uPages == prev.uPages && snap.vAreaData == prev.vAreaData && snap.vObjectData == prev.vAreaData && snap.vLoop == prev.vLoop )
			{
				return FALSE;
			}
		}
		
		TakeAreasSnapshot( snap.mAreaSnap );
		vLevelSnap.push_back( snap );
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::IsLevelCanReverted( CNesLevel * pLevel )
{
	if ( m_file.IsFileLoaded() && IsValidLevel( pLevel ) )
	{
		auto itSnap = m_mSnap.find( pLevel );
		if ( itSnap != m_mSnap.end() )
		{
			std::vector<NES_LEVEL_SNAPSHOT> & vLevelSnap = itSnap->second;
			return ( vLevelSnap.size() > 0 );
		}
	}

	return FALSE;
}

BOOL CNesEditor::RevertLevelState( CNesLevel * pLevel )
{
	if ( m_file.IsFileLoaded() && IsValidLevel( pLevel ) )
	{
		auto itSnap = m_mSnap.find( pLevel );
		if ( itSnap != m_mSnap.end() )
		{
			std::vector<NES_LEVEL_SNAPSHOT> & vLevelSnap = itSnap->second;
			if ( vLevelSnap.size() > 0 )
			{
				std::map<NES_AREA_PTR, std::vector<NES_LINK>> mLinks;
				NES_LEVEL_SNAPSHOT & snap = vLevelSnap.back();
				ValidateLevelPointers( m_mLevel );

				pLevel->LoadLevelData( snap.vAreaData, snap.vObjectData, mLinks[ pLevel->AreaPtr() ] );

				// flush released pointers
				m_gLinks.FreeStaleLinks();
				ValidateAreasSnapshot( snap.mAreaSnap );

				// fix links
				std::vector<NES_LINK> & vLinks = mLinks[ pLevel->AreaPtr() ];
				std::vector<NES_LINK> vCorrectedLinks;
				for ( const auto & link : vLinks )
				{
					auto itLink = snap.mAreaSnap.find( link.aptrTarget );
					if ( itLink != snap.mAreaSnap.end() )
					{
						NES_LINK lNewLink = link;
						lNewLink.aptrTarget = itLink->second->AreaPtr();
						lNewLink.uTargetPage = min( itLink->second->PagesCount() - 1, lNewLink.uTargetPage );
						vCorrectedLinks.push_back( lNewLink );
					}
				}

				// overwrite old vector
				mLinks[ pLevel->AreaPtr() ] = vCorrectedLinks;

				// and load links
				ParseLinks( mLinks );
				pLevel->m_uPages = snap.uPages;
				pLevel->LevelSizeChanged();
				pLevel->InitObjects();

				// revert loops
				pLevel->RevertLoopsState( snap.vLoop );
				vLevelSnap.pop_back();	// done
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CNesEditor::PopLevelSnapshot( CNesLevel * pLevel )
{
	if ( IsLevelCanReverted( pLevel ) )
	{
		m_mSnap[ pLevel ].pop_back();
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::GetSourceLinkList( CNesLevel * pLevel, UINT uPage, std::vector<std::pair<BYTE, CNesLevel*>> & vLinks )
{
	if ( m_file.IsFileLoaded() && IsValidLevel( pLevel ) )
	{
		return m_gLinks.GetSourceList( pLevel, uPage, vLinks );
	}

	return FALSE;
}

BOOL CNesEditor::SetSourcePageLink( CNesLevel * pLevel, UINT uPage, UINT uNewPage )
{
	if ( m_file.IsFileLoaded() && IsValidLevel( pLevel ) && pLevel->PagesCount() > uPage && pLevel->PagesCount() > uNewPage )
	{
		return m_gLinks.SetSourcePage( pLevel, uPage, uNewPage );
	}

	return FALSE;
}

VOID CNesEditor::PointersChanged()
{
	if ( m_file.IsFileLoaded() )
	{
		m_gLinks.FreeStaleLinks();
	}
}

BOOL CNesEditor::GetAreaLevel( std::vector<std::pair<size_t, size_t>>& vLevels, CNesLevel * pLevel )
{
	vLevels.clear();
	if ( m_file.IsFileLoaded() )
	{
		size_t cWorlds = m_mWorld.size();
		for ( size_t w = 0; w < cWorlds; ++w )
		{
			size_t cLevels = m_mWorld[ w ].size();
			for ( size_t l = 0; l < cLevels; ++l )
			{
				for ( auto pWLevel : m_mWorld[ w ][ l ] )
				{
					if ( pLevel == pWLevel )
					{
						vLevels.push_back( { w,l } );
						break;
					}
				}
			}
		}

		return ( vLevels.size() > 0 );
	}

	return FALSE;
}

/*

// evil algorithm

VOID CNesEditor::PushAreaReferences( std::vector<CNesLevel*>& vGameAreas, CNesLevel * pLevel )
{
	auto it = std::find( vGameAreas.begin(), vGameAreas.end(), pLevel );
	if ( it != vGameAreas.end() ) return;

	vGameAreas.push_back( pLevel );

	size_t id = 1;
	CNesLevelItem * pObject = nullptr;
	std::map<BYTE, std::pair<UINT, CNesLevel*>> mObjectLinks;
	pLevel->ReadAreaLinks( mObjectLinks );

	do
	{
		if ( !mObjectLinks.empty() )
		{
			for ( const auto & lnk : mObjectLinks )
			{
				// first - world number
				// second - { target page, target level }
				const auto & pTargetLevel = lnk.second.second;
				PushAreaReferences( vGameAreas, pTargetLevel );
			}
		}

		do
		{
			pObject = static_cast<CNesLevelItem*>( pLevel->ObjectById( id++, FALSE ) );
			if ( pObject && pObject->IsEnterableObject() )
			{
				pObject->ReadLinks( mObjectLinks );
				break;
			}
		} while ( pObject );

	} while ( pObject );
}

BOOL CNesEditor::UnreferencedAreaList( std::vector<CNesLevel*>& vpUnusedAreas )
{
	if ( m_file.IsFileLoaded() )
	{
		// first fill the vector with all areas
		std::vector<CNesLevel*> vGameAreas;

		vpUnusedAreas.clear();
		for ( const auto & type : m_mLevel )
		{
			for ( auto pLevel : type.second )
			{
				vpUnusedAreas.push_back( pLevel );
			}
		}

		// Add to the vGameAreas all areas from worlds
		for ( const auto & worlds : m_mWorld )
		{
			for ( const auto & levels : worlds )
			{
				for ( auto pLevel : levels )
				{
					PushAreaReferences( vGameAreas, pLevel );
				}
			}
		}

		// remove all areas that existing in vGameAreas from vpUnusedAreas
		auto itEnd = vpUnusedAreas.end();

		for ( const auto & pGameLevel : vGameAreas )
		{
			auto _itEnd = std::remove( vpUnusedAreas.begin(), vpUnusedAreas.end(), pGameLevel );
			if ( _itEnd < itEnd ) itEnd = _itEnd;
		}

		if ( itEnd != vpUnusedAreas.end() ) vpUnusedAreas.erase( itEnd, vpUnusedAreas.end() );

		// now leaved areas - is unused, we can return it to GUI
		return ( vpUnusedAreas.size() > 0 );
	}

	return FALSE;
}*/

BOOL CNesEditor::UnreferencedAreaList( std::vector<CNesLevel*>& vpUnusedAreas )
{
	if ( m_file.IsFileLoaded() )
	{
		// first fill the vector with all areas
		vpUnusedAreas.clear();
		for ( const auto & type : m_mLevel )
		{
			for ( auto pLevel : type.second )
			{
				vpUnusedAreas.push_back( pLevel );
			}
		}

		// remove all areas, which found in the worlds list
		for ( const auto & worlds : m_mWorld )
		{
			for ( const auto & levels : worlds )
			{
				for ( auto pLevel : levels )
				{
					auto itLevel = std::find( vpUnusedAreas.begin(), vpUnusedAreas.end(), pLevel );
					if ( itLevel != vpUnusedAreas.end() )
					{
						vpUnusedAreas.erase( itLevel );
					}
				}
			}
		}

		// remove all referenced areas		
		CNesGameLinks & gLinks = m_gLinks;
		auto itReferenced = vpUnusedAreas.end();
		while ( vpUnusedAreas.end() !=
			( itReferenced = std::find_if( vpUnusedAreas.begin(), vpUnusedAreas.end(),
				[ &gLinks ]( CNesLevel * pLevel )
				{
					std::vector<CNesLevel*> vpSource;
					return gLinks.GetReferencesForTarget( vpSource, pLevel, FALSE );
				}
			) ) )
		{
			vpUnusedAreas.erase( itReferenced );
		};

		// now leaved areas - is unused, we can return it to GUI
		return ( vpUnusedAreas.size() > 0 );
	}

	return FALSE;
}

BOOL CNesEditor::DeleteArea( CNesLevel * pLevel )
{
	BOOL fErased = FALSE;
	if ( IsValidLevel( pLevel ) )
	{
		std::vector<std::pair<size_t, size_t>> vLevels;
		if ( !GetAreaLevel( vLevels, pLevel ) )
		{
			std::vector<CNesLevel*> vpSource;
			if ( !m_gLinks.GetReferencesForTarget( vpSource, pLevel, FALSE ) )
			{
				// remove area from the list
				for ( auto & type : m_mLevel )
				{
					auto & vAreas = type.second;
					auto itArea = std::find( vAreas.begin(), vAreas.end(), pLevel );
					if ( itArea != vAreas.end() )
					{
						vAreas.erase( itArea );
						fErased = TRUE;
					}
				}
			}
		}

		if ( fErased )
		{
			delete pLevel;
			ValidateLevelPointers( m_mLevel );
		}
	}

	return fErased;
}

NES_VERSION CNesEditor::GetVersion()
{
	return m_ver;
}

BOOL CNesEditor::GetDemoData( std::vector<std::pair<BYTE, BYTE>>& vDemoData )
{
	if ( m_file.IsFileLoaded() )
	{
		m_demo.DemoPlayData( vDemoData );
		return ( vDemoData.size() > 0 );
	}

	return FALSE;
}

BOOL CNesEditor::SetDemoData( const std::vector<std::pair<BYTE, BYTE>>& vDemoData )
{
	if ( m_file.IsFileLoaded() )
	{
		m_demo.WriteDemoPlayData( vDemoData );
		return TRUE;
	}

	return FALSE;
}

BYTE CNesEditor::GetTimerControl( NES_TIMER_CTRL tmrCtrl )
{
	if ( m_file.IsFileLoaded() )
	{
		return m_player.TimerControl( tmrCtrl );
	}

	return 0;
}

BYTE CNesEditor::GetEntranceControl( NES_PLAYER_CTRL entrCtrl )
{
	if ( m_file.IsFileLoaded() )
	{
		return m_player.EntranceControl( entrCtrl );
	}

	return 0;
}

VOID CNesEditor::SetTimerControl( NES_TIMER_CTRL tmrCtrl, BYTE bCtrl )
{
	if ( m_file.IsFileLoaded() )
	{
		m_player.UpdateTimerControl( tmrCtrl, bCtrl );
	}
}

VOID CNesEditor::SetEntranceControl( NES_PLAYER_CTRL plrCtrl, BYTE bCtrl )
{
	if ( m_file.IsFileLoaded() )
	{
		m_player.UpdateEntranceControl( plrCtrl, bCtrl );
	}
}

BOOL CNesEditor::GetHack( NES_ENGINE_HACK & hack )
{
	if ( m_file.IsFileLoaded() )
	{
		hack = m_hack.Read();
		for ( BYTE tmrCtrl = tmrCtrl0; tmrCtrl < tmrCtrlMax; ++tmrCtrl )
		{
			hack.timerValues.bTimerValue[ tmrCtrl ] = m_player.TimerControl( static_cast<NES_TIMER_CTRL>( tmrCtrl ) );
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CNesEditor::SetHack( const NES_ENGINE_HACK & hack )
{
	if ( m_file.IsFileLoaded() )
	{
		m_hack.Write( hack );
		for ( BYTE tmrCtrl = tmrCtrl0; tmrCtrl < tmrCtrlMax; ++tmrCtrl )
		{
			m_player.UpdateTimerControl( static_cast<NES_TIMER_CTRL>( tmrCtrl ), hack.timerValues.bTimerValue[ tmrCtrl ] );
		}
		return TRUE;
	}
	return FALSE;
}

VOID CNesEditor::SetPlayerPalOption( NES_PLAYER_COL_T npcOption )
{
	m_mTiles.SetPlayerPalOption( npcOption );
}

BOOL CNesEditor::MakeTestROM( const CString & strFilename, USHORT bTargetWorldLevel, const CNesLevel & lvl, BYTE bEntrancePage, BYTE bTimer )
{
	auto file = m_file;
	std::vector<BYTE> vCode0, vCode;
	const USHORT uCodeBase = m_eptr.Pointer( eInitGameSubPtr );
	const USHORT uSetupCodeBase = 0xA000;

	const SHORT AltEntranceControl = 0x0752;
	const SHORT EntrancePage = 0x0751;
	const SHORT OperMode = 0x0770;
	const SHORT OperModeTask = 0x0772;
	const SHORT GameTimerDisplay = 0x07F8;
	const SHORT AreaPointer = 0x0750;
	const SHORT PowerUpType = 0x39;
	const SHORT WorldNumber = 0x075F;
	const SHORT LevelNumber = 0x075C;
	const SHORT AreaNumber = 0x0760;
	const SHORT PlayerStatus = 0x756;
	const SHORT PlayerEntranceCtrl = 0x710;
	const SHORT NumberofLives = 0x75A;
	const SHORT Hidden1UpFlag = 0x75D;
	const SHORT PlayerSize = 0x754;
	const BYTE bEntranceCtrl = lvl.Header().PlayerCtl();

	CNesAssembly code0( uCodeBase );
	code0.LDY( INSTR_IMM( 0x0D ) );
	code0.JSR( TEXT( "SwitchHiBank" ) );
	code0.JMP( INSTR_ABS( uSetupCodeBase ) );
	code0.label( TEXT( "SwitchHiBank" ) );
	code0.LDA( INSTR_IMM( 0x07 ) );
	code0.STA( INSTR_ABS( 0x8000 ) );
	code0.STY( INSTR_ABS( 0x8001 ) );
	code0.RTS();

	CNesAssembly code( uSetupCodeBase );
	code.INC( INSTR_ABS( AltEntranceControl ) );
	code.INC( INSTR_ABS( Hidden1UpFlag ) );
	code.LDA( INSTR_IMM( bEntrancePage ) );
	code.STA( INSTR_ABS( EntrancePage ) );
	code.LDA( INSTR_IMM( 0x03 ) );
	code.STA( INSTR_ABS( NumberofLives ) );
	code.LDA( INSTR_IMM( 01 ) );
	code.STA( INSTR_ABS( OperMode ) );
	code.LDA( INSTR_IMM( 0x00 ) );
	code.STA( INSTR_ABS( PowerUpType ) );
	code.STA( INSTR_ABS( PlayerSize ) );
	code.LDA( INSTR_IMM( 0x02 ) );
	code.STA( INSTR_ABS( PlayerStatus ) );
	code.LDA( INSTR_IMM( bTimer ) );
	code.STA( INSTR_ABS( GameTimerDisplay ) );
	code.LDA( INSTR_IMM( lvl.AreaPtr().bPtr ) );
	code.STA( INSTR_ABS( AreaPointer ) );
	code.LDA( INSTR_IMM( HIBYTE( bTargetWorldLevel ) ) );
	code.STA( INSTR_ABS( WorldNumber ) );
	code.LDA( INSTR_IMM( LOBYTE( bTargetWorldLevel ) ) );
	code.STA( INSTR_ABS( LevelNumber ) );
	code.STA( INSTR_ABS( AreaNumber ) );
	//code.INC( INSTR_ABS( AreaNumber ) );
	code.LDA( INSTR_IMM( LOBYTE( lvl.Meta( TEXT( "BANK" ) ) ) ) );
	code.STA( INSTR_ABS( NES_BANK_VARIABLE ) );
	code.LDY( INSTR_IMM( 0x01 ) );
	code.JMP( INSTR_ABS( code0.LabelPC( "SwitchHiBank" ) ) );

	code0.Compile( vCode0 );
	code.Compile( vCode );
	file.SelectPRG( 0x00, FALSE );
	file.SelectPRG( 0x0D, TRUE );

	auto ptrCode0 = uCodeBase;
	for ( const auto & b : vCode0 ) file.Data<BYTE>( ptrCode0++ ) = b;

	auto ptrCode = uSetupCodeBase;
	for ( const auto & b : vCode ) file.Data<BYTE>( ptrCode++ ) = b;

//	file.Data<USHORT>( uInitializeAreaPtr ) = uNewInitializeArea;

	return file.SaveFile( strFilename );
}