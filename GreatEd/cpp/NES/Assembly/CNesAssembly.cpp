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
#include "h\NES\Assembly\assembly_map.h"

CNesAssemblyCompileError::CNesAssemblyCompileError( const CString & sError, size_t uLine )
	: m_compileError( sError ), m_uLine( uLine )
{
}

const CString & CNesAssemblyCompileError::Error() const
{
	return m_compileError;
}

size_t CNesAssemblyCompileError::Line() const
{
	return m_uLine;
}

// assembly

CNesAssembly::CNesAssembly( NES_OFFSET uBase )
	: m_uBase( uBase ), m_uOffset( 0 )
{

}

VOID CNesAssembly::clear()
{
	m_uOffset = 0;
	m_mLabel.clear();
	m_vLabel.clear();
	m_vOpCode.clear();
}

VOID CNesAssembly::Rebase( NES_OFFSET uBase )
{
	if ( uBase >= 0x8000 ) m_uBase = uBase;
}

NES_OFFSET CNesAssembly::Base() const
{
	return m_uBase;
}

NES_OFFSET CNesAssembly::Offset() const
{
	return m_uOffset;
}

NES_LABEL CNesAssembly::LastLabel() const
{
	return m_vOpCode.size();
}

NES_OFFSET CNesAssembly::OpcodeSize( NES_INSTRUCTION_TYPE optype ) const
{
	switch ( optype )
	{
		case nitLABEL:
		case nitIMPL:
		case nitACC: return 0;
		case nitABS:
		case nitABSX:
		case nitABSY:
		case nitIND: return sizeof( NES_OFFSET );
		case nitIMM:
		case nitX_IND:
		case nitIND_Y:
		case nitREL: return 1;
		case nitZPG:
		case nitZPG_X:
		case nitZPG_Y: return 1;
	}

	return 0;
}

NES_OPCODE & CNesAssembly::NewOpcode( NES_INSTRUCTION_TYPE optype )
{
	m_vOpCode.emplace_back( NES_OPCODE({ optype, niiNOP, niocNOP_IMPL, 0, OpcodeSize( optype ), m_uOffset, LastLabel() }) );
	NES_OPCODE & opcode = m_vOpCode.back();
	m_uOffset += opcode.size;
	if ( nitLABEL != optype ) m_uOffset++;
	return opcode;
}

NES_OPCODE & CNesAssembly::NewOpcode( NES_INSTRUCTION_ID id, NES_INSTRUCTION_TYPE type )
{
	auto itOpcodeList = l_mCode.find( id );
	if ( itOpcodeList != l_mCode.end() )
	{
		auto itOpcode = itOpcodeList->second.find( type );
		if ( itOpcode != itOpcodeList->second.end() )
		{
			auto & opcode = NewOpcode( type );
			opcode.opcode = itOpcode->second;
			opcode.id = id;
			return opcode;
		}
	}

	throw CNesAssemblyCompileError( TEXT( "Unknown instruction" ), LastLabel() );
}

// modifies LAST opcode due to possible size change of opcode
VOID CNesAssembly::ModifyOpcode( NES_INSTRUCTION_ID id, NES_INSTRUCTION_TYPE type )
{
	// don't throw exception if cannot modify
	
	if ( m_vOpCode.empty() ) return;	// nothing to do

	NES_OPCODE & opcode = m_vOpCode.back();
	auto itOpcodeList = l_mCode.find( id );
	if ( itOpcodeList != l_mCode.end() )
	{
		auto itOpcode = itOpcodeList->second.find( type );
		if ( itOpcode != itOpcodeList->second.end() )
		{
			opcode.opcode = itOpcode->second;			
			opcode.type = type;
			opcode.size = OpcodeSize( type );
			m_uOffset = opcode.pc + opcode.size;	// fix next pc
			if ( opcode.type != nitLABEL ) m_uOffset++;
		}
	}
}

