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

CWorldEditorDlg::CWorldEditorDlg( HINSTANCE hInstance )
	:
	CDialog( hInstance, 450, 250, TEXT( "Worlds configuration" ), TRUE ),
	m_cv( hInstance, 0x100, 120, 7, -7, -( 7 + 15 + 7 + 15 + 7 ), WS_VISIBLE | WS_TABSTOP | ES_READONLY, WS_EX_CLIENTEDGE ),
	m_tv( hInstance, 0x101, 7, 7, 100, -( 7 + 15 + 7 ), WS_VISIBLE | WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, TVS_EX_DOUBLEBUFFER ),
	m_btnAddLevel( hInstance, 0x200, TEXT( "Add level..." ), 7, -( 7 + 15 ), 50, 15, FALSE, TRUE ),
	m_btnRemoveLevel( hInstance, 0x201, TEXT( "Delete level" ), 7 + 50 + 7, -( 7 + 15 ), 50, 15, FALSE, TRUE ),
	m_stHalfPage( hInstance, TEXT( "Checkpoint page:" ), 120, -( 7 + 15 + 7 + 15 ) + 2, 70, 10 ),
	m_edHalfPage( hInstance, 0x300, nullptr, 200, -( 7 + 15 + 7 + 15 ), 30, 13, FALSE, TRUE ),
	m_udHalfPage( hInstance, 0x301, TRUE ),
	m_pKeeper( nullptr ), m_ptvSelected( nullptr ), m_fUpdatePending( FALSE ), m_fModified( FALSE )
{
	pushctl( m_tv );
	pushctl( m_btnAddLevel );
	pushctl( m_btnRemoveLevel );
	pushctl( m_stHalfPage );
	pushctl( m_edHalfPage );
	pushctl( m_udHalfPage );
	pushctl( m_cv );
	m_tvSelected.bArea = m_tvSelected.bWorld = m_tvSelected.bLevel = 0;
	m_tvSelected.tvi = tviWorld; m_tvSelected.pLevel = nullptr;
}

BOOL CWorldEditorDlg::OnInit( LPARAM lParam )
{
	UpdateTree();
	SetFocus( m_tv );
	m_tv.Visible( TRUE );
	return FALSE;
}

VOID CWorldEditorDlg::OnOK()
{
	Close( 1 );
}

VOID CWorldEditorDlg::OnButton( USHORT uId )
{
	switch ( uId )
	{
		case 0x200: NewLevel( m_ptvSelected ); break;
		case 0x201: DelLevel( m_ptvSelected ); break;
	}
}

VOID CWorldEditorDlg::OnCommand( USHORT uCmd, USHORT uId, HWND hCtl )
{
	if ( EN_CHANGE == uCmd && 0x300 == uId && m_ptvSelected )
	{
		if ( !m_udHalfPage.IsError() && !( m_ptvSelected->pLevel->GetLevelStatus() & 2 ) )
		{
			BYTE bPage = LOBYTE( m_udHalfPage.Value() );
			m_nagHP[ m_ptvSelected->bWorld ][ m_ptvSelected->bLevel ] = bPage;
			m_cv.ScrollTo( bPage );
		}
	}
}

INT_PTR CWorldEditorDlg::OnNotify( LPNMHDR lpnm )
{
	switch ( lpnm->idFrom )
	{
		case 0x101:
			{
				switch ( lpnm->code )
				{
					case TVN_SELCHANGED:
						{
							NMTREEVIEW * ptvi = (NMTREEVIEW *)lpnm;
							if ( !IsUpdatePending() )
							{
								_tprintf( TEXT( "ptvi->itemNew.state = 0x%08X\n" ), ptvi->itemNew.state );
								if ( ptvi->itemNew.state & TVIS_SELECTED ) 
								{
									SelectLevel( ptvi->itemNew.hItem, (PTVITEM_LEVEL)ptvi->itemNew.lParam );
								}								
							}
							break;
						}
				}
				break;
			}
	}

	return 0;
}

