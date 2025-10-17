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

// hardcode!
// (1st) byte - connected loop?
// (2nd) byte - area ptr
WORD g_wrldLoopTypes[]=
{
	0x0061, 0x0061,
	0x0164, 0x0164, 0x0064, 0x0164, 0x0164, 0x0064,
	0x0065, 0x0065, 0x0065
};

typedef NES_LOOP_COMMAND LOOP_COMMAND;

/*
		////		second patch
		LDA #01
		JSR $FFD7
		RTS
*/

VOID CNesConverter::PatchLoop()
{
	const NES_LEVEL_TYPE lvlType[] = { nltWater,nltGround,nltUnderground,nltCastle };
	CNesAssembly code( m_eptr[ eLoopCode ].ptr );
	std::vector<BYTE> raw;
	USHORT uPointersPtr = m_eptr[ eSharedLoopCommands ].ptr;
	USHORT uPtr = uPointersPtr + sizeof( USHORT ) * 0x80;	
	USHORT uPagesPtr = m_eptr.Pointer( eLoopPages );
	USHORT uYPtr = m_eptr.Pointer( eLoopY );
	USHORT uRewOffsets = m_eptr.Pointer( eLoopRewindOffsets );
	USHORT uLoopCounterIncCode = m_eptr[ eLoopCounterIncCode ].ptr;
	USHORT uLoopAreaOfsCode = m_eptr[ eLoopAreaOfsCode ].ptr;
	USHORT uExecLoopBack = m_eptr.Pointer( eExecLoopBack );
	USHORT uKillAllEnemies = m_eptr.Pointer( eKillAllEnemies );
	BYTE bLoopDataBank = m_eptr[ eSharedLoopCommands ].bLoBank;

	code.label( TEXT( "_" ) );
		code.LDA( INSTR_IMM( bLoopDataBank ) );
		code.JSR( m_aSharedCode.LabelPC( TEXT( "UnsafeSwitchLoBank" ) ) );
		code.LDA( INSTR_ABS( 0x76C ) );
		code.STA( INSTR_ABS( 0 ) );
		code.LDA( INSTR_ABS( 0x76D ) );
		code.STA( INSTR_ABS( 1 ) );
		code.LDY( INSTR_IMM( 0 ) );
		code.LDA( INSTR_IND_Y( 0 ) );
		code.CMP( INSTR_ABS( 0x725 ) );
		code.BEQ( TEXT( "beginCheck" ) );
		code.BCS( TEXT( "ignoreCheck" ) );
		code.BCC( TEXT( "stopThisCheck" ) );

	code.label( TEXT( "beginCheck" ) );
		code.INY();
		code.LDA( INSTR_IND_Y( 0 ) );
		code.INY();
		code.CMP( INSTR_ABS( 0xCE ) );
		code.BNE( TEXT( "failCheck" ) );
		code.LDA( INSTR_ABS( 0x1D ) );
		code.BNE( TEXT( "failCheck" ) );
		code.LDA( INSTR_IND_Y( 0 ) );
		code.BNE( TEXT( "stopThisCheck" ) );
		code.LDA( INSTR_ABS( 0x6D9 ) );
		code.BEQ( TEXT( "stopThisCheck" ) );
		code.BNE( TEXT( "rewindPage" ) );
	
	code.label( TEXT( "failCheck" ) );
		code.LDA( INSTR_IND_Y( 0 ) );
		code.BEQ( TEXT( "rewindPage" ) );
		code.INC( INSTR_ABS( 0x6D9 ) );
		code.BNE( TEXT( "stopThisCheck" ) );

	code.label( TEXT( "rewindPage" ) );
		code.JSR( uExecLoopBack );
		code.JSR( uKillAllEnemies );
		code.STA( INSTR_ABS( 0x6D9 ) );

	code.label( TEXT( "stopThisCheck" ) );
		code.LDA( INSTR_IMM( 0 ) );
		code.STA( INSTR_ABS( 0x745 ) );

	code.label( TEXT( "ignoreCheck" ) );
		code.LDA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.JSR( m_aSharedCode.LabelPC( TEXT( "UnsafeSwitchLoBank" ) ) );
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();
		code.NOP();

	code.Compile( raw );

	std::map<BYTE, std::vector<LOOP_COMMAND>> mLoopCmd;

	// rewrite pointer to stub procedure to the new procedure,
	// which finds a pointer for loops commands array and sets it
	// if pointer is NULL, then no loops will be defined for current area
	m_eptr.Pointer( eLoopHandler ) = m_eptr[ eSharedLoopHandler ].ptr;

	// erase command 'INC vLoopCommand' from DecodeAreaData
	// vLoopCommand flag will be set up in the new procedure,
	// which handles LOOP object (instead of stub RTS)
	m_mmc3.Data<BYTE>( uLoopCounterIncCode++ ) = 0xEA;	// NOP
	m_mmc3.Data<BYTE>( uLoopCounterIncCode++ ) = 0xEA;	// NOP
	m_mmc3.Data<BYTE>( uLoopCounterIncCode++ ) = 0xEA;	// NOP

	// get offset value for rewind Area Data Pointer
	// from the new array instead of stupid array from the old code
	m_mmc3.Data<BYTE>( uLoopAreaOfsCode++ ) = 0xC8; // INY
	m_mmc3.Data<BYTE>( uLoopAreaOfsCode++ ) = 0xB1; // LDA 
	m_mmc3.Data<BYTE>( uLoopAreaOfsCode++ ) = 0x00; // ($00), Y

	// set up bank which will contain an array of loops stuff
	m_mmc3.SelectPRG( m_eptr[ eSharedLoopCommands ].bLoBank, TRUE );
	
	// set up based on original ROM values
	// values is hardcoded at the g_wrldLoopTypes
	// Hi byte contents type of LOOP (in the new data specification)
	// Lo byte contents an area pointer which keeps original LOOP objects
	// Pages and Y positions retreived from the original arrays in the ROM
	for ( const auto & world : g_wrldLoopTypes )
	{
		LOOP_COMMAND lpc = { 0 };
		lpc.bIdInGroup = HIBYTE( world );
		lpc.bPage = m_mmc3.Data<BYTE>( uPagesPtr++ );
		lpc.Y = m_mmc3.Data<BYTE>( uYPtr++ );
		lpc.bRewindOffset = m_mmc3.Data<BYTE>( uRewOffsets );
		mLoopCmd[ LOBYTE( world ) ].push_back( lpc );
	}

	// enumerate ALL possible values for area pointers
	// for specified areas will be created a new structure of LOOP commands
	for ( const auto & type : lvlType )
	{
		NES_AREA_PTR aptr = { 0 };
		aptr.bType = type;
		aptr.bAreaId = 0;
		for ( int i = 0; i < 32; ++i, ++aptr.bAreaId )
		{
			// default value of pointer is NULL
			// its means that no loop array defined for this area
			auto lpcIt = mLoopCmd.find( aptr.bPtr );
			USHORT uDataPtr = 0x0000;
			if ( lpcIt != mLoopCmd.end() )
			{
				// found a data for area 'aptr'
				// so store a LOOP command specifications and set up a pointer to it
				const std::vector<LOOP_COMMAND> & vlpc = lpcIt->second;
				uDataPtr = uPtr;
				m_mmc3.Data<USHORT>( uPointersPtr + sizeof( uDataPtr ) * static_cast<USHORT>( aptr.bPtr ) ) = uPtr;

				for ( const auto & lpc : vlpc )
				{

					m_mmc3.Data<LOOP_COMMAND>( uDataPtr ) = lpc;
					uDataPtr += sizeof( LOOP_COMMAND );
				}

				// each array must be terminated by 0xFF (maximum page ptr value is 0x7F)
				m_mmc3.Data<BYTE>( uDataPtr++ ) = 0xFF;
				uPtr = uDataPtr;
			}
		}
	}

	// rewrite original ProcLoopCommand code with new loops handler
	Patch( eLoopCode, raw.data(), LOWORD( raw.size() ) );

	// new code (instead of stub RTS) see in the shared patch
}