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

CNesConverter::CNesConverter( CNesNROM & nrom, CNesSimpleMMC3 & mmc3, const CNesPointers & eptr )
	: m_nrom( nrom ), m_mmc3( mmc3 ), m_eptr( eptr )
{

}

VOID CNesConverter::Convert()
{
	std::vector<std::vector<BYTE>> vPRG, vNROMPRG;
	std::vector<BYTE> vCHR, vNROMCHR, vPRGBank( NES_PRG_SIZE, 0 );
	
	m_nrom.Get_PRG( vNROMPRG );
	m_nrom.Get_CHR( vNROMCHR );
	vPRG.push_back( vNROMPRG[ 0 ] );
	vPRG.insert( vPRG.end(), ( 0x08 - 0x02 ), vPRGBank );
	vPRG.push_back( vNROMPRG[ 1 ] );
	vCHR = vNROMCHR;

	// fill target chr with duplicates of source chr
	for ( int n = 1; n < 16; ++n )
	{
		vCHR.insert( vCHR.end(), vNROMCHR.begin(), vNROMCHR.end() );
	}

	if ( !m_mmc3.CreateNew( m_nrom, TRUE, vPRG, vCHR ) )
	{
		m_mmc3.FreeFile();
		throw std::exception( "Converter NROM->MMC3 failed" );
	}
	ConvertROM();
}

VOID CNesConverter::ConvertROM()
{
	MoveMusicData();
	MoveAreaData();
	MoveDemoData();
	WriteCodePatch();
	PatchLoop();
	PatchHalfPage();	
}

VOID CNesConverter::Patch( NES_EPOINTERS eptr, const PBYTE pData, USHORT len )
{
	NES_EPOINTER ptr = m_eptr[ eptr ];
	m_eptr.Select( eptr );
	PatchCode( ptr.ptr, pData, len );
}

VOID CNesConverter::PatchCode( USHORT ptr, const PBYTE pData, USHORT len )
{
	for ( USHORT i = 0; i < len; ++i )
	{
		m_mmc3.Data<BYTE>( ptr + i ) = pData[ i ];
	}
}

VOID CNesConverter::ReadAreaData( 
	USHORT uPtrLo, 
	USHORT uPtrHi, 
	USHORT uCount, 
	BYTE bEOFMarker, 
	std::map<USHORT, std::vector<BYTE>>& mData, 
	std::vector<USHORT>& vDataVector 
)
{
	for ( USHORT i = 0; i < uCount; ++i )
	{
		USHORT uPtr = MAKEWORD( m_mmc3.Data<BYTE>( uPtrLo + i ), m_mmc3.Data<BYTE>( uPtrHi + i ) );
		if ( mData.find( uPtr ) == mData.end() )
		{
			USHORT uDataPtr = uPtr;
			std::vector<BYTE> & vData = mData[ uPtr ];
			BYTE bData = 0;
			do
			{
				bData = m_mmc3.Data<BYTE>( uDataPtr );
				vData.push_back( bData );
				m_mmc3.Data<BYTE>( uDataPtr++ ) = bEOFMarker;
			} while ( bData != bEOFMarker );
		}
		vDataVector.push_back( uPtr );
	}
}

USHORT CNesConverter::WriteAreaData( 
	USHORT uPtrLo, 
	USHORT uPtrHi, 
	USHORT uBeginPtr, 
	const std::map<USHORT, std::vector<BYTE>>& mData, 
	const std::vector<USHORT>& vDataVector 
)
{
	USHORT c = LOWORD( vDataVector.size() );
	USHORT uDataPtr = uBeginPtr, uLo = uPtrLo, uHi = uPtrHi;
	std::map<USHORT, USHORT> mDataMap;

	for ( USHORT i = 0; i < c; ++i )
	{
		USHORT uPtr = vDataVector[ i ];
		auto v = mData.find( uPtr );
		if ( v == mData.end() )
		{
			throw std::exception( "Move area data failed" );
		}

		if ( mDataMap.find( uPtr ) == mDataMap.end() )
		{
			const auto & vData = v->second;
			USHORT cData = LOWORD( vData.size() );
			mDataMap[ uPtr ] = uDataPtr;
			for ( USHORT n = 0; n < cData; ++n )
			{
				m_mmc3.Data<BYTE>( uDataPtr++ ) = vData[ n ];
			}
		}

		m_mmc3.Data<BYTE>( uLo++ ) = LOBYTE( mDataMap[ uPtr ] );
		m_mmc3.Data<BYTE>( uHi++ ) = HIBYTE( mDataMap[ uPtr ] );
	}

	return uDataPtr;
}

