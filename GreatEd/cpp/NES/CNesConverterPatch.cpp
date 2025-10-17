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

VOID CNesConverter::WriteCodePatch()
{
	USHORT & uNROMNMIPtr = m_eptr.Pointer( eSharedNMIPtr );
	USHORT & uNROMIRQPtr = m_eptr.Pointer( eSharedIRQPtr );
	USHORT & uNROMRESETPtr = m_eptr.Pointer( eSharedRESETPtr );
	USHORT & uNROMSoundEngine = m_eptr.Pointer( eSoundEnginePtr );

	USHORT & uNROMGetAreaAddr = m_eptr.Pointer( eGetAreaAddressesPtr );
	USHORT & uNROMProcAreaData1 = m_eptr.Pointer( eProcessAreaData1Ptr );
	USHORT & uNROMProcAreaData2 = m_eptr.Pointer( eProcessAreaData2Ptr );
	USHORT & uNROMEnemyCore = m_eptr.Pointer( eEnemiesAndLoopsCorePtr );
	USHORT uLevelsBanks = m_mLocalPointers[ NES_L_LEVEL_BANKS_PTR ];
	USHORT uWorldAreaOffsets = m_eptr.Pointer( eWorldAreasOffsets );
	USHORT uWorldAreas = m_eptr.Pointer( eWorldAreas );

	const BYTE bLoopDataBank = m_eptr[ eSharedLoopCommands ].bLoBank;
	const USHORT uLoopDataPtr = m_eptr[ eSharedLoopCommands ].ptr;

	CNesAssembly & code = m_aSharedCode;
	std::vector<BYTE> raw;

	/////////////////	PATCH CODE	///////////////////
	code.label( TEXT( "AltGetAddresses" ) );
		code.LDA( INSTR_ABS( 0x0750 ) );
		code.STA( INSTR_ABS( 0x076E ) );
		code.JSR( TEXT( "UpdateAreaPtr" ) );
		code.JMP( TEXT( "AltGetAreaAddresses" ) );

	code.label( TEXT( "LoopCmdE" ) );
		code.LDA( INSTR_ABS( 0x0745 ) );
		code.BEQ( TEXT( "beginCheck" ) );
		code.RTS();

	code.label( TEXT( "beginCheck" ) );
		code.LDA( INSTR_IMM( bLoopDataBank ) );
		code.JSR( TEXT( "UnsafeSwitchLoBank" ) );
		code.LDA( INSTR_ABS( 0x076E ) );
		code.ASL( INSTR_ACC() );
		code.TAX();
		code.LDA( INSTR_ABSX( uLoopDataPtr + 1 ) );
		code.BEQ( TEXT( "lbExit2" ) );
		code.STA( INSTR_ABS( 0x01 ) );
		code.LDA( INSTR_ABSX( uLoopDataPtr ) );
		code.STA( INSTR_ABS( 0x00 ) );
		code.LDY( INSTR_IMM( 0 ) );

	code.label( TEXT( "lbLoopFindPage" ) );
		code.LDA( INSTR_IND_Y( 0 ) );
		code.CMP( INSTR_ABS( 0x0725 ) );
		code.BEQ( TEXT( "lbIncY" ) );
		code.BCC( TEXT( "lbIncY" ) );
		code.TYA();
		code.CLC();
		code.ADC( INSTR_ABS( 0 ) );
		code.STA( INSTR_ABS( 0x076C ) );
		code.LDA( INSTR_IMM( 0 ) );
		code.ADC( INSTR_ABS( 1 ) );
		code.STA( INSTR_ABS( 0x076D ) );
		code.INY();
		code.LDA( INSTR_IMM( 1 ) );
		code.BNE( TEXT( "lbExit1" ) );

	code.label( TEXT( "lbIncY" ) );
		code.INY();
		code.INY();
		code.INY();
		code.INY();
		code.LDA( INSTR_IND_Y( 0 ) );
		code.BPL( TEXT( "lbLoopFindPage" ) );
		code.LDA( INSTR_IMM( 0 ) );

	code.label( TEXT( "lbExit1" ) );
		code.STA( INSTR_ABS( 0x0745 ) );

	code.label( TEXT( "lbExit2" ) );
		code.LDA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.JMP( TEXT( "UnsafeSwitchLoBank" ) );

	// EOF loopers patch

	code.label( TEXT( "sbrPwrUpJmp" ) );
		code.LDA( INSTR_IMM( 0x01 ) );
		code.STA( INSTR_ABS( 0x23 ) );
		code.STA( INSTR_ABS( 0x14 ) );
		code.LDA( INSTR_IMM( 0x03 ) );
		code.STA( INSTR_ABS( 0x049F ) );
		code.LDA( INSTR_ABS( 0x39 ) );
		code.CMP( INSTR_IMM( 0x02 ) );
		code.BCS( TEXT( "lbPutBehind" ) );
		code.LDA( INSTR_ABS( 0x0756 ) );
		code.CMP( INSTR_IMM( 0x02 ) );
		code.BCC( TEXT( "lbStrType" ) );
		code.LSR( INSTR_ACC() );

	code.label( TEXT( "lbStrType" ) );
		code.STA( INSTR_ABS( 0x39 ) );

	code.label( TEXT( "lbPutBehind" ) );
		code.LDA( INSTR_IMM( 0x20 ) );
		code.STA( INSTR_ABS( 0x03CA ) );
		code.LDA( INSTR_IMM( 0x02 ) );
		code.STA( INSTR_ABS( 0xFE ) );
		code.RTS();

	code.label( TEXT( "sbrSetupVine" ) );
		code.LDA( INSTR_IMM( 0x2f ) );
		code.STA( INSTR_ABSX( 0x16 ) );
		code.LDA( INSTR_IMM( 0x01 ) );
		code.STA( INSTR_ABSX( 0x0F ) );
		code.LDA( INSTR_ABSY( 0x76 ) );
		code.STA( INSTR_ABSX( 0x6E ) );
		code.LDA( INSTR_ABSY( 0x8F ) );
		code.STA( INSTR_ABSX( 0x87 ) );
		code.LDA( INSTR_ABSY( 0xD7 ) );
		code.STA( INSTR_ABSX( 0xCF ) );
		code.LDY( INSTR_ABS( 0x0398 ) );
		code.BNE( TEXT( "lbNextVO" ) );
		code.STA( INSTR_ABS( 0x039D ) );

	code.label( TEXT( "lbNextVO" ) );
		code.TXA();
		code.STA( INSTR_ABSY( 0x39A ) );
		code.INC( INSTR_ABS( 0x398 ) );
		code.LDA( INSTR_IMM( 0x04 ) );
		code.STA( INSTR_ABS( 0xFE ) );
		code.RTS();

	code.label( TEXT( "AltFindAreaPointer" ) );
		code.LDY( INSTR_ABS( 0x75F ) );
		code.LDA( INSTR_ABSY( uWorldAreaOffsets ) );
		code.CLC();
		code.ADC( INSTR_ABS( 0x760 ) );
		code.TAY();
		code.LDA( INSTR_ABSY( uWorldAreas ) );
	code.label( TEXT( "UpdateAreaPtr" ) );
		code.PHA();
		code.AND( INSTR_IMM( 0x7F ) );
		code.TAY();
		code.LDA( INSTR_ABSY( uLevelsBanks ) );
		code.STA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.PLA();
		code.RTS();

	code.label( TEXT( "AltGetAreaAddresses" ) );
		code.LDA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.JSR( TEXT( "UnsafeSwitchLoBank" ) );
		code.JSR( uNROMGetAreaAddr );
		code.LDA( INSTR_IMM( 0x1 ) );
		code.JMP( TEXT( "UnsafeSwitchLoBank" ) );

	code.label( TEXT( "AltProcessAreaData" ) );
		code.LDA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.JSR( TEXT( "UnsafeSwitchLoBank" ) );
		code.JSR( uNROMProcAreaData1 );
		code.LDA( INSTR_IMM( 0x1 ) );
		code.JMP( TEXT( "UnsafeSwitchLoBank" ) );

	code.label( TEXT( "AltEnemyCore" ) );
		code.LDA( INSTR_ABS( NES_BANK_VARIABLE ) );
		code.JSR( TEXT( "UnsafeSwitchLoBank" ) );
		code.JSR( uNROMEnemyCore );
		code.LDA( INSTR_IMM( 0x1 ) );
		code.JMP( TEXT( "UnsafeSwitchLoBank" ) );

	code.label( TEXT( "SoundEngine" ) );
		code.LDA( INSTR_IMM( 0x02 ) );
		code.JSR( TEXT( "UnsafeSwitchHiBank" ) );
		code.JSR( uNROMSoundEngine );
		code.LDA( INSTR_IMM( 0x0 ) );
		code.JMP( TEXT( "UnsafeSwitchHiBank" ) );

	code.label( TEXT( "InitCHR" ) );
		code.LDA( INSTR_IMM( 0 ) );
		code.STA( INSTR_ABS( 0xA000 ) );
		code.TAX();
		code.CLC();

	code.label( TEXT( "LoopInitChr1" ) );
		code.STX( INSTR_ABS( 0x8000 ) );
		code.STA( INSTR_ABS( 0x8001 ) );
		code.ADC( INSTR_IMM( 0x2 ) );
		code.INX();
		code.CPX( INSTR_IMM( 0x2 ) );
		code.BNE( TEXT( "LoopInitChr1" ) );
		code.CLC();

	code.label( TEXT( "LoopInitChr2" ) );
		code.STX( INSTR_ABS( 0x8000 ) );
		code.STA( INSTR_ABS( 0x8001 ) );
		code.ADC( INSTR_IMM( 0x1 ) );
		code.INX();
		code.CPX( INSTR_IMM( 0x6 ) );
		code.BNE( TEXT( "LoopInitChr2" ) );
		code.RTS();

	code.label( TEXT( "UnsafeSwitchHiBank" ) );
		code.LDY( INSTR_IMM( 0x06 ) );
		code.STY( INSTR_ABS( 0x8000 ) );
		code.STA( INSTR_ABS( 0x8001 ) );
		code.RTS();
		code.BRK();
		code.BRK();

	code.label( TEXT( "UnsafeSwitchLoBank" ) );
		code.LDY( INSTR_IMM( 0x07 ) );
		code.STY( INSTR_ABS( 0x8000 ) );
		code.STA( INSTR_ABS( 0x8001 ) );
		code.RTS();
		code.BRK();
		code.BRK();

	code.label( TEXT( "NMI" ) );
		code.JMP( INSTR_ABS( uNROMNMIPtr ) );

	code.label( TEXT( "RESET" ) );
		code.SEI();
		code.LDA( INSTR_IMM( 0 ) );
		code.JSR( TEXT( "UnsafeSwitchHiBank" ) );
		code.LDA( INSTR_IMM( 1 ) );
		code.JSR( TEXT( "UnsafeSwitchLoBank" ) );
		code.JSR( TEXT( "InitCHR" ) );
		code.JMP( INSTR_ABS( uNROMRESETPtr ) );

	code.label( TEXT( "ENDLESS" ) );
		code.JMP( TEXT( "ENDLESS" ) );

	code.label( TEXT( "IRQ" ) );
		code.RTI();
		/////////////////	EOF PATCH CODE	///////////////////

	code.Rebase( 0xFFFA - code.size() );
	code.Compile( raw );

	PatchCode( code.Base(), raw.data(), LOWORD( raw.size() ) );

	m_eptr.Select( eRESETEndlessJmp );
	m_mmc3.Data<USHORT>( m_eptr[ eRESETEndlessJmp ].ptr + 1 ) =
		code.LabelPC( TEXT( "ENDLESS" ) );	// JMP $ENDLESS instead of JMP $$

	uNROMNMIPtr = code.LabelPC( TEXT( "NMI" ) );
	uNROMRESETPtr = code.LabelPC( TEXT( "RESET" ) );
	uNROMIRQPtr = code.LabelPC( TEXT( "IRQ" ) );
	uNROMSoundEngine = code.LabelPC( TEXT( "SoundEngine" ) );

	uNROMGetAreaAddr = code.LabelPC( TEXT( "AltGetAddresses" ) );
	uNROMProcAreaData1 = uNROMProcAreaData2 = code.LabelPC( TEXT( "AltProcessAreaData" ) );
	uNROMEnemyCore = code.LabelPC( TEXT( "AltEnemyCore" ) );
	m_eptr.Pointer( eFindAreaPointer ) = code.LabelPC( TEXT( "AltFindAreaPointer" ) );

	// move these two initializers into the shared section, 
	// cause its placed in the unavailable bank while enemy ProcLoopCommand executes
	m_eptr.Pointer( ePwrUpJmpPtr ) = code.LabelPC( "sbrPwrUpJmp" );
	m_eptr.Pointer( eSetupVinePtr ) = code.LabelPC( "sbrSetupVine" );
}
