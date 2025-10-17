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

#define NES_UNDEFINED_LABEL			size_t(-1)

typedef enum _tagNES_INSTRUCTION_TYPE
{
	nitACC = 1,
	nitABS = 2,
	nitABSX = 3,
	nitABSY = 4,
	nitIMM = 5,
	nitIND = 6,
	nitX_IND = 7,
	nitIND_Y = 8,
	nitREL = 9,
	nitZPG = 10,
	nitZPG_X = 11,
	nitZPG_Y = 12,
	nitIMPL = 13,
	nitLABEL = 14
} NES_INSTRUCTION_TYPE, *PNES_INSTRUCTION_TYPE;

typedef enum _tagNES_INSTRUCTION_ID
{
	niiBRK,
	niiORA,
	niiASL,
	niiPHP,
	niiBPL,
	niiCLC,
	niiCLV,
	niiCLD,
	niiJSR,
	niiAND,
	niiBIT,
	niiROL,
	niiPLP,
	niiBMI,
	niiSEC,
	niiRTI,
	niiEOR,
	niiLSR,
	niiPHA,
	niiJMP,
	niiBVC,
	niiCLI,
	niiRTS,
	niiADC,
	niiROR,
	niiPLA,
	niiBVS,
	niiSEI,
	niiSTA,
	niiSTY,
	niiSTX,
	niiDEY,
	niiTXA,
	niiTYA,
	niiTAX,
	niiTAY,
	niiBCC,
	niiTXS,
	niiTSX,
	niiLDA,
	niiLDY,
	niiLDX,
	niiCMP,
	niiCPX,
	niiCPY,
	niiDEC,
	niiDEX,
	niiINC,
	niiINX,
	niiINY,
	niiSBC,
	niiBCS,
	niiBNE,
	niiBEQ,
	niiSED,
	niiNOP,
	niiLABEL	// fake instruction for label
} NES_INSTRUCTION_ID, *PNES_INSTRUCTION_ID;