VOID CNesAssembly::label( const CString & sLabel )
{
	auto it = m_mLabel.find( sLabel );
	if ( it != m_mLabel.end() )
	{
		throw CNesAssemblyCompileError( TEXT( "Label already defined '" ) + sLabel + TEXT( "'" ), LastLabel() );
	}

	m_mLabel[ sLabel ] = LastLabel();

#ifdef _DEBUG
	auto & opc = NewOpcode( nitLABEL );
	_tprintf( TEXT( "%ws -> %04x\n" ), sLabel.GetString(), opc.pc + m_uBase );
#else
	NewOpcode( nitLABEL );
#endif
}

NES_OFFSET CNesAssembly::LabelPC( const CString & sLabel ) const
{
	auto itLabel = m_mLabel.find( sLabel );
	if ( itLabel == m_mLabel.end() )
	{
		throw CNesAssemblyCompileError( TEXT( "Label not found: '" ) + sLabel + TEXT( "'" ), 0 );
	}

	if ( itLabel->second >= m_vOpCode.size() )
	{
		throw CNesAssemblyCompileError( TEXT( "INTERNAL: Can't find label in list" ), 0 );
	}

	return m_vOpCode[ itLabel->second ].pc + m_uBase;
}

USHORT CNesAssembly::GetLabel( const CString & sLabel )
{
	auto it = std::find( m_vLabel.begin(), m_vLabel.end(), sLabel );
	if ( it != m_vLabel.end() ) return LOWORD( it - m_vLabel.begin() );
	m_vLabel.push_back( sLabel );
	return LOWORD( m_vLabel.size() - 1 );
}

VOID CNesAssembly::REL( NES_INSTRUCTION_ID id, const CString & sLabel )
{
	auto & opcode = NewOpcode( id, nitREL );
	opcode.arg = GetLabel( sLabel );
}

VOID CNesAssembly::ABSLB( NES_INSTRUCTION_ID id, const CString & sLabel )
{
	auto & opcode = NewOpcode( id, nitABS );
	opcode.arg = GetLabel( sLabel );
	opcode.type = nitREL;
}

VOID CNesAssembly::IMPL( NES_INSTRUCTION_ID id )
{
	NewOpcode( id, nitIMPL );
}

VOID CNesAssembly::OPC( NES_INSTRUCTION_ID id, DWORD dwArg )
{
	NES_INSTRUCTION_TYPE nit = static_cast<NES_INSTRUCTION_TYPE>( LOBYTE( HIWORD( dwArg ) ) );
	NES_ARG arg = LOWORD( dwArg );
	auto & opcode = NewOpcode( id, nit );
	opcode.arg = arg;
	
	// try to optimize ABS -> ZPG, ABS_X -> ZPG_X, ABS_Y -> ZPG_Y if HIWORD of arg is zero
	if ( !HIBYTE( arg ) )
	{
		switch ( nit )
		{
			case nitABS: ModifyOpcode( id, nitZPG ); break;
			case nitABSX: ModifyOpcode( id, nitZPG_X ); break;
			case nitABSY: ModifyOpcode( id, nitZPG_Y ); break;
		}
	}
}

const NES_OPCODE & CNesAssembly::GetLabelOpcode( NES_OFFSET label_id ) const
{
	if ( label_id >= m_vLabel.size() )
	{
		throw CNesAssemblyCompileError( TEXT( "INTERNAL: Label id cannot be found in labels list" ), 0 );
	}

	auto & sLabel = m_vLabel[ label_id ];
	auto itLabel = m_mLabel.find( sLabel );
	if ( itLabel == m_mLabel.end() )
	{
		throw CNesAssemblyCompileError( TEXT( "Label not found: '" ) + sLabel + TEXT( "'" ), 0 );
	}

	if ( itLabel->second >= m_vOpCode.size() )
	{
		throw CNesAssemblyCompileError( TEXT( "INTERNAL: Can't find label in list" ), 0 );
	}

	return m_vOpCode[ itLabel->second ];
}

VOID CNesAssembly::CompileAbsLb( const NES_OPCODE & opcode, std::vector<BYTE>& code ) const
{
	const auto label = GetLabelOpcode( opcode.arg );
	auto pcLabel = label.pc;

	if ( static_cast<LONG>( m_uBase ) + static_cast<LONG>( pcLabel ) >= 0x10000 )
	{
		throw CNesAssemblyCompileError( TEXT( "Jump out of range" ), opcode.line );
	}

	pcLabel += m_uBase;
	code.push_back( LOBYTE( pcLabel ) );
	code.push_back( HIBYTE( pcLabel ) );
}

