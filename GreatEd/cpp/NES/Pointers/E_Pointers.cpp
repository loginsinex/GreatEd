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

VOID CNesPointers::LoadPointersE()
{
	m_eptr[ eMusicPtr1 ].eptr = NES_E_MUSIC_PTR_1;
	m_eptr[ eMusicPtr2 ].eptr = NES_E_MUSIC_PTR_2;
	m_eptr[ eMusicPtr3 ].eptr = NES_E_MUSIC_PTR_3;
	m_eptr[ eMusicPtr4 ].eptr = NES_E_MUSIC_PTR_4;
	m_eptr[ eMusicPtr5 ].eptr = NES_E_MUSIC_PTR_5;
	m_eptr[ eMusicPtr6 ].eptr = NES_E_MUSIC_PTR_6;
	m_eptr[ eMusicPtr7 ].eptr = NES_E_MUSIC_PTR_7;
	m_eptr[ eMusicFreqPtr1 ].eptr = NES_E_MUSIC_FREQ_PTR_1;
	m_eptr[ eMusicFreqPtr2 ].eptr = NES_E_MUSIC_FREQ_PTR_2;
	m_eptr[ eMusicLenPtr ].eptr = NES_E_MUSIC_LEN_PTR;
	m_eptr[ eMusicEnvCastlePtr ].eptr = NES_E_MUSIC_ENV_CASTLE_PTR;
	m_eptr[ eMusicEnvAreaPtr ].eptr = NES_E_MUSIC_ENV_AREA_PTR;
	m_eptr[ eMusicEnvWaterPtr ].eptr = NES_E_MUSIC_ENV_WATER_PTR;
	m_eptr[ eMusicNoiseFlamePtr ].eptr = NES_E_MUSIC_NOISE_FLAME_PTR;
	m_eptr[ eMusicEnvBrickPtr ].eptr = NES_E_MUSIC_ENV_BRICK_PTR;
	m_eptr[ eEOFMusicData ].eptr = NES_E_EOF_MUSIC_DATA;
	m_eptr[ eRESETEndlessJmp ].eptr = NES_E_RESET_ENDLESS_JMP;
	m_eptr[ eSoundEnginePtr ].eptr = NES_E_SOUND_ENGINE_PTR;
	m_eptr[ eGetAreaAddressesPtr ].eptr = NES_E_GET_AREA_ADDRESSES;
	m_eptr[ eProcessAreaData1Ptr ].eptr = NES_E_PROCESS_AREA_DATA_1;
	m_eptr[ eProcessAreaData2Ptr ].eptr = NES_E_PROCESS_AREA_DATA_2;
	m_eptr[ eEnemiesAndLoopsCorePtr ].eptr = NES_E_ENEMY_CORE;
	m_eptr[ eEnemyPtrLo ].eptr = NES_E_ENEMY_PTR_LO;
	m_eptr[ eEnemyPtrHi ].eptr = NES_E_ENEMY_PTR_HI;
	m_eptr[ eLevelPtrLo ].eptr = NES_E_LEVEL_PTR_LO;
	m_eptr[ eLevelPtrHi ].eptr = NES_E_LEVEL_PTR_HI;
	m_eptr[ eEnemyAddrHOffset ].eptr = NES_E_ENEMY_ADDR_H_OFFSET;
	m_eptr[ eLevelAddrHOffset ].eptr = NES_E_LEVEL_ADDR_H_OFFSET;
	m_eptr[ eBrickQMetatiles ].eptr = NES_E_BRICK_Q_METATILES;
	m_eptr[ eWorldAreasOffsets ].eptr = NES_E_WORLD_AREAS_OFFSETS;
	m_eptr[ eWorldAreas ].eptr = NES_E_WORLD_AREAS;
	m_eptr[ eFindAreaPointer ].eptr = NES_E_FIND_AREA_POINTER;
	m_eptr[ ePwrUpJmpPtr ].eptr = NES_E_SBR_PWRUPJMP;
	m_eptr[ eSetupVinePtr ].eptr = NES_E_SBR_SETUP_VINE;
	m_eptr[ eMetatiesPtrHi ].eptr = NES_E_METATILES_PTR_HI;
	m_eptr[ eMetatiesPtrLo ].eptr = NES_E_METATILES_PTR_LO;
	m_eptr[ eVRAMAddrTableHi ].eptr = NES_E_VRAM_TBL_PTR_HI;
	m_eptr[ eVRAMAddrTableLo ].eptr = NES_E_VRAM_TBL_PTR_LO;

	m_eptr[ eBgSceneDataOffset ].eptr = NES_E_BG_SCENE_DATA_OFFSET;
	m_eptr[ eBgSceneData ].eptr = NES_E_BG_SCENERY_DATA;
	m_eptr[ eBgSceneMTiles ].eptr = NES_E_BG_SCENERY_MTILES;
	m_eptr[ eFgSceneDataOffset ].eptr = NES_E_FG_SCENE_DATA_OFFSET;
	m_eptr[ eFgSceneData ].eptr = NES_E_FG_SCENERY_DATA;
	m_eptr[ eGroundMTiles ].eptr = NES_E_GROUND_MTILES;
	m_eptr[ eGroundBits ].eptr = NES_E_GROUND_BITS;
	m_eptr[ eBlockBounds ].eptr = NES_E_BLOCK_BOUNDS;

	m_eptr[ eBlockSpecialWaterWorldId ].eptr = NES_E_BLOCK_W_WORLD_ID;
	m_eptr[ eBlockSpecialWaterWorld8 ].eptr = NES_E_BLOCK_W_WORLD_8;
	m_eptr[ eBlockCloud ].eptr = NES_E_BLOCK_CLOUD;
	m_eptr[ eBlockDefault ].eptr = NES_E_BLOCK_DEFAULT;

	m_eptr[ eBricksMetatiles ].eptr = NES_E_BRICKS_METATILES;
	m_eptr[ eSolidMetatiles ].eptr = NES_E_SOLID_METATILES;
	m_eptr[ eCoinsMetatiles ].eptr = NES_E_COINS_METATILES;
	m_eptr[ eVertPipeMetatiles ].eptr = NES_E_PIPE_METATILES;
	m_eptr[ eHoleMetatiles ].eptr = NES_E_HOLE_METATILES;
	m_eptr[ ePulleyRopeMetatiles ].eptr = NES_E_PUL_ROPE_METATILES;
	m_eptr[ eLPipeShaftMetatiles ].eptr = NES_E_LPIPE_SHFT_METATILES;
	m_eptr[ eLPipeTopMetatiles ].eptr = NES_E_LPIPE_TOP_METATILES;
	m_eptr[ eLPipeBottomMetatiles ].eptr = NES_E_LPIPE_BTM_METATILES;

	m_eptr[ eSpecObjY ].eptr = NES_E_SPEC_OBJ_Y;
	m_eptr[ eSpecObjMetatiles ].eptr = NES_E_SPEC_OBJ_METATILES;
	m_eptr[ eCastleMetatiles ].eptr = NES_E_CASTLE_METATILES;

	m_eptr[ eStaircaseY ].eptr = NES_E_STAIRCASE_Y;
	m_eptr[ eStaircaseHeight ].eptr = NES_E_STAIRCASE_HEIGHT;
	m_eptr[ eBgColors ].eptr = NES_E_BG_COLORS;
	m_eptr[ eGFXEnemy ].eptr = NES_E_GFX_ENEMY;
	m_eptr[ eGFXEnemyOffsets ].eptr = NES_E_GFX_ENEMY_OFFSETS;

	m_eptr[ eGFXPowerUp ].eptr = NES_E_GFX_POWERUP;

	m_eptr[ eLoopPages ].eptr = NES_E_LOOP_PAGES;
	m_eptr[ eLoopY ].eptr = NES_E_LOOP_Y;
	m_eptr[ eLoopHandler ].eptr = NES_E_LOOP_HANDLER_PTR;
	m_eptr[ eLoopRewindOffsets ].eptr = NES_E_LOOP_REW_OFFSETS_PTR;
	m_eptr[ eLoopCounterIncCode ].eptr = NES_E_LOOP_COUNTER_INC_CODE;
	m_eptr[ eLoopCode ].eptr = NES_E_LOOP_CODE;
	m_eptr[ eLoopAreaOfsCode ].eptr = NES_E_LOOP_AREA_OFS_CODE;
	m_eptr[ eExecLoopBack ].eptr = NES_E_EXEC_LOOP_BACK;
	m_eptr[ eKillAllEnemies ].eptr = NES_E_KILL_ALL_ENEMIES;
	m_eptr[ ePlayerSpriteTable ].eptr = NES_E_PLAYER_SPRITE_TABLE;
	m_eptr[ ePlayerTiles ].eptr = NES_E_PLAYER_SPRITE_TILES;
	m_eptr[ eHalfPagesCode ].eptr = NES_E_HALF_PAGES_CODE;
	m_eptr[ eHalfPagesData ].eptr = NES_E_HALF_PAGES_PTR;

	m_eptr[ eDemoActionArray ].eptr = NES_E_DEMO_ACTION_ARRAY;
	m_eptr[ eDemoTimingArray ].eptr = NES_E_DEMO_TIMING_ARRAY;

	m_eptr[ ePlayerEntranceCtrl ].eptr = NES_E_PLAYER_ENTRANCE_CTRL;
	m_eptr[ ePlayerEntranceX ].eptr = NES_E_PLAYER_ENTRANCE_X;
	m_eptr[ ePlayerTimerCtrl ].eptr = NES_E_PLAYER_TIMER_CTRL;
	m_eptr[ eCoinsFor1Up ].eptr = NES_E_COINS_FOR_1UP;
	m_eptr[ eHardModeWorld ].eptr = NES_E_HARD_MODE_WORLD;
	m_eptr[ eHardModeLevel ].eptr = NES_E_HARD_MODE_LEVEL;
	m_eptr[ eWarpWorlds ].eptr = NES_E_WARP_ZONE_WORLDS;
	m_eptr[ eBowserHammersWorld ].eptr = NES_E_BOWSER_HAMMERS;
	m_eptr[ ePalRotateColors ].eptr = NES_E_PAL_ROTATE_COL_PTR;

	m_eptr[ eSpinyEggPatchPlace ].eptr = NES_E_SPINY_EGG_PATCH_PLACE;
	m_eptr[ eSmallBBox ].eptr = NES_E_SMALLBBOX;

	m_eptr[ eBowserIdentities ].eptr = NES_E_BOWSER_IDENTITIES;
	m_eptr[ eBowserHits ].eptr = NES_E_BOWSER_HITS;

	m_eptr[ eTileListBricksRowTileId ].eptr = NES_E_TILE_BRICK_ROW_BLOCK_ID;
	m_eptr[ eTileListBridgeTile1 ].eptr = NES_E_TILE_BRIDGE_TILE1;
	m_eptr[ eTileListBridgeTile2 ].eptr = NES_E_TILE_BRIDGE_TILE2;
	m_eptr[ eTileListBridgeHeight0 ].eptr = NES_E_TILE_BRIDGE_HEIGHT0;
	m_eptr[ eTileListBridgeHeight1 ].eptr = NES_E_TILE_BRIDGE_HEIGHT1;
	m_eptr[ eTileListBridgeHeight2 ].eptr = NES_E_TILE_BRIDGE_HEIGHT2;
	m_eptr[ eTileListWaterTile1 ].eptr = NES_E_TILE_WATER_TILE1;
	m_eptr[ eTileListWaterTile2 ].eptr = NES_E_TILE_WATER_TILE2;
	m_eptr[ eTileListQBlockRow ].eptr = NES_E_TILE_QBLOCK_ROW;
	m_eptr[ eTileListQBlockHeight1 ].eptr = NES_E_TILE_QBLOCK_ROW_HEIGHT1;
	m_eptr[ eTileListQBlockHeight2 ].eptr = NES_E_TILE_QBLOCK_ROW_HEIGHT2;
	m_eptr[ eTileListRope ].eptr = NES_E_TILE_ROPE;
	m_eptr[ eTileListResidualBall ].eptr = NES_E_TILE_RESIDUAL;
	m_eptr[ eTileListStaircase ].eptr = NES_E_TILE_STAIRCASE;
	m_eptr[ eTileListTreeLedgeLeft ].eptr = NES_E_TILE_TREE_LEDGE_LEFT;
	m_eptr[ eTileListTreeLedgeMiddle ].eptr = NES_E_TILE_TREE_LEDGE_MDL;
	m_eptr[ eTileListTreeLedgeRight ].eptr = NES_E_TILE_TREE_LEDGE_RIGHT;
	m_eptr[ eTileListTreeLedgeBottom ].eptr = NES_E_TILE_TREE_LEDGE_BTM;
	m_eptr[ eTileListMushroomLedgeLeft ].eptr = NES_E_TILE_MUSH_LEDGE_LEFT;
	m_eptr[ eTileListMushroomLedgeRight ].eptr = NES_E_TILE_MUSH_LEDGE_RIGHT;
	m_eptr[ eTileListMushroomLedgeMiddle ].eptr = NES_E_TILE_MUSH_LEDGE_MDL;
	m_eptr[ eTileListMushroomLedgeTop ].eptr = NES_E_TILE_MUSH_LEDGE_TOP;
	m_eptr[ eTileListMushroomLedgeBottom ].eptr = NES_E_TILE_MUSH_LEDGE_BTM;
	m_eptr[ eTileListBulletBillTop ].eptr = NES_E_TILE_BULL_BILL_TOP;
	m_eptr[ eTileListBulletBillMiddle ].eptr = NES_E_TILE_BULL_BILL_MDL;
	m_eptr[ eTileListBulletBillBottom ].eptr = NES_I_TILE_BULL_BILL_BTM;
	m_eptr[ eTileListWaterPipeBottom ].eptr = NES_E_TILE_WATER_PIPE_BTM;
	m_eptr[ eTileListWaterPipeTop ].eptr = NES_E_TILE_WATER_PIPE_TOP;
	m_eptr[ eTileListJumpspringTop ].eptr = NES_E_TILE_JUMPSPRING_TOP;
	m_eptr[ eTileListJumpspringBottom ].eptr = NES_E_TILE_JUMPSPRING_BTM;
	m_eptr[ eTileListFlagpoleTop ].eptr = NES_E_TILE_FLAGPOLE_TOP;
	m_eptr[ eTileListFlagpoleRope ].eptr = NES_E_TILE_FLAGPOLE_ROPE;
	m_eptr[ eTileListFlagpoleBottom ].eptr = NES_E_TILE_FLAGPOLE_BOTTOM;
	m_eptr[ eTileListStoneBlock ].eptr = NES_E_TILE_STONE_BLOCK;
	m_eptr[ ePatchPal ].eptr = NES_E_PATCH_PAL;
	m_eptr[ eBlankPal ].eptr = NES_E_BLANK_PALETTE;
	m_eptr[ eStyleVRAMTables ].eptr = NES_E_STYLE_VRAM_TABLES;
	m_eptr[ ePlayerColors ].eptr = NES_E_PLAYER_COLORS;

	m_eptr[ eTitleScreenCHRPtrHi ].eptr = NES_E_CHR_TITLE_DATA_PTR_HI;
	m_eptr[ eTitleScreenCHRPtrLo ].eptr = NES_E_CHR_TITLE_DATA_PTR_LO;

	m_eptr[ eTitleScreenStartPtrHi ].eptr = NES_E_TITLE_DATA_START_PTR_HI;
	m_eptr[ eTitleScreenStartPtrLo ].eptr = NES_E_TITLE_DATA_START_PTR_LO;
	m_eptr[ eTitleScreenEndPtrHi ].eptr = NES_E_TITLE_DATA_END_PTR_HI;
	m_eptr[ eTitleScreenEndPtrLo ].eptr = NES_E_TITLE_DATA_END_PTR_LO;
	m_eptr[ eInitGameSubPtr ].eptr = NES_E_INIT_GAME_POINTER;
}
