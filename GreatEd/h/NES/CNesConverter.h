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

#define	NES_L_LEVEL_BANKS_PTR				1
#define NES_LENGTH_OF_QBRICKS_METATILES		14
#define NES_LENGTH_OF_ENEMY_ADDR_H_OFFSET	4
#define NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET	4

#define NES_BANK_VARIABLE					0x7FB

class CNesConverter
{
	CNesNROM			&		m_nrom;
	CNesSimpleMMC3		&		m_mmc3;
	const CNesPointers		&	m_eptr;
	std::map<USHORT, USHORT>	m_mLocalPointers;
	CNesAssembly				m_aSharedCode;

	VOID				ConvertROM();

	VOID				Patch( NES_EPOINTERS eptr, const PBYTE pData, USHORT len );
	VOID				PatchCode( USHORT ptr, const PBYTE pData, USHORT len );
	VOID				PatchLoop();
	VOID				PatchHalfPage();
	VOID				WriteCodePatch();
	VOID				MoveMusicData();
	VOID				MoveAreaData();
	VOID				MoveQBricks( USHORT uPtrTo );
	VOID				MoveDemoData();
	VOID				ReadAreaData(
		USHORT uPtrLo,
		USHORT uPtrHi,
		USHORT uCount,
		BYTE bEOFMarker,
		std::map<USHORT, std::vector<BYTE>> & mData,
		std::vector<USHORT> & vDataVector
	);

	USHORT				WriteAreaData(
		USHORT uPtrLo,
		USHORT uPtrHi,
		USHORT uBeginPtr,
		const std::map<USHORT, std::vector<BYTE>> & mData,
		const std::vector<USHORT> & vDataVector
	);

	USHORT				WriteBanksArray( 
		USHORT c, 
		USHORT uPtr, 
		BYTE bBankId 
	);

public:
	CNesConverter( CNesNROM & nrom, CNesSimpleMMC3 & mmc3, const CNesPointers & eptr );

	VOID				Convert();
};