VOID CWorldEditorDlg::Set_Param( const NES_GAME_AREA_LIST & nag, const NES_GAME_AREA_HP_LIST & nhp, CNesLevelsKeeper * pKeeper, BYTE bWorld, BYTE bLevel, CNesLevel * pSelected )
{
	m_pKeeper = pKeeper;
	m_nagWorlds = nag;
	m_nagHP = nhp;
	m_tvSelected.bWorld = bWorld;
	m_tvSelected.bLevel = bLevel;
	m_tvSelected.pLevel = pSelected;
}

VOID CWorldEditorDlg::Get_Param( NES_GAME_AREA_LIST & nag, NES_GAME_AREA_HP_LIST & nhp, BYTE & bWorld, BYTE & bLevel, CNesLevel ** pLevel )
{
	nag = m_nagWorlds;
	nhp = m_nagHP;

	switch ( m_tvSelected.tvi )
	{
		case tviWorld:
			{
				*pLevel = nullptr;
				bLevel = 0;
				bWorld = LOBYTE( m_tvSelected.bWorld );
				break;
			}
		case tviLevel:
			{
				*pLevel = nullptr;
				bLevel = LOBYTE( m_tvSelected.bLevel );
				bWorld = LOBYTE( m_tvSelected.bWorld );
				break;
			}
		case tviArea:
			{
				*pLevel = m_tvSelected.pLevel;
				bLevel = LOBYTE( m_tvSelected.bLevel );
				bWorld = LOBYTE( m_tvSelected.bWorld );
				break;
			}
		default:
			{
				*pLevel = nullptr;
				bLevel = bWorld = 0;
				break;
			}
	}
}

BOOL CWorldEditorDlg::IsModified() const
{
	return m_fModified;
}

VOID CWorldEditorDlg::BeginUpdate()
{
	m_fUpdatePending = TRUE;
}

VOID CWorldEditorDlg::EndUpdate()
{
	m_fUpdatePending = FALSE;
}

BOOL CWorldEditorDlg::IsUpdatePending()
{
	return m_fUpdatePending;
}

VOID CWorldEditorDlg::UpdateTree()
{
	if ( IsUpdatePending() )
	{
		return;
	}

	BeginUpdate();
	FreeTVItems();
	m_tv.ResetContent();

	HTREEITEM hSelItem = nullptr;
	BYTE bWorldCounter = 0;

	for ( const auto & world : m_nagWorlds )
	{
		const BYTE bWorld = bWorldCounter++;
		const auto & levels = world;
		BYTE bLevelCounter = 0;
		PTVITEM_LEVEL pWorldItem = nullptr;
		HTREEITEM hWorldItem = AddItem( TVI_ROOT, pWorldItem = AddWorld( bWorld ), TRUE, TEXT( "World #%d" ), bWorld + 1 );

		if ( m_tvSelected.bWorld == bWorld )
		{
			hSelItem = hWorldItem;
			m_tvSelected.tvi = tviWorld;
		}

		for ( const auto & level : levels )
		{
			const BYTE bLevel = bLevelCounter++;
			const auto & vAreas = level;
			PTVITEM_LEVEL pLevelItem = nullptr;
			HTREEITEM hLevelItem = AddItem( hWorldItem, pLevelItem = AddLevel( bWorld, bLevel ), FALSE, TEXT( "Level #%d" ), bLevel + 1 );

			if ( m_tvSelected.bLevel == bLevel && m_tvSelected.bWorld == bWorld )
			{
				m_tvSelected.tvi = tviLevel;
				hSelItem = hLevelItem;
			}

			if ( vAreas.size() > 0 )
			{
				pLevelItem->pLevel = vAreas[ 0 ];

				if ( !pWorldItem->pLevel )
				{
					pWorldItem->pLevel = pLevelItem->pLevel;
				}
				
				if ( vAreas.size() > 1 )
				{
					size_t nArea = 0;
					for ( const auto pArea : vAreas )
					{
						PTVITEM_LEVEL pAreaItem = nullptr;
						HTREEITEM hAreaItem = AddItem( hLevelItem, pAreaItem = AddArea( bWorld, bLevel, nArea ), FALSE, TEXT( "Area #%u" ), nArea + 1 );
						if ( pArea == m_tvSelected.pLevel && m_tvSelected.bLevel == bLevel && m_tvSelected.bWorld == bWorld )
						{
							m_tvSelected.tvi = tviArea;
							hSelItem = hAreaItem;
						}

						nArea++;
						pAreaItem->pLevel = pArea;
					}
				}
			}
		}
	}

	if ( hSelItem )
	{
		m_tv.SetCurSel( hSelItem );
	}

	EndUpdate();

	SelectLevel( m_tv.GetCurSel() );
}