VOID CNesConverter::MoveQBricks( USHORT uPtrTo )
{
	USHORT & uQBricksPtr = m_eptr.Pointer( eBrickQMetatiles );
	CopyMemory(
		m_mmc3.Data<BYTE[ NES_LENGTH_OF_QBRICKS_METATILES ]>( uPtrTo ),
		m_mmc3.Data<BYTE[ NES_LENGTH_OF_QBRICKS_METATILES ]>( uQBricksPtr ),
		NES_LENGTH_OF_QBRICKS_METATILES );
	uQBricksPtr = uPtrTo;
}

VOID CNesConverter::MoveDemoData()
{
	USHORT uNewPtr = m_eptr[ eSharedDemoData ].ptr;
	USHORT & uDemoActionPtr = m_eptr.Pointer( eDemoActionArray );
	USHORT & uDemoTimingPtr = m_eptr.Pointer( eDemoTimingArray );
	USHORT uNROMActionPtr = uDemoActionPtr + 1, uNROMTimingPtr = uDemoTimingPtr + 1;
	USHORT uMMC3TimingPtr = uNewPtr, uMMC3ActionPtr = 0;
	std::vector<BYTE> vActionData, vTimingData;
	BYTE b = 0;

	m_mmc3.SelectPRG( m_eptr[ eSharedDemoData ].bLoBank, TRUE );
	uDemoTimingPtr = uMMC3TimingPtr - 1;
	while ( b = m_mmc3.Data<BYTE>( uNROMTimingPtr++ ) )
	{
		m_mmc3.Data<BYTE>( uMMC3TimingPtr++ ) = b;
	}
	m_mmc3.Data<BYTE>( uMMC3TimingPtr++ ) = 0;

	uMMC3ActionPtr = uMMC3TimingPtr;
	uDemoActionPtr = uMMC3ActionPtr - 1;
	while ( b = m_mmc3.Data<BYTE>( uNROMActionPtr++ ) )
	{
		m_mmc3.Data<BYTE>( uMMC3ActionPtr++ ) = b;
	}
	m_mmc3.Data<BYTE>( uMMC3ActionPtr++ ) = 0;
}

USHORT CNesConverter::WriteBanksArray( USHORT c, USHORT uPtr, BYTE bBankId )
{
	for ( USHORT i = 0; i < c; ++i )
	{
		m_mmc3.Data<BYTE>( uPtr++ ) = bBankId;
	}
	return uPtr;
}

VOID CNesConverter::MoveAreaData()
{
	USHORT uMMC3Ptr = m_eptr[ eSharedEnemyPtrLo ].ptr;
	USHORT & uEnemyPtrLo = m_eptr.Pointer( eEnemyPtrLo );
	USHORT & uEnemyPtrHi = m_eptr.Pointer( eEnemyPtrHi );
	USHORT & uLevelPtrLo = m_eptr.Pointer( eLevelPtrLo );
	USHORT & uLevelPtrHi = m_eptr.Pointer( eLevelPtrHi );
	USHORT & uEnemyAddrHOffset = m_eptr.Pointer( eEnemyAddrHOffset );
	USHORT & uLevelAddrHOffset = m_eptr.Pointer( eLevelAddrHOffset );
	USHORT uEnemiesCount = uEnemyPtrHi - uEnemyPtrLo;
	USHORT uLevelsCount = uLevelPtrHi - uLevelPtrLo;
	USHORT uQBricksPtr = 0;
	USHORT uLevelBanksPtr = 0;

	std::map<USHORT, std::vector<BYTE>> mEnemies, mLevels;
	std::vector<USHORT> vEnemiesPtr, vLevelsPtr;
	ReadAreaData( uEnemyPtrLo, uEnemyPtrHi, uEnemiesCount, 0xFF, mEnemies, vEnemiesPtr );
	ReadAreaData( uLevelPtrLo, uLevelPtrHi, uLevelsCount, 0xFD, mLevels, vLevelsPtr );

	// copy levels offsets to the new place (after enemy offsets)
	// because its a pointers place in NROM, then do it before writing data
	CopyMemory(
		m_mmc3.Data<BYTE[ NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET ]>( uEnemyAddrHOffset + NES_LENGTH_OF_ENEMY_ADDR_H_OFFSET ),
		m_mmc3.Data<BYTE[ NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET ]>( uLevelAddrHOffset ),
		NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET );
	
	// so new pointer is uEnemy + length of enemy offsets
	uLevelAddrHOffset = uEnemyAddrHOffset + NES_LENGTH_OF_ENEMY_ADDR_H_OFFSET;
	uQBricksPtr = uLevelAddrHOffset + NES_LENGTH_OF_LEVEL_ADDR_H_OFFSET;

	// copy qbricks metatiles to new offset
	MoveQBricks( uQBricksPtr );

	// change pointers to enemies and levels
	uEnemyPtrLo = uQBricksPtr + NES_LENGTH_OF_QBRICKS_METATILES;
	uEnemyPtrHi = uEnemyPtrLo + uEnemiesCount;
	uLevelPtrLo = uEnemyPtrHi + uEnemiesCount;
	uLevelPtrHi = uLevelPtrLo + uLevelsCount;

	// calculate pointer for levels banks. If levels pointers are 0xAzzz, then levels banks will be at 0xB000 - 128
	uLevelBanksPtr = ( ( uLevelPtrHi + 0x1000 ) & 0xF000 ) - 128;
	m_eptr.Select( eSharedEnemyPtrLo );

	// write the data
	uMMC3Ptr = WriteAreaData( uEnemyPtrLo, uEnemyPtrHi, uMMC3Ptr, mEnemies, vEnemiesPtr );
	uMMC3Ptr = WriteAreaData( uLevelPtrLo, uLevelPtrHi, uMMC3Ptr, mLevels, vLevelsPtr );
	
	m_mLocalPointers[ NES_L_LEVEL_BANKS_PTR ] = uLevelBanksPtr;
	
	// write a banks numbers (its #03) to a array
	WriteBanksArray( 0x80, uLevelBanksPtr, m_eptr[ eSharedEnemyPtrLo ].bLoBank );
}

