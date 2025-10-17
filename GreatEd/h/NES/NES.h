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

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// reference additional headers your program requires here
extern void * __cdecl operator new( size_t size );
extern void __cdecl operator delete( void *ptr );
extern void __cdecl operator delete[]( void *ptr );

#include "h\GUI\Wrapper\Controls.h"

extern const COLORREF g_crNesColor[];

#define			NES_ATTR_COUNT			4
#define			NES_TILES_COUNT			256
#define			NES_TILE_WIDTH			8
#define			NES_TILE_HEIGHT			8
#define			NES_PAL_COLORS			16

#pragma pack(push,1)
typedef struct _tagNES_TILE
{
	BYTE pix[NES_TILE_HEIGHT][NES_TILE_WIDTH];
} NES_TILE, *PNES_TILE;

typedef struct _tagNES_PPU_PALETTE
{
	BYTE			bPal[NES_PAL_COLORS];
} NES_PPU_PALETTE, *PNES_PPU_PALETTE;
#pragma pack(pop)

typedef enum _tagNES_PLAYER_COL_T
{
	npcMario,
	npcLuigi,
	npcFire,
	npcCount
} NES_PLAYER_COL_T, *PNES_PLAYER_COL_T;

typedef struct _tagNES_PLAYER_PAL
{
	BYTE					bPal[ npcCount ][ 4 ];
} NES_PLAYER_PAL, *PNES_PLAYER_PAL;

#include "h\NES\CNesBitmap.h"
#include "h\NES\CNesFile.h"
extern const CNesColors g_hbmColor;

#include "h\NES\Assembly\CNesAssembly.h"
#include "h\NES\CNesPointers.h"
#include "h\NES\CNesLeveleader.h"
#include "h\NES\CNesGameEngineHack.h"
#include "h\NES\CNesGameLoopCommand.h"
#include "h\NES\CNesDemoPlay.h"
#include "h\NES\CNesPlayerEntrance.h"
#include "h\NES\CNesTitleScreen.h"
#include "h\NES\NesObjects\CNesTileCache.h"
#include "h\NES\NesObjects\CNesMetatiles.h"
#include "h\NES\NesObjects\CNesLevelView.h"
#include "h\NES\NesObjects\CNesGameLinks.h"
#include "h\NES\NesObjects\NesLevelItems\GroundDefinition.h"
#include "h\NES\CNesDrawQueue.h"
#include "h\NES\NesObjects\NesEnemyItems\ComplexEnemyObjects.h"
#include "h\NES\NesObjects\NesEnemyItems\SimpleEnemyItems.h"
#include "h\NES\NesObjects\NesLevelItems\MassiveObjects.h"
#include "h\NES\NesObjects\NesLevelItems\LongObjects.h"
#include "h\NES\NesObjects\NesLevelItems\SingleBlockObjects.h"
#include "h\NES\NesObjects\NesLevelItems\CNesLevelItemGroundType.h"
#include "h\NES\CNesConverter.h"
#include "h\NES\CNesLevel.h"
#include "h\NES\CNesLevelInterface.h"
#include "h\NES\CNesCanvas.h"
#include "h\NES\CNesLevelsKeeper.h"
#include "h\NES\CNesEditor.h"
#include "h\NES\CNesCanvas_Win32Control.h"
#include "h\NES\CNesObjectsNames.h"