typedef enum _tagNES_INSTRUCTION_OPCODE : BYTE
{
	niocBRK_IMPL = 0x00,
	niocPHP_IMPL = 0x08,
	niocCLC_IMPL = 0x18,
	niocPLP_IMPL = 0x28,
	niocSEC_IMPL = 0x38,
	niocRTI_IMPL = 0x40,
	niocPHA_IMPL = 0x48,
	niocCLI_IMPL = 0x58,
	niocRTS_IMPL = 0x60,
	niocPLA_IMPL = 0x68,
	niocSEI_IMPL = 0x78,
	niocDEY_IMPL = 0x88,
	niocTXA_IMPL = 0x8A,
	niocTYA_IMPL = 0x98,
	niocTXS_IMPL = 0x9A,
	niocTAY_IMPL = 0xA8,
	niocTAX_IMPL = 0xAA,
	niocCLV_IMPL = 0xB8,
	niocTSX_IMPL = 0xBA,
	niocINY_IMPL = 0xC8,
	niocDEX_IMPL = 0xCA,
	niocCLD_IMPL = 0xD8,
	niocINX_IMPL = 0xE8,
	niocNOP_IMPL = 0xEA,
	niocSED_IMPL = 0xF8,

	niocBPL_REL = 0x10,
	niocBMI_REL = 0x30,
	niocBVC_REL = 0x50,
	niocBVS_REL = 0x70,
	niocBCC_REL = 0x90,
	niocBCS_REL = 0xB0,
	niocBNE_REL = 0xD0,
	niocBEQ_REL = 0xF0,

	niocJSR_ABS = 0x20,

	niocJMP_ABS = 0x4C,
	niocJMP_IND = 0x6C,

	niocORA_X_IND = 0x01,
	niocORA_IND_Y = 0x11,
	niocORA_ZPG = 0x05,
	niocORA_ZPG_X = 0x15,
	niocORA_IMM = 0x09,
	niocORA_ABS = 0x0D,
	niocORA_ABS_Y = 0x19,
	niocORA_ABS_X = 0x1D,

	niocASL_ACC = 0x0A,
	niocASL_ZPG = 0x06,
	niocASL_ZPG_X = 0x16,
	niocASL_ABS = 0x0E,
	niocASL_ABS_X = 0x1E,

	niocAND_X_IND = 0x21,
	niocAND_IND_Y = 0x31,
	niocAND_ZPG = 0x25,
	niocAND_ZPG_X = 0x35,
	niocAND_IMM = 0x29,
	niocAND_ABS_Y = 0x39,
	niocAND_ABS = 0x2D,
	niocAND_ABS_X = 0x3D,

	niocBIT_ABS = 0x24,
	niocBIT_ZPG = 0x2C,

	niocROL_ZPG = 0x26,
	niocROL_ACC = 0x2A,
	niocROL_ABS = 0x2E,
	niocROL_ZPG_X = 0x36,
	niocROL_ABS_X = 0x3E,

	niocEOR_X_IND = 0x41,
	niocEOR_IND_Y = 0x51,
	niocEOR_ZPG = 0x45,
	niocEOR_ZPG_X = 0x55,
	niocEOR_IMM = 0x49,
	niocEOR_ABS = 0x4D,
	niocEOR_ABS_Y = 0x59,
	niocEOR_ABS_X = 0x5D,

	niocLSR_ZPG = 0x46,
	niocLSR_ZPG_X = 0x56,
	niocLSR_ACC = 0x4A,
	niocLSR_ABS = 0x4E,
	niocLSR_ABS_X = 0x5E,

	niocADC_X_IND = 0x61,
	niocADC_IND_Y = 0x71,
	niocADC_ZPG = 0x65,
	niocADC_ZPG_X = 0x75,
	niocADC_IMM = 0x69,
	niocADC_ABS_Y = 0x79,
	niocADC_ABS = 0x6D,
	niocADC_ABS_X = 0x7D,

	niocROR_ZPG = 0x66,
	niocROR_ZPG_X = 0x76,
	niocROR_ACC = 0x6A,
	niocROR_ABS = 0x6E,
	niocROR_ABS_X = 0x7E,

	niocSTA_X_IND = 0x81,
	niocSTA_IND_Y = 0x91,
	niocSTA_ZPG = 0x85,
	niocSTA_ZPG_X = 0x95,
	niocSTA_ABS_Y = 0x99,
	niocSTA_ABS = 0x8D,
	niocSTA_ABS_X = 0x9D,

	niocSTY_ZPG = 0x84,
	niocSTY_ZPG_X = 0x94,
	niocSTY_ABS = 0x8C,

	niocSTX_ZPG = 0x86,
	niocSTX_ZPG_Y = 0x96,
	niocSTX_ABS = 0x8E,

	niocLDA_X_IND = 0xA1,
	niocLDA_IND_Y = 0xB1,
	niocLDA_ZPG = 0xA5,
	niocLDA_ZPG_X = 0xB5,
	niocLDA_IMM = 0xA9,
	niocLDA_ABS = 0xAD,
	niocLDA_ABS_Y = 0xB9,
	niocLDA_ABS_X = 0xBD,

	niocLDY_IMM = 0xA0,
	niocLDY_ZPG = 0xA4,
	niocLDY_ZPG_X = 0xB4,
	niocLDY_ABS = 0xAC,
	niocLDY_ABS_X = 0xBC,

	niocLDX_IMM = 0xA2,
	niocLDX_ZPG = 0xA6,
	niocLDX_ZPG_Y = 0xB6,
	niocLDX_ABS = 0xAE,
	niocLDX_ABS_Y = 0xBE,

	niocCMP_X_IND = 0xC1,
	niocCMP_IND_Y = 0xD1,
	niocCMP_ZPG = 0xC5,
	niocCMP_ZPG_X = 0xD5,
	niocCMP_IMM = 0xC9,
	niocCMP_ABS_Y = 0xD9,
	niocCMP_ABS = 0xCD,
	niocCMP_ABS_X = 0xDD,

	niocCPX_IMM = 0xE0,
	niocCPX_ZPG = 0xE4,
	niocCPX_ABS = 0xEC,

	niocCPY_IMM = 0xC0,
	niocCPY_ZPG = 0xC4,
	niocCPY_ABS = 0xCC,

	niocDEC_ZPG = 0xC6,
	niocDEC_ZPG_X = 0xD6,
	niocDEC_ABS = 0xCE,
	niocDEC_ABS_X = 0xDE,
	
	niocINC_ZPG = 0xE6,
	niocINC_ZPG_X = 0xF6,
	niocINC_ABS = 0xEE,
	niocINC_ABS_X = 0xFE,

	niocSBC_X_IND = 0xE1,
	niocSBC_IND_Y = 0xF1,
	niocSBC_ZPG = 0xE5,
	niocSBC_ZPG_X = 0xF5,
	niocSBC_IMM = 0xE9,
	niocSBC_ABS_Y = 0xF9,
	niocSBC_ABS = 0xED,
	niocSBC_ABS_X = 0xFD		
} NES_INSTRUCTION_OPCODE, *PNES_INSTRUCTION_OPCODE;

