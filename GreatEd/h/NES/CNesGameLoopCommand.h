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

#define NES_LOOP_COMMAND_MINIMUM_PAGE				5
#define NES_LOOP_OWNED_COMMAND_MINIMUM_PAGE			1

#pragma pack(push,1)
typedef struct _tagNES_LOOP_COMMAND
{
	BYTE		bPage;
	BYTE		Y;
	BYTE		bIdInGroup;
	BYTE		bRewindOffset;
	bool		operator==( const _tagNES_LOOP_COMMAND & lp ) const
	{
		return ( lp.bPage == bPage && lp.Y == Y && lp.bIdInGroup == bIdInGroup );
	};
} NES_LOOP_COMMAND, *PNES_LOOP_COMMAND;

typedef enum _tagNES_LEVEL_TYPE : BYTE
{
	nltWater,
	nltGround,
	nltUnderground,
	nltCastle,
	nltTypeCount
} NES_LEVEL_TYPE, *PNES_LEVEL_TYPE;

typedef struct _tagNES_AREA_PTR
{
	union
	{
		BYTE		bPtr;
		struct
		{
			BYTE			bAreaId : 5;
			NES_LEVEL_TYPE	bType : 2;
			BYTE			bUnused : 1;
		};
	};

	bool operator<( const _tagNES_AREA_PTR & x ) const
	{
		return this->bPtr < x.bPtr;
	}

	bool operator==( const _tagNES_AREA_PTR & x ) const
	{
		return this->bPtr == x.bPtr;
	}

} NES_AREA_PTR, *PNES_AREA_PTR;

#pragma pack(pop)

class CNesLevel;
typedef std::map<NES_LEVEL_TYPE, std::map<BYTE, std::vector<NES_LOOP_COMMAND>>> NES_LOOP_LIST;

class CNesGameLoopCommand
{
	CNesSimpleMMC3		& m_file;
	CNesPointers		& m_eptr;

	NES_LOOP_LIST		m_lpList;

public:
	CNesGameLoopCommand( CNesSimpleMMC3 & file, CNesPointers & eptr );

	VOID	LoadLoopList();
	VOID	DumpLoopList();
	VOID	FlushList();
	BOOL	LoopList( CNesLevel * pLevel, std::vector<NES_LOOP_COMMAND> & vlp );
	BOOL	SetLoopList( CNesLevel * pLevel, const std::vector<NES_LOOP_COMMAND> & vlp );
};