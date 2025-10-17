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

const NES_EPOINTERS g_eTile[] = 
{
	eTileListBricksRowTileId,
	eTileListBridgeTile1,
	eTileListBridgeTile2,
	eTileListBridgeHeight0,
	eTileListBridgeHeight1,
	eTileListBridgeHeight2,
	eTileListWaterTile1,
	eTileListWaterTile2,
	eTileListQBlockRow,
	eTileListQBlockHeight1,
	eTileListQBlockHeight2,
	eTileListRope,
	eTileListResidualBall,
	eTileListStaircase,
	eTileListTreeLedgeLeft,
	eTileListTreeLedgeMiddle,
	eTileListTreeLedgeRight,
	eTileListTreeLedgeBottom,
	eTileListMushroomLedgeLeft,
	eTileListMushroomLedgeRight,
	eTileListMushroomLedgeMiddle,
	eTileListMushroomLedgeTop,
	eTileListMushroomLedgeBottom,
	eTileListBulletBillTop,
	eTileListBulletBillMiddle,
	eTileListBulletBillBottom,
	eTileListWaterPipeTop,
	eTileListWaterPipeBottom,
	eTileListJumpspringTop,
	eTileListJumpspringBottom,
	eTileListFlagpoleTop,
	eTileListFlagpoleRope,
	eTileListFlagpoleBottom,
	eTileListStoneBlock
};

CNesObjectData::CNesObjectData( CNesSimpleMMC3 & file, const CNesPointers & eptr )
	: m_file( file ), m_eptr( eptr )
{

}

VOID CNesObjectData::LoadData( )
{
	LoadGroundData();
	LoadQBlockMetatiles();
	LoadBricksMetatiles();
	LoadSolid();
	LoadPipeMetatiles();
	LoadPulleyRopeMetatiles();
	LoadSpecObjData();
	LoadCastleMetatiles();
	LoadStaircaseData();
	LoadBgColors();
	LoadEnemyGFX();
	LoadItemTiles();
	LoadPlayerPal();
}

VOID CNesObjectData::DumpData()
{
	DumpBgColors();
	DumpItemTiles();
	DumpPlayerPal();
}

VOID CNesObjectData::LoadGroundData()
{
	const CNesPointers & eptr = m_eptr;

	CopyMemory( m_grData.bBgSceneOffset, m_file.Data<BYTE[ 3 ]>( eptr.Pointer( eBgSceneDataOffset ) + 1 ), _ARRAYSIZE( m_grData.bBgSceneOffset ) );
	CopyMemory( m_grData.bBgSceneData, m_file.Data<BYTE[ 144 ]>( eptr.Pointer( eBgSceneData ) ), _ARRAYSIZE( m_grData.bBgSceneData ) );
	CopyMemory( m_grData.bBgSceneMTiles, m_file.Data<NES_METATILE[ 36 ]>( eptr.Pointer( eBgSceneMTiles ) ), _ARRAYSIZE( m_grData.bBgSceneMTiles ) );
	CopyMemory( m_grData.bFgSceneOffset, m_file.Data<BYTE[ 3 ]>( eptr.Pointer( eFgSceneDataOffset ) + 1 ), _ARRAYSIZE( m_grData.bFgSceneOffset ) );
	CopyMemory( m_grData.bFgSceneData, m_file.Data<NES_METATILE[ 39 ]>( eptr.Pointer( eFgSceneData ) ), _ARRAYSIZE( m_grData.bFgSceneData ) );
	CopyMemory( m_grData.bGroundMTiles, m_file.Data<NES_METATILE[ 4 ]>( eptr.Pointer( eGroundMTiles ) ), _ARRAYSIZE( m_grData.bGroundMTiles ) );
	CopyMemory( m_grData.bGroundBits, m_file.Data<WORD[ 16 ]>( eptr.Pointer( eGroundBits ) ), _ARRAYSIZE( m_grData.bGroundBits ) * sizeof( WORD ) );
	CopyMemory( m_grData.bBlockBounds, m_file.Data<NES_METATILE[ 4 ]>( eptr.Pointer( eBlockBounds ) ), 4 );

	m_grData.bSpecWaterBlockForWorld = m_file.Data<BYTE>( eptr[ eBlockSpecialWaterWorldId ].ptr );
	m_grData.bSpecWaterBlock = m_file.Data<NES_METATILE>( eptr[ eBlockSpecialWaterWorld8 ].ptr );
	m_grData.bBlockCloud = m_file.Data<NES_METATILE>( eptr[ eBlockCloud ].ptr );
	m_grData.bBlockDefault = m_file.Data<NES_METATILE>( eptr[ eBlockDefault ].ptr );
}