typedef std::map<NES_INSTRUCTION_TYPE, NES_INSTRUCTION_OPCODE>		NES_INSTRUCTION_CODE_MAP, *PNES_INSTRUCTION_CODE_MAP;
typedef std::map<NES_INSTRUCTION_ID, NES_INSTRUCTION_CODE_MAP>		NES_INSTRUCTION_MAP, *PNES_INSTRUCTION_MAP;
typedef USHORT														NES_OFFSET, *PNES_OFFSET;
typedef USHORT														NES_ARG, *PNES_ARG;
typedef DWORD														NES_TARG, *PNES_TARG;
typedef size_t														NES_LABEL, *PNES_LABEL;

typedef struct _tagNES_OPCODE
{
	NES_INSTRUCTION_TYPE			type;
	NES_INSTRUCTION_ID				id;
	NES_INSTRUCTION_OPCODE			opcode;
	NES_ARG							arg;
	NES_OFFSET						size;
	NES_OFFSET						pc;
	NES_LABEL						line;
} NES_OPCODE, *PNES_OPCODE;

typedef std::vector<NES_OPCODE>			NES_OPCODE_LIST, *PNES_OPCODE_LIST;
typedef std::map<CString, NES_LABEL>	NES_LABEL_LIST, *PNES_LABEL_LIST;
typedef std::vector<CString>			NES_LABEL_MAP, *PNES_LABEL_MAP;

#define INSTR_ACC()			MAKELONG( 0, nitACC )
#define INSTR_ABS( arg )	MAKELONG( ( arg ), nitABS )
#define INSTR_ABSX( arg )	MAKELONG( ( arg ), nitABSX )
#define INSTR_ABSY( arg )	MAKELONG( ( arg ), nitABSY )
#define INSTR_IMM( arg )	MAKELONG( ( arg ), nitIMM )
#define INSTR_IND( arg )	MAKELONG( ( arg ), nitIND )
#define INSTR_X_IND( arg )	MAKELONG( ( arg ), nitX_IND )
#define INSTR_IND_Y( arg )	MAKELONG( ( arg ), nitIND_Y )
#define INSTR_REL( arg )	MAKELONG( ( arg ), nitREL )
#define INSTR_ZPG( arg )	MAKELONG( ( arg ), nitZPG )
#define INSTR_ZPG_X( arg )	MAKELONG( ( arg ), nitZPG_X )
#define INSTR_ZPG_Y( arg )	MAKELONG( ( arg ), nitZPG_Y )
#define INSTR_LABEL( arg )	MAKELONG( ( arg ), nitLABEL )

class CNesAssemblyCompileError
{
	const CString				m_compileError;
	const size_t				m_uLine;

public:
	CNesAssemblyCompileError( const CString & sError, size_t uLine );

	const CString & Error() const;
	size_t Line() const;
};

class CNesAssembly
{
	NES_OFFSET			m_uBase;
	NES_OFFSET			m_uOffset;
	NES_OPCODE_LIST		m_vOpCode;
	NES_LABEL_LIST		m_mLabel;
	NES_LABEL_MAP		m_vLabel;	// branch jumps

