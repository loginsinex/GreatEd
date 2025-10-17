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

#include "h\NES\NES.h"

#include "Pointers/E_Pointers.h"
#include "Pointers/I_Pointers.h"

#define			NES_S_MUSIC_PTR				0x02FF8000
#define			NES_S_SHARED_CODE_PTR		0xFFFFFEB2 // 0xFFFFFF08
#define			NES_S_SAVE_AREA_PTR			0xFFFFFEB2
#define			NES_S_LOOP_HANDLER_PTR		0xFFFFFEBB
#define			NES_S_NMI					0xFFFFFFFA
#define			NES_S_RESET					0xFFFFFFFC
#define			NES_S_IRQ					0xFFFFFFFE
#define			NES_S_MMC3_NMI				0xFFFFFFE2
#define			NES_S_MMC3_RESET			0xFFFFFFE5
#define			NES_S_MMC3_IRQ				0xFFFFFFF9
#define			NES_S_SOUND_ENGINE_PTR		0xFFFFFF9C
#define			NES_S_RESET_ENDLESS_JMP		0xFFFFFFF6
#define			NES_S_ALT_GET_AREA_ADDR		0xFFFFFF72
#define			NES_S_ALT_PROCESS_AREA_DATA	0xFFFFFF80
#define			NES_S_ALT_ENEMY_CORE		0xFFFFFF8e
#define			NES_S_ALT_FIND_AREA_POINTER	0xFFFFFF58
#define			NES_S_ENEMY_PTR_LO			0x0004A000	// other addresses will be calculated automaticaly
#define			NES_S_SBR_PWRUPJMP			0xFFFFFF08
#define			NES_S_SBR_SETUP_VINE		0xFFFFFF2D
#define			NES_S_AREA_BANKS			0xFFFFFF6B
#define			NES_S_AREA_HALFPAGES		0x0003A000
#define			NES_S_LOOP_COMMANDS			0x0003A108
#define			NES_S_DEMO_DATA				0x0001A000

typedef union _tagNES_EPOINTER
{
	struct
	{
		USHORT		ptr;
		BYTE		bLoBank;
		BYTE		bHiBank;
	};
	DWORD			eptr;
} NES_EPOINTER, *PNES_EPOINTER;

typedef enum _tagNES_VERSION : TCHAR
{
	ver_E = _T('E'),
	ver_I = _T('U'),
	ver_X = _T('G'),
	ver_Unknown = _T('0')
} NES_VERSION, *PNES_VERSION;

