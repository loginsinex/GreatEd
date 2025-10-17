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

VOID CNesConverter::PatchHalfPage()
{
	const BYTE bHalfPagesBank = m_eptr[ eSharedHalfPages ].bLoBank;
	const USHORT uHalfPagesPtr = m_eptr[ eSharedHalfPages ].ptr;
	const USHORT uHalfPagesDataPtr = uHalfPagesPtr + NES_WORLDS_COUNT;
	const USHORT uHalfPagesNROMData = m_eptr.Pointer( eHalfPagesData );
	CNesAssembly code( m_eptr[ eHalfPagesCode ].ptr );
	std::vector<BYTE> raw;

	code.label( TEXT( "_" ) );
		code.LDA( INSTR_IMM( bHalfPagesBank ) );
		code.JSR( m_aSharedCode.LabelPC( "UnsafeSwitchLoBank" ) );
		code.LDY( INSTR_ABS( 0x75F ) );
		code.LDA( INSTR_ABSY( uHalfPagesPtr ) );
		code.STA( INSTR_ABS( 0 ) );
		code.LDA( INSTR_ABS( 0x75C ) );
		code.ADC( INSTR_ABS( 0 ) );
		code.TAY();
		code.LDA( INSTR_ABSY( uHalfPagesDataPtr ) );
		code.CMP( INSTR_ABS( 0x71A ) );
		code.BEQ( TEXT( "lbSetPage" ) );
		code.BCC( TEXT( "lbSetPage" ) );
		code.LDA( INSTR_IMM( 0 ) );
	
	code.label( TEXT( "lbSetPage" ) );
		code.STA( INSTR_ABS( 0x75B ) );
		code.LDA( INSTR_IMM( 1 ) );
		code.JSR( m_aSharedCode.LabelPC( TEXT( "UnsafeSwitchLoBank" ) ) );
		code.LDA( INSTR_IMM( 0 ) );

	code.Compile( raw );

	BYTE bData[ 2 * NES_WORLDS_COUNT ] = { 0 };
	BYTE bWorldOffset[ NES_WORLDS_COUNT ] = { 0 };
	BYTE bOffset = 0, bLevelNumber = 0, bWorld = 0;
	USHORT uDataPtr = uHalfPagesDataPtr;

	// read the data
	CopyMemory( bData, m_mmc3.Data<BYTE[ 2 * NES_WORLDS_COUNT ]>( uHalfPagesNROMData ), 2 * NES_WORLDS_COUNT );

	m_mmc3.SelectPRG( bHalfPagesBank, TRUE );
	// ignore worlds offsets
	// its will be written by worlds dumper
	// just dump data as is
	for ( auto b : bData )
	{
		const BYTE bArea1 = ( ( b & 0xF0 ) >> 4 );
		const BYTE bArea2 = ( b & 0x0F );
		m_mmc3.Data<BYTE>( uDataPtr++ ) = bArea1;
		m_mmc3.Data<BYTE>( uDataPtr++ ) = bArea2;
	}

	Patch( eHalfPagesCode, raw.data(), LOWORD( raw.size() ) );
}