VOID CWorldEditorDlg::FreeTVItems()
{
	for ( auto & tvi : m_tvItem )
	{
		delete tvi.first;
	}

	m_tvItem.clear();
}

VOID CWorldEditorDlg::OnNCDestroy()
{
	FreeTVItems();
	delete m_cv.GetView();
}

HTREEITEM CWorldEditorDlg::AddItem( HTREEITEM hParentItem, PTVITEM_LEVEL pItem, BOOL fBold, LPCTSTR pszString, ... )
{
	va_list argptr;
	CString msg;
	va_start( argptr, pszString );
	msg.FormatV( pszString, argptr );
	va_end( argptr );

	HTREEITEM hItem = m_tv.AddItem( hParentItem, msg.GetString(), (LPARAM)pItem, fBold, FALSE, TRUE );
	m_tvItem[ pItem ] = hItem;
	
	return hItem;
}

PTVITEM_LEVEL CWorldEditorDlg::AddWorld( size_t bWorld )
{
	PTVITEM_LEVEL pItem = new TVITEM_LEVEL;
	pItem->pLevel = nullptr;
	pItem->bWorld = bWorld;
	pItem->tvi = tviWorld;
	return pItem;
}

PTVITEM_LEVEL CWorldEditorDlg::AddLevel( size_t bWorld, size_t bLevel )
{
	PTVITEM_LEVEL pItem = new TVITEM_LEVEL;
	pItem->pLevel = nullptr;
	pItem->bWorld = bWorld;
	pItem->bLevel = bLevel;
	pItem->tvi = tviLevel;
	return pItem;
}

PTVITEM_LEVEL CWorldEditorDlg::AddArea( size_t bWorld, size_t bLevel, size_t uArea )
{
	PTVITEM_LEVEL pItem = new TVITEM_LEVEL;
	pItem->pLevel = nullptr;
	pItem->bWorld = bWorld;
	pItem->bLevel = bLevel;
	pItem->bArea = uArea;
	pItem->tvi = tviArea;
	return pItem;
}

VOID CWorldEditorDlg::SelectLevel( HTREEITEM hItem )
{
	if ( hItem )
	{
		PTVITEM_LEVEL pItem = (PTVITEM_LEVEL)m_tv.Param( hItem );
		if ( pItem )
		{
			SelectLevel( hItem, pItem );
		}
	}
}

VOID CWorldEditorDlg::SelectLevel( HTREEITEM hItem, PTVITEM_LEVEL pItem )
{
	UNREFERENCED_PARAMETER( hItem );

	if ( pItem )
	{
		BYTE bWorld = LOBYTE( pItem->bWorld );
		delete m_cv.GetView();
		m_cv.SetView( m_pKeeper->GetInterface( bWorld, m_bmView, pItem->pLevel ) );
		//m_cv.RenderView();
		InvalidateRect( m_cv, nullptr, TRUE );
		m_btnAddLevel.Enable( pItem->tvi != tviArea );
		m_btnRemoveLevel.Enable( pItem->tvi == tviLevel );
		m_ptvSelected = pItem;
		m_tvSelected = *pItem;

		if ( !( pItem->pLevel->GetLevelStatus() & 2 ) )
		{
			m_edHalfPage.Enable( TRUE );
			m_udHalfPage.Enable( TRUE );
			m_udHalfPage.SetRange( pItem->pLevel->PagesCount() - 1, 0 );
			m_udHalfPage.Value( m_nagHP[ bWorld ][ pItem->bLevel ] );
			m_cv.ScrollTo( m_nagHP[ bWorld ][ pItem->bLevel ] );
		}
		else
		{
			m_edHalfPage.Enable( FALSE );
			m_udHalfPage.Enable( FALSE );
			m_udHalfPage.SetRange( 0, 0 );
			m_udHalfPage.Value( 0 );
		}
	}
	else
	{
		m_btnAddLevel.Enable( FALSE );
		m_btnRemoveLevel.Enable( FALSE );
		m_edHalfPage.Enable( FALSE );
		m_udHalfPage.Enable( FALSE );
		m_udHalfPage.SetRange( 0, 0 );
		m_udHalfPage.Value( 0 );
	}
}

