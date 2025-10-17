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

CNesSingleBlockObjects::CNesSingleBlockObjects( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesLevelItem( nLevel, item )
{
	SetObjPlace( item.pt.x, item.pt.y );
}

BOOL CNesSingleBlockObjects::WidthChanging( int & cx )
{
	return FALSE;
}

BOOL CNesSingleBlockObjects::HeightChanging( int & cy )
{
	return FALSE;
}

BOOL CNesSingleBlockObjects::PlaceChanging( int & x, int & y )
{
	if ( y > 0x0b )
	{
		y = 0x0b;
	}

	return TRUE;
}

/// -- objects

CNesQBlock::CNesQBlock( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSingleBlockObjects( nLevel, item )
{
	SetObjectToForeground();
	SetObjWidth( 1 );
	SetObjHeight( 1 );
}

VOID CNesQBlock::InitObject()
{
	BYTE mtileId = Item().item.itemId;
	BYTE bSpriteId = 0xFF, bAttribute = 0;

	NES_METATILE mTile = Level().ObjData().Tile( eTileListStoneBlock );

	if ( mtileId < 0x0a )
	{
		if ( mtileId > 2 )
		{
			if ( Level().AreaType() != nltGround )
				mtileId += 5;			
		}

		mTile = Level().ObjData().QBlockMetatiles().bMetatile[ mtileId ];
	}
	SetMetatile( 0, 0, mTile );

	switch ( Item().item.itemId )
	{
		case 0x00: bSpriteId = nustPowerup; bAttribute = 2; break;
		case 0x02: bSpriteId = nustCoin; bAttribute = 2; break;
		case 0x03: bSpriteId = nustPowerup; bAttribute = 1; break;
		case 0x04: bSpriteId = nustPowerup; bAttribute = 2; break;
		case 0x05: bSpriteId = nustVine; bAttribute = 1; break;
		case 0x06: bSpriteId = nustStarman; bAttribute = 2; break;
		case 0x07: bSpriteId = nustCoin; bAttribute = 2; break;
		case 0x08: bSpriteId = nustPowerup; bAttribute = 1; break;
	}

	if ( 0xFF != bSpriteId )
	{
		SetSprite( bSpriteId, bAttribute, 0, 3 );
	}
}

BOOL CNesQBlock::IsEntranceObject()
{
	return ( Item().item.itemId == 0x05 );
}

NES_ENTRANCE_TYPE CNesQBlock::EntranceType()
{
	return ( 0x05 == Item().item.itemId ? etVine : etNotEntrance );
}

CNesWaterPipe::CNesWaterPipe( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSingleBlockObjects( nLevel, item )
{
	SetObjWidth( 1 );
	SetObjHeight( 2 );
}

VOID CNesWaterPipe::InitObject()
{
	SetMetatile( 0, 0, Level().ObjData().Tile( eTileListWaterPipeTop ) ); // { 0x6b } );
	SetMetatile( 0, 1, Level().ObjData().Tile( eTileListWaterPipeBottom ) ); // { 0x6c } );
}

BOOL CNesWaterPipe::IsEntranceObject()
{
	return TRUE;
}

NES_ENTRANCE_TYPE CNesWaterPipe::EntranceType()
{
	return etSideEntrance;
}

// jumpspring

CNesJumpspring::CNesJumpspring( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSingleBlockObjects( nLevel, item )
{
	SetObjectToForeground();
	SetObjWidth( 1 );
	SetObjHeight( 2 );
}

VOID CNesJumpspring::InitObject()
{
	SetMetatile( 0, 0, Level().ObjData().Tile( eTileListJumpspringTop ) );// { 0x67 } );
	SetMetatile( 0, 1, Level().ObjData().Tile( eTileListJumpspringBottom ) );// { 0x68 } );
	SetSprite( 40, 2, 0, 3 );
}

// intro pipe
CNesIntroPipe::CNesIntroPipe( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesLevelItem( nLevel, item )
{
	if ( 0x40 == Item().item.itemId )
	{
		OverrideSpecial( NES_SPECIAL_OBJECT_D );
	}

	SetObjPlace( item.pt.x, 0 );
	SetObjWidth( 4 );
	SetObjHeight( 11 );
}

BOOL CNesIntroPipe::PlaceChanging( int & x, int & y )
{
	if ( NES_SPECIAL_OBJECT_D == GetRealPoint().y && 0x0F == ( 0x0F & x ) )
	{
		return FALSE;
	}

	y = 0;
	return TRUE;
}

BOOL CNesIntroPipe::HeightChanging( int & cy )
{
	return FALSE;
}

BOOL CNesIntroPipe::WidthChanging( int & cx )
{
	return FALSE;
}

VOID CNesIntroPipe::InitObject()
{
	NES_PIPE_METATILES pipeMetatiles = Level().ObjData().PipeMetatiles();
	NES_LPIPE_METATILES lpipeMetatiles = Level().ObjData().LPipeMetatiles();

	int y = 0;
	for ( y = 0; y <= 6; ++y )
	{
		SetMetatile( 2, y, { 0 } );
		SetMetatile( 3, y, { 0 } );
	}

	SetMetatile( 2, 0 + y, pipeMetatiles.bMetatile[ 5 ] );
	SetMetatile( 3, 0 + y, pipeMetatiles.bMetatile[ 4 ] );
	SetMetatile( 2, 1 + y, pipeMetatiles.bMetatile[ 7 ] );
	SetMetatile( 3, 1 + y, pipeMetatiles.bMetatile[ 6 ] );

	SetMetatile( 0, 2 + y, lpipeMetatiles.SideTop.bMetatile[ 3 ] );
	SetMetatile( 1, 2 + y, lpipeMetatiles.SideTop.bMetatile[ 2 ] );
	SetMetatile( 2, 2 + y, lpipeMetatiles.SideTop.bMetatile[ 1 ] );
	SetMetatile( 3, 2 + y, lpipeMetatiles.SideTop.bMetatile[ 0 ] );
	SetMetatile( 0, 3 + y, lpipeMetatiles.SideBottom.bMetatile[ 3 ] );
	SetMetatile( 1, 3 + y, lpipeMetatiles.SideBottom.bMetatile[ 2 ] );
	SetMetatile( 2, 3 + y, lpipeMetatiles.SideBottom.bMetatile[ 1 ] );
	SetMetatile( 3, 3 + y, lpipeMetatiles.SideBottom.bMetatile[ 0 ] );
}

BOOL CNesIntroPipe::IsEntranceObject()
{
	return TRUE;
}

NES_ENTRANCE_TYPE CNesIntroPipe::EntranceType()
{
	return etIntroPipe;
}

// flagpole

CNesFlagpoleObject::CNesFlagpoleObject( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesLevelItem( nLevel, item )
{
	if ( 0x41 == Item().item.itemId )
	{
		OverrideSpecial( NES_SPECIAL_OBJECT_D );
	}

	SetObjPlace( item.pt.x, 0 );
	SetObjWidth( 1 );
	SetObjHeight( 11 );
}

BOOL CNesFlagpoleObject::PlaceChanging( int & x, int & y )
{
	if ( NES_SPECIAL_OBJECT_D == GetRealPoint().y && 0x0F == ( 0x0F & x ) )
	{
		return FALSE;
	}

	y = 0;
	return TRUE;
}

BOOL CNesFlagpoleObject::HeightChanging( int & cy )
{
	return FALSE;
}

BOOL CNesFlagpoleObject::WidthChanging( int & cx )
{
	return FALSE;
}

VOID CNesFlagpoleObject::InitObject()
{
	const NES_METATILE fpTop = Level().ObjData().Tile( eTileListFlagpoleTop ); // { 0x24 };
	const NES_METATILE fpMiddle = Level().ObjData().Tile( eTileListFlagpoleRope ); // { 0x25 };
	const NES_METATILE fpBottom = Level().ObjData().Tile( eTileListFlagpoleBottom ); // { 0x61 };

	SetSprite( nustFlagpole, 1, -1, 3 );
	SetMetatile( 0, 0, fpTop );
	SetMetatile( 0, 10, fpBottom );
	for ( BYTE y = 1; y < 10; ++y )
		SetMetatile( 0, y, fpMiddle );
}

// spec objects

CNesSpecObj::CNesSpecObj( CNesLevel & nLevel, const NES_LEVEL_ITEM & item, BYTE bItemId, BYTE len )
	:
	CNesLevelItem( nLevel, item ),
	m_y( nLevel.ObjData().SpecObjData().Y[ bItemId ] ),
	m_mTile( nLevel.ObjData().SpecObjData().tile[ bItemId ] ),
	m_len( len )
{
	SetObjPlace( item.pt.x, m_y );
	SetObjWidth( len );
	SetObjHeight( 1 );
}

BOOL CNesSpecObj::PlaceChanging( int & x, int & y )
{
	if ( NES_SPECIAL_OBJECT_D == Item().item.Y )
	{
		if ( 0x0F == ( x & 0x0F ) )		// objects with X=0x0F and Y=0x0D marks as 0xFD - EOF level
		{
			return FALSE;
		}
	}
	y = m_y;
	return TRUE;
}

BOOL CNesSpecObj::HeightChanging( int & cy )
{
	return FALSE;
}

BOOL CNesSpecObj::WidthChanging( int & cx )
{
	return FALSE;
}

VOID CNesSpecObj::InitObject()
{
	for ( BYTE x = 0; x < m_len; ++x )
		SetMetatile( x, 0, m_mTile );
}

// axe

CNesAxeObject::CNesAxeObject( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSpecObj( nLevel, item, 0, 1 )
{
	if ( 0x42 == Item().item.itemId )
	{
		OverrideSpecial( NES_SPECIAL_OBJECT_D );
	}

	SetObjectToForeground();
}

// chain

CNesChainObject::CNesChainObject( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSpecObj( nLevel, item, 1, 1 )
{
	if ( Item().item.itemId == 0x43 )
	{
		OverrideSpecial( NES_SPECIAL_OBJECT_D );
	}

	SetObjectToForeground();
}

// bridge

CNesCastleBridgeObject::CNesCastleBridgeObject( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesSpecObj( nLevel, item, 2, 13 )
{
	OverrideSpecial( NES_SPECIAL_OBJECT_D );
}

// command objects

CNesCommandObject::CNesCommandObject( CNesLevel & nLevel, const NES_LEVEL_ITEM & item, const NES_METATILE & tile )
	: CNesLevelItem( nLevel, item ), m_tile( tile )
{
	OverrideSpecial( NES_SPECIAL_OBJECT_D );
	SetObjectToForeground();
	SetObjPlace( item.pt.x, 13 );
	SetObjWidth( 1 );
	SetObjHeight( 1 );
}

BOOL CNesCommandObject::PlaceChanging( int & x, int & y )
{
	if ( 0x0F == ( x & 0x0F ) )
	{
		return FALSE;
	}

	y = 13;
	return TRUE;
}

BOOL CNesCommandObject::HeightChanging( int & cy )
{
	return FALSE;
}

BOOL CNesCommandObject::WidthChanging( int & cx )
{
	return FALSE;
}

VOID CNesCommandObject::InitObject()
{
	SetMetatile( 0, 0, m_tile );
}

// warp scroll stop

CNesWarpScrollStop::CNesWarpScrollStop( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x41 } )
{
	
}

// scroll stop

CNesScrollStop::CNesScrollStop( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x40 } )
{

}

// frenzy 1
CNesFrenzyObject1::CNesFrenzyObject1( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x0e } ) // { 0x88 } )
{

}

// frenzy 2
CNesFrenzyObject2::CNesFrenzyObject2( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x64 } ) // { 0x0d } )
{

}

// frenzy 3
CNesFrenzyObject3::CNesFrenzyObject3( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x88 } ) // { 0x67 } )
{

}

// loop command
CNesLoopCommand::CNesLoopCommand( CNesLevel & nLevel, const NES_LEVEL_ITEM & item )
	: CNesCommandObject( nLevel, item, { 0x24 } ), // { 0x26 } )
	m_fExternal( FALSE )
{
	m_lp.Y = 0xB0;
	m_lp.bIdInGroup = 0;
	m_lp.bPage = max( NES_LOOP_COMMAND_MINIMUM_PAGE, LOBYTE( item.pt.x / 16 + 1 ) );
}

VOID CNesLoopCommand::SetLoop( const NES_LOOP_COMMAND & lp )
{
	POINT pt = GetObjectPoint();

	m_lp.bIdInGroup = lp.bIdInGroup;
	m_lp.Y = lp.Y;
	m_lp.bPage = lp.bPage;
	// SetObjPlace( pt.x & 0x0F | ( static_cast<int>( lp.bPage ) << 4 ), pt.y );
	m_fExternal = TRUE;
}

VOID CNesLoopCommand::GetLoop( NES_LOOP_COMMAND & lp )
{
	lp.bIdInGroup = m_lp.bIdInGroup;
	lp.Y = m_lp.Y;
	lp.bPage = m_lp.bPage;
	lp.bRewindOffset = m_lp.bRewindOffset;
}

BOOL CNesLoopCommand::LoopConnected()
{
	return m_lp.bIdInGroup;
}

VOID CNesLoopCommand::LoopConnected( BOOL fConnected )
{
	m_lp.bIdInGroup = ( fConnected ? 1 : 0 );
	m_fExternal = FALSE;
}

BYTE CNesLoopCommand::AllowedHeight()
{
	return m_lp.Y / 16;
}

VOID CNesLoopCommand::AllowedHeight( BYTE Y )
{
	m_lp.Y = LOBYTE( Y * 16 );
	m_fExternal = FALSE;
}

BOOL CNesLoopCommand::IsExternalSet()
{
	return m_fExternal;
}

BYTE CNesLoopCommand::TriggerPage()
{
	return m_lp.bPage;
}

VOID CNesLoopCommand::TriggerPage( BYTE bPage )
{
	bPage = max( ( LoopConnected() ? NES_LOOP_OWNED_COMMAND_MINIMUM_PAGE : NES_LOOP_COMMAND_MINIMUM_PAGE ), bPage );
	m_lp.bPage = bPage;
}

VOID CNesLoopCommand::RewindOffset( BYTE bRewindOffset )
{
	m_lp.bRewindOffset = bRewindOffset;
}

BOOL CNesLoopCommand::PlaceChanging( int & x, int & y )
{
	if ( x / 16 >= static_cast<int>( m_lp.bPage ) || 0x0F == ( x & 0x0F ) )
	{
		return FALSE;
	}

	y = NES_SPECIAL_OBJECT_D;
	return TRUE;
}

VOID CNesLoopCommand::DrawExtra( CNesLevelView & view, BYTE bWorldId, int x, int y, BOOL fAfterObject )
{
	if ( fAfterObject )
	{
		// draw player as marker of pass view
		// get the right point for sprite
		NES_SPRITE spr = { 0 };
		POINT pt = { ( m_lp.bPage - 1 ) << 4, m_lp.Y >> 4 };
		spr.attr = ( m_lp.bIdInGroup ? 1 : 0 );
		spr.id = npstBigRunning;
		view.DrawSprite( pt.x << 1, (pt.y - 2) << 1, spr );
		spr.id += NES_PLAYER_GFX_OFFSET;
		view.DrawSprite( pt.x << 1, ( ( pt.y - 2 ) << 1 ) + 1, spr );

		// draw sprite when player occurs
		if ( !m_lp.bIdInGroup )
		{
			pt.x = ( m_lp.bPage - 4 ) << 4;
			pt.y = ( m_lp.Y >> 4 );
			spr.attr = 0;
			spr.id = npstBigTurning;
			view.DrawSprite( pt.x << 1, ( pt.y - 2 ) << 1, spr );
			spr.id += NES_PLAYER_GFX_OFFSET;
			view.DrawSprite( pt.x << 1, ( ( pt.y - 2 ) << 1 ) + 1, spr );
		}
	}
}