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

CNesLevelsKeeper::CNesLevelsKeeper( CNesGameLinks & links, CNesGameEngineHack & hack, NES_AREA_GROUPS & nAreaList, CNesMetatiles & mTiles )
	: m_gLinks( links ), m_nAreaList( nAreaList ), m_mTiles( mTiles ), m_hack( hack )
{

}

size_t CNesLevelsKeeper::AreasCount( NES_LEVEL_TYPE lType )
{
	auto it = m_nAreaList.find( lType );

	if ( it != m_nAreaList.end() )
	{
		return it->second.size();
	}

	return 0;
}

CNesLevel * CNesLevelsKeeper::EnumAreas( NES_LEVEL_TYPE lType, size_t id )
{
	auto it = m_nAreaList.find( lType );

	if ( it != m_nAreaList.end() )
	{
		if ( id < it->second.size() )
		{
			return it->second[ id ];
		}
	}

	return nullptr;
}

BOOL CNesLevelsKeeper::FindTarget( CNesLevel * pLevel, NES_LEVEL_TYPE & lType, size_t & id )
{
	for ( auto it : m_nAreaList )
	{
		auto lvl = std::find( it.second.begin(), it.second.end(), pLevel );
		if ( lvl != it.second.end() )
		{
			lType = it.first;
			id = ( lvl - it.second.begin() );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CNesLevelsKeeper::SetObjectTarget( BYTE bWorld, CNesLevelItem * pObj, CNesLevel * pLevel, BYTE uTargetPage )
{
	// find the level from areas
	size_t id;
	NES_LEVEL_TYPE lType;

	if ( FindTarget( pLevel, lType, id ) )
	{
		if ( uTargetPage < pLevel->PagesCount() )
		{
			CNesLink * pLink = m_gLinks.AddLink( bWorld, &pObj->Level() );
			pLink->SetTarget( pLevel, uTargetPage );

			pObj->SetLink( pLink );
			m_gLinks.FreeStaleLinks();
			return TRUE;
		}
	}

	return FALSE;
}

CNesLevelInterface * CNesLevelsKeeper::GetInterface( BYTE bWorld, CPaletteBitmap & bmView, CNesLevel * pLevel )
{
	return new CNesLevelInterface( m_mTiles, m_hack, bmView, *pLevel, bWorld );
}