VOID CWorldEditorDlg::NewLevel( PTVITEM_LEVEL pItem )
{
	if ( pItem )
	{
		if ( m_nagWorlds[ pItem->bWorld ].size() > 8 )
		{
			ShowMessage( TEXT( "Too many levels for this world." ) );
			return;
		}

		switch ( pItem->tvi )
		{
			case tviWorld: // insert to end
				{
					std::vector<CNesLevel*> vpLevel;
					if ( CreateLevelItem( LOBYTE(pItem->bWorld), pItem->pLevel, vpLevel ) )
					{
						m_nagWorlds[ pItem->bWorld ].push_back( vpLevel );
						m_nagHP[ pItem->bWorld ].push_back( 0 );	// default page is 0
						UpdateTree();
						m_fModified = TRUE;
					}
					break;
				}
			case tviLevel: // insert before!
				{
					std::vector<CNesLevel*> vpLevel;
					if ( CreateLevelItem( LOBYTE( pItem->bWorld ), pItem->pLevel, vpLevel ) )
					{
						m_nagWorlds[ pItem->bWorld ].insert( m_nagWorlds[ pItem->bWorld ].begin() + pItem->bLevel, vpLevel );
						m_nagHP[ pItem->bWorld ].insert( m_nagHP[ pItem->bWorld ].begin() + pItem->bLevel, 0 );	// sync half pages. Default page is 0
						UpdateTree();
						m_fModified = TRUE;
					}
					break;
				}
			default: break;
		}
	}
}

VOID CWorldEditorDlg::DelLevel( PTVITEM_LEVEL pItem )
{
	if ( pItem )
	{
		switch ( pItem->tvi )
		{
			case tviLevel:
				{
					if ( m_nagWorlds[ pItem->bWorld ].size() > 1 )
					{
						if ( Confirm( TEXT( "Are you sure that you want to remove selected level from the world?" ) ) )
						{
							m_nagWorlds[ pItem->bWorld ].erase( m_nagWorlds[ pItem->bWorld ].begin() + pItem->bLevel );
							m_nagHP[ pItem->bWorld ].erase( m_nagHP[ pItem->bWorld ].begin() + pItem->bLevel );	// sync half pages
							UpdateTree();
							m_fModified = TRUE;
						}
					}
					else
					{
						ShowError( TEXT( "Unable to delete the last level from the world!" ) );
					}
					break;
				}
			default: break;
		}
	}
}

BOOL CWorldEditorDlg::CreateLevelItem( BYTE bWorld, CNesLevel * pSelectedLevel, std::vector<CNesLevel*>& vpLevel )
{
	CAreasDlg dlg( GetInstance(), m_pKeeper );

	vpLevel.clear();
	dlg.Set_Obj( bWorld, pSelectedLevel, nullptr, 0 );

	if ( dlg.Show( this ) )
	{
		BYTE dummy1;
		BOOL dummy2;
		dlg.Get_Param( &pSelectedLevel, dummy1, dummy2 );

		vpLevel.push_back( pSelectedLevel );
		if ( pSelectedLevel->GetLevelStatus() & 2 )
		{
			if ( Confirm( TEXT( "Auto level requires next area to proceed. Do you want to select next area?" ) ) )
			{
				CAreasDlg dlg2( GetInstance(), m_pKeeper, TRUE );
				dlg2.Set_Obj( bWorld, pSelectedLevel, nullptr, 0 );

				if ( dlg2.Show( this ) )
				{
					dlg2.Get_Param( &pSelectedLevel, dummy1, dummy2 );

					if ( pSelectedLevel->GetLevelStatus() & 2 )
					{
						ShowErrorF( TEXT( "Can't set second area as auto level [0x%02X]" ), pSelectedLevel->AreaPtr().bPtr );
						return FALSE;
					}

					vpLevel.push_back( pSelectedLevel );
					return TRUE;
				}
			}
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}