VOID CNesAssembly::CompileRelLb( const NES_OPCODE & opcode, std::vector<BYTE>& code ) const
{
	const auto label = GetLabelOpcode( opcode.arg );
	const auto pcLabel = label.pc;
	const auto pcOpc = opcode.pc + opcode.size + 1 /* command byte */;

	// TODO: Double check forward jump for max value - 128
	if ( pcLabel < pcOpc )
	{
		if ( pcOpc - pcLabel > 127 ) throw CNesAssemblyCompileError( TEXT( "Branch too far" ), opcode.line );
		code.push_back( LOBYTE( 0x100 - ( pcOpc - pcLabel ) ) );
	}
	else
	{
		if ( pcLabel - pcOpc > 128 ) throw CNesAssemblyCompileError( TEXT( "Branch too far" ), opcode.line );
		code.push_back( LOBYTE( pcLabel - pcOpc ) );
	}
}

VOID CNesAssembly::CompileRel( const NES_OPCODE & opcode, std::vector<BYTE>& code ) const
{
	code.push_back( opcode.opcode );
	switch ( opcode.size )
	{
		case 1: CompileRelLb( opcode, code ); break;
		case 2: CompileAbsLb( opcode, code ); break;
	}
}

VOID CNesAssembly::CompileOpc( const NES_OPCODE & opcode, std::vector<BYTE>& code ) const
{
	code.push_back( opcode.opcode );
	switch ( opcode.size )
	{
		case 1: code.push_back( LOBYTE( opcode.arg ) ); break;
		case 2: code.push_back( LOBYTE( opcode.arg ) ); code.push_back( HIBYTE( opcode.arg ) ); break;
	}
}

VOID CNesAssembly::Compile( std::vector<BYTE>& code ) const
{
	code.clear();

	for ( const auto & opcode : m_vOpCode )
	{
		switch ( opcode.type )
		{
			case nitLABEL: break;	// labels does not have any opcodes
			case nitREL: CompileRel( opcode, code ); break;
			default: CompileOpc( opcode, code ); break;
		}
	}
}

NES_OFFSET CNesAssembly::size() const
{
	return m_uOffset;
}

VOID CNesAssembly::dump_labels() const
{
#ifdef _DEBUG
	for ( const auto & lb : m_mLabel )
	{
		_tprintf( TEXT( "%ws -> 0x%04x\n" ), lb.first.GetString(), m_uBase + m_vOpCode[ lb.second ].pc );
	}
#endif
}


// instructions list

VOID CNesAssembly::BPL( const CString & sLabel ) { REL( niiBPL, sLabel ); }
VOID CNesAssembly::BMI( const CString & sLabel ) { REL( niiBMI, sLabel ); }
VOID CNesAssembly::BVC( const CString & sLabel ) { REL( niiBVC, sLabel ); }
VOID CNesAssembly::BVS( const CString & sLabel ) { REL( niiBVS, sLabel ); }
VOID CNesAssembly::BCC( const CString & sLabel ) { REL( niiBCC, sLabel ); }
VOID CNesAssembly::BCS( const CString & sLabel ) { REL( niiBCS, sLabel ); }
VOID CNesAssembly::BNE( const CString & sLabel ) { REL( niiBNE, sLabel ); }
VOID CNesAssembly::BEQ( const CString & sLabel ) { REL( niiBEQ, sLabel ); }
VOID CNesAssembly::JSR( const CString & sLabel ) { ABSLB( niiJSR, sLabel ); }
VOID CNesAssembly::JSR( NES_OFFSET arg )
{
	auto & opcode = NewOpcode( niiJSR, nitABS );
	opcode.arg = arg;
}