VOID CNesConverter::MoveMusicData()
{
	NES_EPOINTER eMMC3MusicPtr = m_eptr[ eSharedMusicPtr ];
	USHORT eNROMMusicPtr = m_eptr.Pointer( eMusicPtr2 );
	NES_EPOINTER eEOFMusicPtr = m_eptr[ eEOFMusicData ];
	USHORT uSizeOfMusicData = eEOFMusicPtr.ptr - eNROMMusicPtr;
	USHORT uDelta = eNROMMusicPtr - eMMC3MusicPtr.ptr;
	std::map<USHORT, USHORT> mMusPtr;

	m_eptr.Select( eSharedMusicPtr );

	for ( USHORT i = 0; i < uSizeOfMusicData; ++i )
	{
		m_mmc3.Data<BYTE>( eMMC3MusicPtr.ptr + i ) = m_mmc3.Data<BYTE>( eNROMMusicPtr + i );
		m_mmc3.Data<BYTE>( eNROMMusicPtr + i ) = 0xFF;
	}

	m_eptr.Pointer( eMusicPtr1 ) = eMMC3MusicPtr.ptr - 1;
	m_eptr.Pointer( eMusicPtr2 ) = eMMC3MusicPtr.ptr + 0;
	m_eptr.Pointer( eMusicPtr3 ) = eMMC3MusicPtr.ptr + 1;
	m_eptr.Pointer( eMusicPtr4 ) = eMMC3MusicPtr.ptr + 2;
	m_eptr.Pointer( eMusicPtr5 ) = eMMC3MusicPtr.ptr + 3;
	m_eptr.Pointer( eMusicPtr6 ) = eMMC3MusicPtr.ptr + 4;
	m_eptr.Pointer( eMusicPtr7 ) = eMMC3MusicPtr.ptr + 5;

	// move code pointers
	m_eptr.Pointer( eMusicFreqPtr1 ) -= uDelta;
	m_eptr.Pointer( eMusicFreqPtr2 ) -= uDelta;
	m_eptr.Pointer( eMusicLenPtr ) -= uDelta;
	m_eptr.Pointer( eMusicEnvCastlePtr ) -= uDelta;
	m_eptr.Pointer( eMusicEnvAreaPtr ) -= uDelta;
	m_eptr.Pointer( eMusicEnvWaterPtr ) -= uDelta;
	m_eptr.Pointer( eMusicNoiseFlamePtr ) -= uDelta;
	m_eptr.Pointer( eMusicEnvBrickPtr ) -= uDelta;

	// patch pointers in headers
	for ( USHORT i = 0; i < 49; ++i )
	{
		USHORT ptr = eMMC3MusicPtr.ptr + m_mmc3.Data<BYTE>( eMMC3MusicPtr.ptr + i );
		if ( mMusPtr.find( ptr ) == mMusPtr.end() )
		{
			USHORT mptr = m_mmc3.Data<USHORT>( ptr + 1 );
			m_mmc3.Data<USHORT>( ptr + 1 ) = mptr - uDelta;
			mMusPtr[ ptr ] = mptr;
		}
	}	
}