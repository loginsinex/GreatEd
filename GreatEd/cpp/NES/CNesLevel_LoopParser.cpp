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

VOID CNesLevel::ParseLoops()
{
	std::vector<NES_LOOP_COMMAND> vlp;
	if ( m_loop.LoopList( this, vlp ) )
	{
		// dump loops
		std::map<int, CNesLoopCommand*> mLoop;
		for ( auto it = m_obj.vpItem.begin() + 1; it != m_obj.vpItem.end(); ++it )
		{
			CNesLevelItem * pObj = *it;
			if ( 0x4B == pObj->Item().item.itemId )
			{
				if ( NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
				{
					mLoop[ pObj->GetObjectPoint().x / 16 ] = static_cast<CNesLoopCommand*>( pObj );
				}
			}
		}

		std::sort( vlp.begin(), vlp.end(), [] ( const NES_LOOP_COMMAND & nlp1, const NES_LOOP_COMMAND & nlp2 ) { return nlp1.bPage < nlp2.bPage; } );

		for ( const auto lp : vlp )
		{
			auto pgIt = mLoop.find( lp.bPage );
			for ( auto & loop : mLoop )
			{
				if ( !loop.second->IsExternalSet() && loop.first <= static_cast<int>( lp.bPage ) )
				{
					loop.second->SetLoop( lp );
					_tprintf( TEXT( "Area: 0x%02X, Page: %d\n" ), m_lvlPtr.bPtr, lp.bPage );
					break;
				}
			}
		}
	}
}

VOID CNesLevel::CopyLoops( CNesLevel * pCopy )
{
	// we need to copy CURRENT loops instead of loops from file
	size_t counter = 0;
	std::vector<CNesLoopCommand*> vpCurrent, vpCopy;
	GetLoopObjects( vpCurrent );
	pCopy->GetLoopObjects( vpCopy );

	if ( vpCurrent.size() == vpCopy.size() )
	{
		size_t c = vpCurrent.size();
		for ( size_t i = 0; i < c; ++i )
		{
			CNesLoopCommand * pLoop = vpCurrent[ i ];
			CNesLoopCommand * pLoopCopy = vpCopy[ i ];

			NES_LOOP_COMMAND lp;
			pLoop->GetLoop( lp );
			pLoopCopy->SetLoop( lp );
		}
	}
}

VOID CNesLevel::DumpLoops( std::map<BYTE, std::vector<CNesLoopCommand*>>& mLoop )
{
	mLoop.clear();

	for ( auto it = m_obj.vpItem.begin() + 1; it != m_obj.vpItem.end(); ++it )
	{
		CNesLevelItem * pObj = *it;
		if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
		{
			CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pObj );
			if ( pLoop->TriggerPage() >= 4 && !pLoop->LoopConnected() )
			{
				NES_LOOP_COMMAND nlp = { 0 };
				mLoop[ pLoop->TriggerPage() - 4 ].push_back( pLoop );
				pLoop->GetLoop( nlp );
			}
		}
	}
}

VOID CNesLevel::GetLoopObjects( std::vector<CNesLoopCommand*>& vItemLoop )
{
	vItemLoop.clear();
	vItemLoop.reserve( m_obj.vpItem.size() - 1 );

	for ( auto it = m_obj.vpItem.begin() + 1; it != m_obj.vpItem.end(); ++it )
	{
		CNesLevelItem * pObj = *it;
		if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
		{
			CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pObj );
			vItemLoop.push_back( pLoop );
		}
	}
}

VOID CNesLevel::TakeLoopsSnapshot( std::vector<NES_LOOP_COMMAND>& vLoop )
{
	vLoop.clear();
	vLoop.reserve( m_obj.vpItem.size() - 1 );

	for ( auto it = m_obj.vpItem.begin() + 1; it != m_obj.vpItem.end(); ++it )
	{
		CNesLevelItem * pObj = *it;
		if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
		{
			CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pObj );
			NES_LOOP_COMMAND nlp = { 0 };
			pLoop->GetLoop( nlp );
			vLoop.push_back( nlp );
		}
	}
}

VOID CNesLevel::RevertLoopsState( const std::vector<NES_LOOP_COMMAND>& vLoop )
{
	size_t cLoops = vLoop.size();
	size_t nLoop = 0;
	for ( auto it = m_obj.vpItem.begin() + 1; it != m_obj.vpItem.end(); ++it )
	{
		CNesLevelItem * pObj = *it;
		if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
		{
			CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pObj );
			if ( nLoop < cLoops )
			{
				const NES_LOOP_COMMAND & nlp = vLoop[ nLoop++ ];
				pLoop->SetLoop( nlp );
			}
			else
			{
				break;
			}
		}
	}
}

BOOL CNesLevel::FindLoopObjectId( CNesLoopCommand * pObject, size_t & idObject )
{
	size_t counter = 1;
	NES_LOOP_COMMAND lp;
	pObject->GetLoop( lp );
	for ( auto it = m_obj.vpItem.begin() + 1; it < m_obj.vpItem.end(); ++it, ++counter )
	{
		CNesLevelItem * pItem = *it;
		NES_LOOP_COMMAND lpFind;
		if ( 0x4B == pItem->Item().item.itemId && NES_SPECIAL_OBJECT_D == pItem->Item().item.Y )
		{
			CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pItem );
			pLoop->GetLoop( lpFind );
			if ( lpFind == lp )
			{
				idObject = counter;
				return TRUE;
			}
		}
	}

	return FALSE;
}
