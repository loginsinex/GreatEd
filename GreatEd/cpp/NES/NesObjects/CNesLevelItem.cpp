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

CNesLevelItem::CNesLevelItem( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesObject( nLevel, FALSE ), m_item( item ), m_fForceForeground( FALSE )
{

}

CNesLevelItem::CNesLevelItem( CNesLevel & nLevel )
	: CNesObject( nLevel, FALSE ), m_fForceForeground( FALSE )
{
	ZeroMemory( &m_item, sizeof( m_item ) );
}

const NES_LEVEL_ITEM & CNesLevelItem::Item()
{
	return m_item;
}

VOID CNesLevelItem::ModifyItemId( BYTE bItemId )
{
	m_item.item.itemId = bItemId;
}

BOOL CNesLevelItem::IsObjectForeground()
{
	return m_fForceForeground;
}

VOID CNesLevelItem::SetObjectToForeground()
{
	m_fForceForeground = TRUE;
}

VOID CNesLevelItem::DrawExtra( CNesLevelView & view, BYTE bWorldId, int x, int y, BOOL fAfterObject )
{
	if ( fAfterObject )
	{
		if ( IsEnterableObject() )
		{
			view.DrawMetatile( GetObjectRect().left, 15, { 0xc0 } );
		}
	}
}

NES_METATILE CNesLevelItem::HandleMetatile( BYTE bWorldId, int X, int Y, const NES_METATILE & currentTile, const NES_METATILE & newTile )
{
	/*
	if ( IsObjectForeground() )
	{
		return newTile;
	}
	*/

//	return newTile;

	const BYTE & srcTile = currentTile.bMetatile;
	const BYTE & dstTile = newTile.bMetatile;

	if ( srcTile == 0x00 )
	{
		return newTile;
	}

	if ( dstTile == 0x00 )
	{
		if (
			srcTile != 0x51 && srcTile != 0x52 && srcTile != 0x53 &&
			!( srcTile > 0x55 && srcTile < 0x5F )
			)
		{
			return newTile;
		}
		else
		{
			return currentTile;
		}
	}

	if ( srcTile == 0x17 )
		return currentTile;

	if ( srcTile == 0x1a )
		return currentTile;

	if ( srcTile >= 0xc0 )
		return currentTile;

	if ( srcTile != 0x54 )
		return newTile;

	if ( dstTile == 0x50 )
		return currentTile;

	return newTile;
}

BOOL CNesLevelItem::IsEntranceObject()
{
	return FALSE;
}

BOOL CNesLevelItem::IsEnterableObject()
{
	return ( m_mLinks.size() > 0 );
}

NES_ENTRANCE_TYPE CNesLevelItem::EntranceType()
{
	return etNotEntrance;
}

CNesLevel * CNesLevelItem::GetTargetLevel( BYTE uWorld, BYTE & uPage )
{
	auto it = m_mLinks.find( uWorld );
	if ( it != m_mLinks.end() )
	{
		CNesLink * pLink = it->second;
		if ( pLink )
		{
			uPage = pLink->TargetPage();
			return pLink->TargetLevel();
		}
	}

	return nullptr;
}

VOID CNesLevelItem::DumpPointers( std::vector<NES_LEVEL_ENEMY_ITEM>& vItem )
{
	for ( const auto & wl : m_mLinks )
	{
		const BYTE bWorld = wl.first;
		CNesLink * pLink = wl.second;
		NES_LEVEL_ENEMY_ITEM item = { 0 };

		item.pt.x = GetObjectRect().left;
		item.pt.y = NES_SPECIAL_OBJECT_E;
		item.item.Y = NES_SPECIAL_OBJECT_E;
		item.item.X = LOBYTE( item.pt.x ) & 0x0F;
		item.item.bData[ 1 ] = pLink->TargetLevel()->AreaPtr().bPtr;
		item.item.bData[ 2 ] = ( ( ( bWorld & 7 ) << 5 ) | pLink->TargetPage() & 0x1f );

		vItem.push_back( item );
	}
}

CNesLevelItem::~CNesLevelItem()
{
	for ( auto lnk : m_mLinks )
	{
		lnk.second->Release();
	}
}

VOID CNesLevelItem::SetLink( CNesLink * pLink )
{
	BYTE uWorld = pLink->SourceWorld();
	RemoveLink( uWorld );
	m_mLinks[ uWorld ] = pLink;
	pLink->AddRef();
	InitObject();
}

VOID CNesLevelItem::ReadLinks( std::map<BYTE, std::pair<UINT, CNesLevel*>>& mLinks )
{
	for ( const auto & world : m_mLinks )
	{
		mLinks[ world.first ] = { world.second->TargetPage(), world.second->TargetLevel() };
	}
}

VOID CNesLevelItem::RemoveLink( BYTE uWorld )
{
	auto it = m_mLinks.find( uWorld );
	if ( it != m_mLinks.end() )
	{
		it->second->Release();
		m_mLinks.erase( it );
		InitObject();
	}
}

VOID CNesLevelItem::RemoveLinks()
{
	for ( auto lnk : m_mLinks )
	{
		lnk.second->Release();
	}

	m_mLinks.clear();

	InitObject();
	
}