	NES_OFFSET			OpcodeSize( NES_INSTRUCTION_TYPE type ) const;
	NES_OPCODE &		NewOpcode( NES_INSTRUCTION_TYPE optype );
	NES_OPCODE &		NewOpcode( NES_INSTRUCTION_ID id, NES_INSTRUCTION_TYPE type );
	VOID				ModifyOpcode( NES_INSTRUCTION_ID id, NES_INSTRUCTION_TYPE type );
	USHORT				GetLabel( const CString & sLabel );	// for branch instructions

	NES_OFFSET			Offset() const;
	NES_LABEL			LastLabel() const;

	VOID				REL( NES_INSTRUCTION_ID id, const CString & sLabel );
	VOID				ABSLB( NES_INSTRUCTION_ID id, const CString & sLabel );
	VOID				IMPL( NES_INSTRUCTION_ID id );
	VOID				OPC( NES_INSTRUCTION_ID id, DWORD dwArg );

	const NES_OPCODE &	GetLabelOpcode( NES_OFFSET label_id ) const;
	VOID				CompileAbsLb( const NES_OPCODE & opcode, std::vector<BYTE> & code ) const;
	VOID				CompileRelLb( const NES_OPCODE & opcode, std::vector<BYTE> & code ) const;
	VOID				CompileRel( const NES_OPCODE & opcode, std::vector<BYTE> & code ) const;
	VOID				CompileOpc( const NES_OPCODE & opcode, std::vector<BYTE> & code ) const;

public:
	CNesAssembly( NES_OFFSET uBase = 0x8000 );
	VOID			Rebase( NES_OFFSET uBase );
	NES_OFFSET		Base() const;
	VOID			label( const CString & sLabel );
	NES_OFFSET		LabelPC( const CString & sLabel ) const;
	VOID			clear();
	NES_OFFSET		size() const;
	VOID			dump_labels() const;

	VOID			JSR( const CString & sLabel );
	VOID			JSR( NES_OFFSET arg );
	VOID			JMP( const CString & sLabel );
	VOID			BPL( const CString & sLabel );
	VOID			BMI( const CString & sLabel );
	VOID			BVC( const CString & sLabel );
	VOID			BVS( const CString & sLabel );
	VOID			BCC( const CString & sLabel );
	VOID			BCS( const CString & sLabel );
	VOID			BNE( const CString & sLabel );
	VOID			BEQ( const CString & sLabel );

	VOID			BRK();
	VOID			PHP();
	VOID			CLC();
	VOID			PLP();
	VOID			SEC();
	VOID			RTI();
	VOID			PHA();
	VOID			CLI();
	VOID			RTS();
	VOID			PLA();
	VOID			SEI();
	VOID			DEY();
	VOID			TXA();
	VOID			TYA();
	VOID			TXS();
	VOID			TAY();
	VOID			TAX();
	VOID			CLV();
	VOID			TSX();
	VOID			INY();
	VOID			DEX();
	VOID			CLD();
	VOID			INX();
	VOID			NOP();
	VOID			SED();

	VOID			ORA( NES_TARG arg );
	VOID			ASL( NES_TARG arg );
	VOID			AND( NES_TARG arg );
	VOID			BIT( NES_TARG arg );
	VOID			ROL( NES_TARG arg );
	VOID			EOR( NES_TARG arg );
	VOID			LSR( NES_TARG arg );
	VOID			JMP( NES_TARG arg );
	VOID			ADC( NES_TARG arg );
	VOID			ROR( NES_TARG arg );
	VOID			STA( NES_TARG arg );
	VOID			STY( NES_TARG arg );
	VOID			STX( NES_TARG arg );
	VOID			LDA( NES_TARG arg );
	VOID			LDY( NES_TARG arg );
	VOID			LDX( NES_TARG arg );
	VOID			CMP( NES_TARG arg );
	VOID			CPX( NES_TARG arg );
	VOID			CPY( NES_TARG arg );
	VOID			DEC( NES_TARG arg );
	VOID			INC( NES_TARG arg );
	VOID			SBC( NES_TARG arg );

	VOID			Compile( std::vector<BYTE> & code ) const;
};