VOID CNesAssembly::JMP( const CString & sLabel ) { ABSLB( niiJMP, sLabel ); }
VOID CNesAssembly::BRK() { IMPL( niiBRK ); }
VOID CNesAssembly::PHP() { IMPL( niiPHP ); }
VOID CNesAssembly::CLC() { IMPL( niiCLC ); }
VOID CNesAssembly::PLP() { IMPL( niiPLP ); }
VOID CNesAssembly::SEC() { IMPL( niiSEC ); }
VOID CNesAssembly::RTI() { IMPL( niiRTI ); }
VOID CNesAssembly::PHA() { IMPL( niiPHA ); }
VOID CNesAssembly::CLI() { IMPL( niiCLI ); }
VOID CNesAssembly::RTS() { IMPL( niiRTS ); }
VOID CNesAssembly::PLA() { IMPL( niiPLA ); }
VOID CNesAssembly::SEI() { IMPL( niiSEI ); }
VOID CNesAssembly::DEY() { IMPL( niiDEY ); }
VOID CNesAssembly::TXA() { IMPL( niiTXA ); }
VOID CNesAssembly::TYA() { IMPL( niiTYA ); }
VOID CNesAssembly::TXS() { IMPL( niiTXS ); }
VOID CNesAssembly::TAY() { IMPL( niiTAY ); }
VOID CNesAssembly::TAX() { IMPL( niiTAX ); }
VOID CNesAssembly::CLV() { IMPL( niiCLV ); }
VOID CNesAssembly::TSX() { IMPL( niiTSX ); }
VOID CNesAssembly::INY() { IMPL( niiINY ); }
VOID CNesAssembly::DEX() { IMPL( niiDEX ); }
VOID CNesAssembly::CLD() { IMPL( niiCLD ); }
VOID CNesAssembly::INX() { IMPL( niiINX ); }
VOID CNesAssembly::NOP() { IMPL( niiNOP ); }
VOID CNesAssembly::SED() { IMPL( niiSED ); }
VOID CNesAssembly::ORA( NES_TARG arg ) { OPC( niiORA, arg ); }
VOID CNesAssembly::ASL( NES_TARG arg ) { OPC( niiASL, arg ); }
VOID CNesAssembly::AND( NES_TARG arg ) { OPC( niiAND, arg ); }
VOID CNesAssembly::BIT( NES_TARG arg ) { OPC( niiBIT, arg ); }
VOID CNesAssembly::ROL( NES_TARG arg ) { OPC( niiROL, arg ); }
VOID CNesAssembly::EOR( NES_TARG arg ) { OPC( niiEOR, arg ); }
VOID CNesAssembly::LSR( NES_TARG arg ) { OPC( niiLSR, arg ); }
VOID CNesAssembly::JMP( NES_TARG arg ) { OPC( niiJMP, arg ); }
VOID CNesAssembly::ADC( NES_TARG arg ) { OPC( niiADC, arg ); }
VOID CNesAssembly::ROR( NES_TARG arg ) { OPC( niiROR, arg ); }
VOID CNesAssembly::STA( NES_TARG arg ) { OPC( niiSTA, arg ); }
VOID CNesAssembly::STY( NES_TARG arg ) { OPC( niiSTY, arg ); }
VOID CNesAssembly::STX( NES_TARG arg ) { OPC( niiSTX, arg ); }
VOID CNesAssembly::LDA( NES_TARG arg ) { OPC( niiLDA, arg ); }
VOID CNesAssembly::LDY( NES_TARG arg ) { OPC( niiLDY, arg ); }
VOID CNesAssembly::LDX( NES_TARG arg ) { OPC( niiLDX, arg ); }
VOID CNesAssembly::CMP( NES_TARG arg ) { OPC( niiCMP, arg ); }
VOID CNesAssembly::CPX( NES_TARG arg ) { OPC( niiCPX, arg ); }
VOID CNesAssembly::CPY( NES_TARG arg ) { OPC( niiCPY, arg ); }
VOID CNesAssembly::DEC( NES_TARG arg ) { OPC( niiDEC, arg ); }
VOID CNesAssembly::INC( NES_TARG arg ) { OPC( niiINC, arg ); }
VOID CNesAssembly::SBC( NES_TARG arg ) { OPC( niiSBC, arg ); }