typedef enum _tagNES_EPOINTERS
{
	eMusicPtr1,
	eMusicPtr2,
	eMusicPtr3,
	eMusicPtr4,
	eMusicPtr5,
	eMusicPtr6,
	eMusicPtr7,
	eMusicFreqPtr1,
	eMusicFreqPtr2,
	eMusicLenPtr,
	eMusicEnvCastlePtr,
	eMusicEnvAreaPtr,
	eMusicEnvWaterPtr,
	eMusicNoiseFlamePtr,
	eMusicEnvBrickPtr,
	eGetAreaAddressesPtr,
	eProcessAreaData1Ptr,
	eProcessAreaData2Ptr,
	eEnemiesAndLoopsCorePtr,
	eSoundEnginePtr,
	eEOFMusicData,
	eRESETEndlessJmp,
	eEnemyPtrLo,
	eEnemyPtrHi,
	eLevelPtrLo,
	eLevelPtrHi,
	eEnemyAddrHOffset,
	eLevelAddrHOffset,
	eBrickQMetatiles,
	eWorldAreasOffsets,
	eWorldAreas,
	eFindAreaPointer,
	eSetupVinePtr,
	ePwrUpJmpPtr,
	eMetatiesPtrHi,
	eMetatiesPtrLo,
	eVRAMAddrTableHi,
	eVRAMAddrTableLo,
	eBgSceneDataOffset,
	eBgSceneData,
	eBgSceneMTiles,
	eFgSceneDataOffset,
	eFgSceneData,
	eGroundMTiles,
	eGroundBits,
	eBlockSpecialWaterWorldId,
	eBlockSpecialWaterWorld8,
	eBlockCloud,
	eBlockDefault,
	eBlockBounds,
	eBricksMetatiles,
	eSolidMetatiles,
	eCoinsMetatiles,
	eVertPipeMetatiles,
	eHoleMetatiles,
	ePulleyRopeMetatiles,
	eLPipeShaftMetatiles,
	eLPipeBottomMetatiles,
	eLPipeTopMetatiles,
	eCastleMetatiles,
	eStaircaseY,
	eStaircaseHeight,
	eSpecObjY,
	eSpecObjMetatiles,
	eBgColors,
	eGFXEnemy,
	eGFXEnemyOffsets,
	eGFXPowerUp,
	eLoopPages,
	eLoopY,
	eLoopHandler,
	eLoopRewindOffsets,
	eLoopCounterIncCode,
	eLoopCode,
	eLoopAreaOfsCode,
	eExecLoopBack,
	eKillAllEnemies,
	ePlayerSpriteTable,
	ePlayerTiles,
	eHalfPagesCode,
	eHalfPagesData,
	eDemoActionArray,
	eDemoTimingArray,
	ePlayerEntranceCtrl,
	ePlayerEntranceX,
	ePlayerTimerCtrl,
	eCoinsFor1Up,
	eHardModeWorld,
	eHardModeLevel,
	eWarpWorlds,
	eBowserHammersWorld,
	ePalRotateColors,
	eSpinyEggPatchPlace,
	eSmallBBox,
	eBowserIdentities,
	eBowserHits,
	ePatchPal,
	ePlayerColors,

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
	eTileListStoneBlock,

	eBlankPal,
	eStyleVRAMTables,
	eInitGameSubPtr,

	eSharedMusicPtr,
	//eSharedCodePtr,
	eSharedNMIPtr,
	eSharedRESETPtr,
	eSharedIRQPtr,
	eSharedMMC3NMI,
	eSharedMMC3RESET,
	eSharedMMC3IRQ,
	eSharedSoundEnginePtr,
	eSharedRESETEndlessJmp,
//	eSharedAltGetAreaAddr,
//	eSharedAltProcessAreaData,
//	eSharedAltEnemyCore,
//	eSharedAltFindAreaPointer,
	eSharedAltGetAreaAddresses,
	eSharedSaveAreaPtr,
	eSharedEnemyPtrLo,
	eSharedAltSetupVinePtr,
	eSharedAltPwrUpJmpPtr,
//	eSharedAreaBanks,
	eSharedLoopCommands,
	eSharedLoopHandler,
	eSharedHalfPages,
	eSharedDemoData,
	eTitleScreenCHRPtrHi,
	eTitleScreenCHRPtrLo,
	eTitleScreenStartPtrHi,
	eTitleScreenStartPtrLo,
	eTitleScreenEndPtrHi,
	eTitleScreenEndPtrLo
} NES_EPOINTERS, *PNES_EPOINTERS;

class CNesPointers
{
	CNesSimpleMMC3						&		m_file;
	NES_VERSION									m_uVersion;
	std::map<NES_EPOINTERS, NES_EPOINTER>		m_eptr;

	VOID			LoadPointers();
	VOID			LoadPointersE();
	VOID			LoadPointersI();
	VOID			LoadPointersX();

	VOID			LoadSharedPointers();

public:
	CNesPointers( CNesSimpleMMC3 & mmc3 );
	VOID			Version( NES_VERSION id );

	NES_EPOINTER	operator[]( NES_EPOINTERS eptr ) const;

	VOID			Select( NES_EPOINTERS eptr ) const;
	USHORT	&		Pointer( NES_EPOINTERS eptr ) const;
};