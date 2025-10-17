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

typedef struct _tagNES_QBLOCK_METATILES
{
	NES_METATILE			bMetatile[ 14 ];
} NES_QBLOCK_METATILES, *PNES_QBLOCK_METATILES;

typedef struct _tagNES_BRICKS_METATILES
{
	NES_METATILE			bMetatile[ 5 ];
} NES_BRICKS_METATILES, *PNES_BRICKS_METATILES;

typedef struct _tagNES_SOLID_METATILES
{
	NES_METATILE			bMetatile[ 4 ];
} NES_SOLID_METATILES, *PNES_SOLID_METATILES;

typedef struct _tagNES_PIPE_METATILES
{
	NES_METATILE			bMetatile[ 8 ];
} NES_PIPE_METATILES, *PNES_PIPE_METATILES;

typedef struct _tagNES_PULLEY_ROPE_METATILES
{
	NES_METATILE			bMetatile[ 3 ];
} NES_PULLEY_ROPE_METATILES, *PNES_PULLEY_ROPE_METATILES;

typedef struct _tagNES_LPIPE_METATILES
{
	NES_SOLID_METATILES		SideShaft;
	NES_SOLID_METATILES		SideTop;
	NES_SOLID_METATILES		SideBottom;
} NES_LPIPE_METATILES, *PNES_LPIPE_METATILES;

typedef struct _tagNES_SPEC_OBJ
{
	BYTE					Y[ 3 ];
	NES_METATILE			tile[ 3 ];
} NES_SPEC_OBJ, *PNES_SPEC_OBJ;

typedef struct _tagNES_CASTLE_METATILES
{
	NES_METATILE			bMetatiles[ 12 ][ 5 ];
} NES_CASTLE_METATILES, *PNES_CASTLE_METATILES;

typedef struct _tagNES_STAIRCASE_DATA
{
	BYTE					Y[ 9 ];
	BYTE					height[ 9 ];
} NES_STAIRCASE_DATA, *PNES_STAIRCASE_DATA;

typedef struct _tagNES_BG_COLORS
{
	BYTE					bColor[ 8 ];
} NES_BG_COLORS, *PNES_BG_COLORS;

typedef struct _tagNES_GFX_OFFSETS
{
	BYTE					bPtr[ 27 ];
} NES_GFX_OFFSETS, *PNES_GFX_OFFSETS;

typedef std::map<NES_EPOINTERS, NES_METATILE>	NES_ITEM_TILE;

class CNesObjectData
{
	CNesSimpleMMC3 &		m_file;
	const CNesPointers &	m_eptr;
	NES_GROUND_DATA			m_grData;
	NES_QBLOCK_METATILES	m_qbTiles;
	NES_BRICKS_METATILES	m_brTiles;
	NES_SOLID_METATILES		m_brSolid;
	NES_SOLID_METATILES		m_brCoins;
	NES_PIPE_METATILES		m_pipeTiles;
	NES_SOLID_METATILES		m_holeTiles;
	NES_PULLEY_ROPE_METATILES	m_plRopeTiles;
	NES_LPIPE_METATILES		m_lpipeMetatiles;
	NES_SPEC_OBJ			m_specObj;
	NES_CASTLE_METATILES	m_castleMetatiles;
	NES_STAIRCASE_DATA		m_staircaseData;
	NES_BG_COLORS			m_bgColors;
	NES_GFX_OFFSETS			m_gfxEnemy;
	NES_ITEM_TILE			m_mTile;
	NES_PLAYER_PAL			m_nPlayerPal;


	VOID					LoadGroundData();
	VOID					LoadQBlockMetatiles();
	VOID					LoadBricksMetatiles();
	VOID					LoadSolid();
	VOID					LoadPipeMetatiles();
	VOID					LoadPulleyRopeMetatiles();
	VOID					LoadLPipeMetatiles();
	VOID					LoadSpecObjData();
	VOID					LoadCastleMetatiles();
	VOID					LoadStaircaseData();
	VOID					LoadBgColors();
	VOID					DumpBgColors();
	VOID					LoadEnemyGFX();
	VOID					LoadItemTiles();
	VOID					DumpItemTiles();
	VOID					LoadPlayerPal();
	VOID					DumpPlayerPal();

public:
	CNesObjectData( CNesSimpleMMC3 & file, const CNesPointers & eptr );

	VOID					LoadData();
	VOID					DumpData();

	// objects interface
	const NES_GROUND_DATA &	GroundData() const;
	const NES_QBLOCK_METATILES & QBlockMetatiles() const;
	const NES_BRICKS_METATILES & BricksMetatiles() const;
	const NES_SOLID_METATILES & SolidMetatiles() const;
	const NES_SOLID_METATILES & CoinsMetatiles() const;
	const NES_SOLID_METATILES & HoleMetatiles() const;
	const NES_PIPE_METATILES & PipeMetatiles() const;
	const NES_PULLEY_ROPE_METATILES & PulleyRopeMetatiles() const;
	const NES_LPIPE_METATILES & LPipeMetatiles() const;
	const NES_SPEC_OBJ & SpecObjData() const;
	const NES_CASTLE_METATILES & CastleMetatiles() const;
	const NES_STAIRCASE_DATA & StaircaseData() const;
	const NES_BG_COLORS & BgColors() const;
	VOID SetBgColors( const NES_BG_COLORS & nbg );
	const NES_GFX_OFFSETS & GFXEnemy() const;
	const NES_PLAYER_PAL & PlayerPal() const;
	VOID SetPlayerPal( const NES_PLAYER_PAL & npc );
	NES_METATILE Tile( NES_EPOINTERS tileId ) const;
};