VOID CNesObjectData::LoadBricksMetatiles()
{
	m_brTiles = m_file.Data<NES_BRICKS_METATILES>( m_eptr.Pointer( eBricksMetatiles ) );
}

VOID CNesObjectData::LoadQBlockMetatiles()
{
	m_qbTiles = m_file.Data<NES_QBLOCK_METATILES>( m_eptr.Pointer( eBrickQMetatiles ) );
}

VOID CNesObjectData::LoadSolid()
{
	m_brSolid = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eSolidMetatiles ) );
	m_brCoins = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eCoinsMetatiles ) );
	m_holeTiles = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eHoleMetatiles ) );
}

VOID CNesObjectData::LoadPulleyRopeMetatiles()
{
	m_plRopeTiles = m_file.Data<NES_PULLEY_ROPE_METATILES>( m_eptr.Pointer( ePulleyRopeMetatiles ) );
}

VOID CNesObjectData::LoadPipeMetatiles()
{
	m_pipeTiles = m_file.Data<NES_PIPE_METATILES>( m_eptr.Pointer( eVertPipeMetatiles ) );
	LoadLPipeMetatiles();
}

VOID CNesObjectData::LoadLPipeMetatiles()
{
	m_lpipeMetatiles.SideShaft = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eLPipeShaftMetatiles ) );
	m_lpipeMetatiles.SideBottom = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eLPipeBottomMetatiles ) );
	m_lpipeMetatiles.SideTop = m_file.Data<NES_SOLID_METATILES>( m_eptr.Pointer( eLPipeTopMetatiles ) );
}

VOID CNesObjectData::LoadSpecObjData()
{
	CopyMemory( m_specObj.Y, m_file.Data<BYTE[ 3 ]>( m_eptr.Pointer( eSpecObjY ) + 2 ), _ARRAYSIZE( m_specObj.Y ) );
	CopyMemory( m_specObj.tile, m_file.Data<NES_METATILE[ 3 ]>( m_eptr.Pointer( eSpecObjMetatiles ) + 2 ), _ARRAYSIZE( m_specObj.tile ) );
}

VOID CNesObjectData::LoadCastleMetatiles()
{
	m_castleMetatiles = m_file.Data<NES_CASTLE_METATILES>( m_eptr.Pointer( eCastleMetatiles ) );
}

VOID CNesObjectData::LoadStaircaseData()
{
	CopyMemory( m_staircaseData.Y, m_file.Data<BYTE[ 9 ]>( m_eptr.Pointer( eStaircaseY ) ), _ARRAYSIZE( m_staircaseData.Y ) );
	CopyMemory( m_staircaseData.height, m_file.Data<BYTE[ 9 ]>( m_eptr.Pointer( eStaircaseHeight ) ), _ARRAYSIZE( m_staircaseData.height ) );
}

VOID CNesObjectData::LoadBgColors()
{
	m_bgColors = m_file.Data<NES_BG_COLORS>( m_eptr.Pointer( eBgColors ) );
}

VOID CNesObjectData::DumpBgColors()
{
	m_file.Data<NES_BG_COLORS>( m_eptr.Pointer( eBgColors ) ) = m_bgColors;
}

VOID CNesObjectData::LoadEnemyGFX()
{
	m_gfxEnemy = m_file.Data<NES_GFX_OFFSETS>( m_eptr.Pointer( eGFXEnemyOffsets ) );
}

VOID CNesObjectData::LoadItemTiles()
{
	BYTE bLoBank = 0xFF, bHiBank = 0xFF;

	for ( auto e : g_eTile )
	{
		NES_EPOINTER & ptr = m_eptr[ e ];
		if ( ptr.bHiBank != bHiBank && ptr.bHiBank != 0xFF )
		{
			m_file.SelectPRG( ptr.bHiBank, FALSE );
			bHiBank = ptr.bHiBank;
		}

		if ( ptr.bLoBank != bLoBank && ptr.bLoBank != 0xFF )
		{
			m_file.SelectPRG( ptr.bLoBank, TRUE );
			bLoBank = ptr.bLoBank;
		}

		m_mTile[ e ].bMetatile = m_file.Data<BYTE>( m_eptr[ e ].ptr );
	}
}

