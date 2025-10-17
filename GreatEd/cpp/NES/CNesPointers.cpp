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

CNesPointers::CNesPointers( CNesSimpleMMC3 & mmc3 )
	: m_file( mmc3 )
{
	Version( ver_E );
}

VOID CNesPointers::Version( NES_VERSION id )
{
	m_uVersion = id;
	m_eptr.clear();
	LoadPointers();
}

VOID CNesPointers::LoadSharedPointers()
{
	m_eptr[ eSharedMusicPtr ].eptr = NES_S_MUSIC_PTR;
//	m_eptr[ eSharedCodePtr ].eptr = NES_S_SHARED_CODE_PTR;
	m_eptr[ eSharedNMIPtr ].eptr = NES_S_NMI;
	m_eptr[ eSharedRESETPtr ].eptr = NES_S_RESET;
	m_eptr[ eSharedIRQPtr ].eptr = NES_S_IRQ;
	m_eptr[ eSharedMMC3NMI ].eptr = NES_S_MMC3_NMI;
	m_eptr[ eSharedMMC3RESET ].eptr = NES_S_MMC3_RESET;
	m_eptr[ eSharedMMC3IRQ ].eptr = NES_S_MMC3_IRQ;
	m_eptr[ eSharedSoundEnginePtr ].eptr = NES_S_SOUND_ENGINE_PTR;
	m_eptr[ eSharedRESETEndlessJmp ].eptr = NES_S_RESET_ENDLESS_JMP;
//	m_eptr[ eSharedAltGetAreaAddr ].eptr = NES_S_ALT_GET_AREA_ADDR;
//	m_eptr[ eSharedAltProcessAreaData ].eptr = NES_S_ALT_PROCESS_AREA_DATA;
	m_eptr[ eSharedEnemyPtrLo ].eptr = NES_S_ENEMY_PTR_LO;
//	m_eptr[ eSharedAltEnemyCore ].eptr = NES_S_ALT_ENEMY_CORE;
//	m_eptr[ eSharedAltFindAreaPointer ].eptr = NES_S_ALT_FIND_AREA_POINTER;
	m_eptr[ eSharedAltPwrUpJmpPtr ].eptr = NES_S_SBR_PWRUPJMP;
	m_eptr[ eSharedAltSetupVinePtr ].eptr = NES_S_SBR_SETUP_VINE;
	m_eptr[ eSharedSaveAreaPtr ].eptr = NES_S_SAVE_AREA_PTR;
//	m_eptr[ eSharedAreaBanks ].eptr = NES_S_AREA_BANKS;
	m_eptr[ eSharedLoopCommands ].eptr = NES_S_LOOP_COMMANDS;
	m_eptr[ eSharedLoopHandler ].eptr = NES_S_LOOP_HANDLER_PTR;
	m_eptr[ eSharedHalfPages ].eptr = NES_S_AREA_HALFPAGES;
	m_eptr[ eSharedDemoData ].eptr = NES_S_DEMO_DATA;
}

VOID CNesPointers::LoadPointers()
{
	LoadSharedPointers();

	switch ( m_uVersion )
	{
		case ver_E: LoadPointersE(); break;
		case ver_I: LoadPointersI(); break;
		case ver_X: LoadPointersX(); break;
		default: throw std::exception( "Unknown version" ); break;
	}
}

NES_EPOINTER CNesPointers::operator[]( NES_EPOINTERS eptr ) const
{
	auto ptr = m_eptr.find( eptr );
	if ( ptr == m_eptr.end() )
	{
		throw std::exception( "No such pointer" );
	}

	return ptr->second;
}

VOID CNesPointers::Select( NES_EPOINTERS eptr ) const
{
	if ( !const_cast<CNesSimpleMMC3*>( &m_file )->IsFileLoaded() )
	{
		throw std::exception( "Trying to read from an empty file" );
	}
	
	auto e = this->operator[]( eptr );

	if ( e.bHiBank != 0xFF )
	{
		const_cast<CNesSimpleMMC3*>( &m_file )->SelectPRG( e.bHiBank, FALSE );
	}

	if ( e.bLoBank != 0xFF )
	{
		const_cast<CNesSimpleMMC3*>( &m_file )->SelectPRG( e.bLoBank, TRUE );
	}
}

USHORT & CNesPointers::Pointer( NES_EPOINTERS eptr ) const
{
	auto e = this->operator[]( eptr );
	Select( eptr );

	return m_file.Data<USHORT>( e.ptr );
}