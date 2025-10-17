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

#include "GUI\GUI.h"

CUnusedAreasDlg::CUnusedAreasDlg( HINSTANCE hInstance )
	: CDialog( hInstance, 450, 200, TEXT( "Unused areas" ), FALSE ),
	m_cv( hInstance, 0x100, 160, 7, -7, -( 7 + 15 + 7 ), WS_VISIBLE | WS_TABSTOP | ES_READONLY, WS_EX_CLIENTEDGE ),
	m_lv( hInstance, 0x101, 7, 7, 150, -( 7 + 15 + 7 ), WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | WS_BORDER ),
	m_btnRemove( hInstance, IDOK, TEXT( "Remove" ), -( 7 + ( 50 + 7 ) + 50 ), -( 7 + 15 ), 50, 15, TRUE ),
	m_btnLeave( hInstance, IDCANCEL, TEXT( "Leave" ), -( 7 + 50 ), -( 7 + 15 ), 50, 15 ),
	m_pKeeper( nullptr )
{
	pushctl( m_lv );
	pushctl( m_btnRemove );
	pushctl( m_btnLeave );
	pushctl( m_cv );
}

BOOL CUnusedAreasDlg::OnInit( LPARAM lParam )
{
	LPCTSTR pszGroup[] = { TEXT( "Water" ), TEXT( "Ground" ), TEXT( "Underground" ), TEXT( "Castle" ) };
	INT id = 0;

	m_lv.ExtendedStyle( LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
	m_lv.AddColumn( TEXT( "Area" ) );
	m_lv.EnableGroups( TRUE );
	
	for ( const auto & pszGrp : pszGroup )
	{
		m_lv.AddGroup( id++, pszGrp );
	}

	for ( auto pLevel : m_vpUnusedLevels )
	{
		CString strArea;
		strArea.Format( TEXT( "%s #%d" ), pszGroup[ pLevel->AreaType() ], pLevel->AreaPtr().bAreaId + 1 );

		INT_PTR idLevel = m_lv.AddItem( strArea, (LPARAM)pLevel, pLevel->AreaType() );
		m_lv.Check( LODWORD( idLevel ), TRUE );
	}

	InvalidateRect( m_lv, nullptr, TRUE );
	SetFocus( m_btnRemove );
	RenderSelected();
	return FALSE;
}

VOID CUnusedAreasDlg::OnOK()
{
	m_vpCheckedLevels.clear();
	INT cSize = LODWORD( m_lv.Count() );
	for ( INT i = 0; i < cSize; ++i )
	{
		if ( m_lv.Check( LODWORD( i ) ) )
		{
			CNesLevel * pLevel = (CNesLevel*)m_lv.Param( i );
			if ( pLevel )
			{
				m_vpCheckedLevels.push_back( pLevel );
			}
		}
	}

	Close( 1 );
}

VOID CUnusedAreasDlg::OnDestroy()
{
	FreeView();
}

VOID CUnusedAreasDlg::FreeView()
{
	CNesLevelInterface * pInterface = m_cv.GetView();
	m_cv.DelView();
	delete pInterface;
}

VOID CUnusedAreasDlg::RenderSelected()
{
	CNesLevel * pLevel = (CNesLevel*)m_lv.Param( LODWORD( m_lv.CurSel() ) );
	if ( pLevel )
	{
		CNesLevelInterface * pInterface = m_pKeeper->GetInterface( 0, m_bmView, pLevel );
		FreeView();
		m_cv.SetView( pInterface );
		InvalidateRect( m_cv, nullptr, TRUE );
	}	
}

INT_PTR CUnusedAreasDlg::OnNotify( LPNMHDR lpnm )
{
	switch ( lpnm->idFrom )
	{
		case 0x101:
			{
				switch ( lpnm->code )
				{
					case LVN_ITEMCHANGED:
						{
							NMLISTVIEW * pnmlv = (NMLISTVIEW*)lpnm;
							if ( ( pnmlv->uChanged & LVIF_STATE ) && ( pnmlv->uNewState & LVIS_SELECTED ) )
							{
								RenderSelected();
							}
							break;
						}
				}
			}
	}

	return 0;
}

VOID CUnusedAreasDlg::Set_Param( CNesLevelsKeeper * pKeeper, const std::vector<CNesLevel*>& vpUnusedLevels )
{
	m_pKeeper = pKeeper;
	m_vpUnusedLevels = vpUnusedLevels;
}

VOID CUnusedAreasDlg::Get_Param( std::vector<CNesLevel*>& vpUnusedLevels )
{
	vpUnusedLevels = m_vpCheckedLevels;
}