VOID CNesObjectData::DumpItemTiles()
{
	BYTE bLoBank = 0xFF, bHiBank = 0xFF;

	for ( auto e : g_eTile )
	{
		NES_EPOINTER & ptr = m_eptr[ e ];
		if ( ptr.bHiBank != bHiBank && ptr.bHiBank != 0xFF )
		{
			m_file.SelectPRG( ptr.bHiBank, FALSE );
			bHiBank = ptr.bHiBank;
		}

		if ( ptr.bLoBank != bLoBank && ptr.bLoBank != 0xFF )
		{
			m_file.SelectPRG( ptr.bLoBank, TRUE );
			bLoBank = ptr.bLoBank;
		}

		m_file.Data<BYTE>( m_eptr[ e ].ptr ) = m_mTile[ e ].bMetatile;
	}
}


VOID CNesObjectData::LoadPlayerPal()
{
	m_nPlayerPal = m_file.Data<NES_PLAYER_PAL>( m_eptr.Pointer( ePlayerColors ) );
}

VOID CNesObjectData::DumpPlayerPal()
{
	m_file.Data<NES_PLAYER_PAL>( m_eptr.Pointer( ePlayerColors ) ) = m_nPlayerPal;
}


const NES_QBLOCK_METATILES & CNesObjectData::QBlockMetatiles() const
{
	return m_qbTiles;
}

const NES_GROUND_DATA & CNesObjectData::GroundData() const
{
	return m_grData;
}

const NES_BRICKS_METATILES & CNesObjectData::BricksMetatiles() const
{
	return m_brTiles;
}

const NES_SOLID_METATILES & CNesObjectData::SolidMetatiles() const
{
	return m_brSolid;
}

const NES_SOLID_METATILES & CNesObjectData::CoinsMetatiles() const
{
	return m_brCoins;
}

const NES_SOLID_METATILES & CNesObjectData::HoleMetatiles() const
{
	return m_holeTiles;
}

const NES_PIPE_METATILES & CNesObjectData::PipeMetatiles() const
{
	return m_pipeTiles;
}

const NES_PULLEY_ROPE_METATILES & CNesObjectData::PulleyRopeMetatiles() const
{
	return m_plRopeTiles;
}

const NES_LPIPE_METATILES & CNesObjectData::LPipeMetatiles() const
{
	return m_lpipeMetatiles;
}

const NES_SPEC_OBJ & CNesObjectData::SpecObjData() const
{
	return m_specObj;
}

const NES_CASTLE_METATILES & CNesObjectData::CastleMetatiles() const
{
	return m_castleMetatiles;
}

const NES_STAIRCASE_DATA & CNesObjectData::StaircaseData() const
{
	return m_staircaseData;
}

const NES_BG_COLORS & CNesObjectData::BgColors() const
{
	return m_bgColors;
}

VOID CNesObjectData::SetBgColors( const NES_BG_COLORS & nbg )
{
	m_bgColors = nbg;
}

const NES_GFX_OFFSETS & CNesObjectData::GFXEnemy() const
{
	return m_gfxEnemy;
}

const NES_PLAYER_PAL & CNesObjectData::PlayerPal() const
{
	return m_nPlayerPal;
}

VOID CNesObjectData::SetPlayerPal( const NES_PLAYER_PAL & npc )
{
	for ( int i = npcMario; i < npcCount; ++i )
	{
		// skip bg col 0 (it was not used actually in a game)
		m_nPlayerPal.bPal[ i ][ 1 ] = npc.bPal[ i ][ 1 ];
		m_nPlayerPal.bPal[ i ][ 2 ] = npc.bPal[ i ][ 2 ];
		m_nPlayerPal.bPal[ i ][ 3 ] = npc.bPal[ i ][ 3 ];
	}
}

NES_METATILE CNesObjectData::Tile( NES_EPOINTERS tileId ) const
{
	const auto & tile = m_mTile.find( tileId );
	if ( tile != m_mTile.end() )
	{
		return tile->second;
	}

	return { 0 };
}