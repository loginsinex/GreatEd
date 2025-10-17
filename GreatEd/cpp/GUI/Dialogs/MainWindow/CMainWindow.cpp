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

CMainWindow::CMainWindow( HINSTANCE hInstance )
	: CWindow( hInstance, TEXT( "GreatEd_class" ) ),
	m_status( hInstance, IDCU_MAINWINDOW_STATUS, TRUE, TRUE, TRUE ),
	m_menu( hInstance, IDC_PROJECT ),
	m_tb( hInstance, IDCU_MAINWINDOW_TOOLBAR, 0, 0, 0, 0, TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | TBSTYLE_TOOLTIPS | WS_TABSTOP | WS_VISIBLE ),
	m_rb( hInstance, IDCU_MAINWINDOW_REBAR, 0, 0, 0, 0, WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_AUTOSIZE | CCS_TOP | CCS_ADJUSTABLE | RBS_BANDBORDERS | WS_VISIBLE ),
	m_tc( hInstance, IDCU_MAINWINDOW_TAB, 0, 0, 0, 0, WS_TABSTOP | WS_VISIBLE | TCS_HOTTRACK | TCS_MULTILINE | TCS_TOOLTIPS ),
	m_nNames( hInstance ), m_dlgDemo( hInstance ), m_pEditor( nullptr ),
	m_pctab( nullptr )
{
	Register( CS_DBLCLKS, NULL, LoadCursor( NULL, IDC_ARROW ), (HBRUSH)( COLOR_BTNFACE + 1 ) );

	pushctl( m_status );
	pushctl( m_tb );
	pushctl( m_rb );
	pushctl( m_tc );
}

BOOL CMainWindow::OnCreate( LPCREATESTRUCT lpcs )
{
	CPngLoader pngIcons( GetInstance(), MAKEINTRESOURCE( IDB_ICONS ), TEXT( "PNG" ) );
	CBitmap bmIcons;
	HBITMAP hBmIcons = nullptr;
	HICON hWndIcon = LoadIcon( GetInstance(), MAKEINTRESOURCE( IDI_SMALL ) );
	SetMenu(*this, m_menu);

	pngIcons.Bitmap( bmIcons );
	hBmIcons = bmIcons.Bitmap();

	wSendMessage( WM_SETICON, ICON_BIG, (LPARAM)hWndIcon );
	wSendMessage( WM_SETICON, ICON_SMALL, (LPARAM)hWndIcon );

	m_tb.SetImageList( hBmIcons );

	m_tb.AddButton( 12, ID_FILE_OPEN, nullptr, 0, TRUE, IDS_FILE_OPEN );
	m_tb.AddButton( 13, ID_FILE_SAVE, nullptr, 0, FALSE, IDS_FILE_SAVE );
	m_tb.AddSeparator();
	m_tb.AddButton( 11, ID_EDIT_INSERTOBJECT, nullptr, 0, FALSE, IDS_EDIT_INSERTOBJECT );
	m_tb.AddButton( 10, ID_EDIT_DELETEOBJECT, nullptr, 0, FALSE, IDS_EDIT_DELETEOBJECT );
	m_tb.AddSeparator();
	m_tb.AddButton( 29, ID_EDIT_CREATENEWAREA, nullptr, 0, FALSE, IDS_EDIT_CREATENEWAREA );
	m_tb.AddButton( 21, ID_EDIT_EDITWORLDCONFIGURATION, nullptr, 0, FALSE, IDS_EDIT_EDITWORLDCONFIGURATION );
	m_tb.AddSeparator();
	m_tb.AddButton( 4, ID_VIEW_SELECTLEVEL, nullptr, 0, FALSE, IDS_VIEW_SELECTLEVEL );
	m_tb.AddButton( 1, ID_COMMAND_RUNEMULATORWITHROM, nullptr, 0, FALSE, IDS_COMMAND_RUNEMULATORWITHROM );
	m_tb.AddSeparator();
	m_tb.AddButton( 37, ID_TOOLS_PALETTEEDITOR, nullptr, 0, FALSE, IDS_TOOLS_PALETTEEDITOR );
	m_tb.AddButton( 3, ID_TOOLS_BACKGROUNDCOLORSEDITOR, nullptr, 0, FALSE, IDS_TOOLS_BACKGROUNDCOLORSEDITOR );
	m_tb.AddButton( 34, ID_TOOLS_DEMO, nullptr, 0, FALSE, IDS_TOOLS_DEMO );
	m_tb.AddButton( 23, ID_TOOLS_GAMESSETTINGS, nullptr, 0, FALSE, IDS_TOOLS_GAMESSETTINGS );
	m_tb.AddSeparator();
	m_tb.AddButton( 16, IDM_ABOUT, nullptr, 0, TRUE, IDS_ABOUT );

	m_tb.Visible( TRUE );
	m_rb.InsertBand( m_tb, 0, TRUE );
	m_status.SetParts( 300, 300, 350, 350, 0 );
	wSendMessage( WM_UPDATEUISTATE, MAKELONG( UIS_SET, UISF_HIDEACCEL | UISF_HIDEFOCUS ) );
	return TRUE;
}


VOID CMainWindow::UpdateAreaPanel( CCanvasTabDlg * pTab )
{
	CV_LEVEL_ID cvli;
	pTab->GetLevelId( cvli );

	UpdateHeader( pTab );

	BOOL fEnableCloseTab = ( m_tc.Count() > 1 );
	m_menu.EnableItems(fEnableCloseTab, ID_WINDOW_CLOSETAB);
}

VOID CMainWindow::OnDestroy()
{
	// delete Canvas()->GetView();
}

VOID CMainWindow::OnCommand( USHORT uCmd, USHORT uId, HWND hCtl )
{
	if ( 1 == uCmd )
	{
		OnButton( uId );
	}
}

LRESULT CMainWindow::OnTabNotify( CCanvasTabDlg * pTab, LPNMHDR lpnm )
{
	switch ( lpnm->code )
	{
		case NCVN_OBJECT_HILITE: return Ctl_OnHilite( pTab, (PNCVNVIEW)lpnm );
		case NCVN_OBJECT_SELECT: return Ctl_OnSelect( pTab, (PNCVNVIEW)lpnm );
		case NCVN_REDRAW_REQUEST: return Ctl_OnRedraw( pTab, (PNCVNVIEW)lpnm );
		case NCVN_DBL_CLICK: return Ctl_OnDblClick( pTab, (PNCVNVIEW)lpnm );
		case NCVN_MAP_MOVE: return Ctl_OnMapMove( pTab, (PNCNVIEWMAP)lpnm );
		case TCVNTF_FOLLOWLEVEL: return Ctl_OnFollowLevel( pTab, (PCVNTFNM_FOLLOW)lpnm );
		case TCVNTF_UPDATEBUFFERMENU: UpdateBufferMenu( pTab ); break;
		case TCVNTF_UPDATEPANEL: UpdateAreaPanel( pTab ); break;
		case TCVNTF_UPDATEUNDOMENU: UpdateUndoMenu( pTab ); break;
	}

	return 0;
}

INT_PTR CMainWindow::OnNotify( LPNMHDR lpnm )
{
	switch ( lpnm->idFrom )
	{
		case IDCU_MAINWINDOW_REBAR:
			{
				switch ( lpnm->code )
				{
					case RBN_HEIGHTCHANGE: OnRebarHeightChange(); break;
				}
				break;
			}
		case IDCU_MAINWINDOW_TAB:
			{
				return m_pctab->OnNotify( lpnm );
			}
		default:
			{
				if ( TTN_GETDISPINFOA == lpnm->code || TTN_GETDISPINFOW == lpnm->code )
				{
					Ctl_OnToolbarTip( lpnm->idFrom, (LPNMTTDISPINFO)lpnm, ( TTN_GETDISPINFOA == lpnm->code ) );
				}
				break;
			}
	}

	return 0;
}

VOID CMainWindow::OnSize( INT dwState, USHORT cx, USHORT cy )
{
	OnRebarHeightChange();
}

VOID CMainWindow::OnSizing( INT dwState, USHORT cx, USHORT cy )
{
	OnSize( dwState, cx, cy );
}

VOID CMainWindow::OnRebarHeightChange()
{
	RECT rcWnd = { 0 }, rcStatus = { 0 }, rcPan = { 0 }, rcRebar = { 0 };
	GetClientRect( *this, &rcWnd );
	GetWindowRect( m_status, &rcStatus );
	GetWindowRect( m_rb, &rcRebar );


	SetWindowPos( /* m_canvas */ m_tc, NULL, rcPan.right - rcPan.left, rcRebar.bottom - rcRebar.top,
		rcWnd.right - rcWnd.left - ( rcPan.right - rcPan.left ),
		rcWnd.bottom - ( rcStatus.bottom - rcStatus.top ) - ( rcRebar.bottom - rcRebar.top ), 0 );

	if ( m_pctab && m_pctab->IsOnTab() ) m_pctab->Tab()->SizeChanged();

	InvalidateRect( m_status, nullptr, TRUE );
}

VOID CMainWindow::LoadLevel( BYTE bWorld, BYTE bLevel, CNesLevel * pLevel )
{
	if ( m_pEditor )
	{
		CV_LEVEL_ID cvli = { bWorld, bLevel, pLevel };
		SelectLevelTab( cvli );
		SetLoadedLevelInfo( m_pctab->Tab() );
	}
}

VOID CMainWindow::LoadSubLevel( BYTE bWorld, CNesLevel * pLevel )
{
	if ( m_pEditor )
	{
		CV_LEVEL_ID cvli;
		cvli.bWorld = bWorld;
		cvli.bLevel = 0xFF;
		cvli.pLevel = pLevel;

		SelectLevelTab( cvli );
		SetLoadedLevelInfo( m_pctab->Tab() );
	}
}

VOID CMainWindow::SelectLevelTab( const CV_LEVEL_ID & cvli )
{
	if ( !m_pctab->SelectTab( cvli ) )
	{
		m_pctab->AddTab( cvli );
	}
	else if ( cvli.bLevel != 0xFF )
	{
		m_pctab->Tab()->SetLevelNumber( cvli.bLevel );
	}
}

VOID CMainWindow::SetLoadedLevelInfo( CCanvasTabDlg * pTab )
{
	if ( !pTab ) return;

	CV_LEVEL_ID cvli;
	pTab->GetLevelId( cvli );

	UpdateHeader( pTab );
	UpdateAreaPanel( pTab );
	UpdateUndoMenu( pTab );
}

VOID CMainWindow::LoadLevel( BYTE bWorld, BYTE bLevel )
{
	if ( m_pEditor )
	{
		NES_GAME_AREA_LIST nAreaList;
		m_pEditor->GetAreaList( nAreaList );

		if ( bWorld < nAreaList.size() && bLevel < nAreaList[ bWorld ].size() && nAreaList[ bWorld ][ bLevel ].size() > 0 )
		{
			LoadLevel( bWorld, bLevel, nAreaList[ bWorld ][ bLevel ][ 0 ] );
		}
	}
}

VOID CMainWindow::AreaLinkManager()
{
	m_pctab->Tab()->Cmd_AreaLinksManager();
}

VOID CMainWindow::UpdateBufferMenu( CCanvasTabDlg * pTab )
{
	if ( !pTab ) return;

	m_menu.EnableItems(pTab->IsObjectSelected(), ID_EDIT_COPY, ID_EDIT_CUT);
	m_menu.EnableItems(m_buffer.CanPaste(), ID_EDIT_PASTE);
}

VOID CMainWindow::UpdateUndoMenu( CCanvasTabDlg * pTab )
{
	if ( !pTab ) return;

	m_menu.EnableItems(pTab->IsCanUndo(), ID_EDIT_UNDO);
}

VOID CMainWindow::DumpArea( LPCTSTR pszFile )
{
	std::vector<BYTE> bData, bLevel, bEnemy;
	m_pctab->Tab()->Cmd_DumpArea( bLevel, bEnemy );
	bData.push_back( LOBYTE( bLevel.size() ) );
	bData.push_back( LOBYTE( bEnemy.size() ) );
	bData.insert( bData.end(), bLevel.begin(), bLevel.end() );
	bData.insert( bData.end(), bEnemy.begin(), bEnemy.end() );

	HANDLE hFile = CreateFile( pszFile, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD w;
		WriteFile( hFile, bData.data(), LODWORD( bData.size() ), &w, nullptr );
		CloseHandle( hFile );
	}
	else
	{
		ShowAlertF( TEXT( "Unable to create file '%s'" ), pszFile );
	}
}

VOID CMainWindow::LoadArea( LPCTSTR pszFile )
{
	std::vector<BYTE> bData, bLevel, bEnemy;
	HANDLE hFile = CreateFile( pszFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER li = { 0 };
		if ( GetFileSizeEx( hFile, &li ) )
		{
			if ( li.QuadPart > 2 && li.QuadPart < 0x210 )
			{
				DWORD w;
				bData.insert( bData.end(), li.QuadPart, 0 );
				ReadFile( hFile, bData.data(), LODWORD( bData.size() ), &w, nullptr );

				if ( bData.size() >= bData[ 0 ] + bData[ 1 ] )
				{
					auto it = bData.begin() + 2;
					bLevel.insert( bLevel.end(), it, it + bData[ 0 ] );
					it += bData[ 0 ];
					bEnemy.insert( bEnemy.end(), it, it + bData[ 1 ] );

					if ( bLevel.size() > 0 && bEnemy.size() > 0 )
					{
						if ( bLevel.back() == 0xFD && bEnemy.back() == 0xFF )
						{
							bLevel.pop_back();
							bEnemy.pop_back();
							m_pctab->Tab()->Cmd_LoadArea( bLevel, bEnemy );
						}
					}
				}
			}
		}
		CloseHandle( hFile );
	}
	else
	{
		ShowAlertF( TEXT( "Unable to open file '%s'" ), pszFile );
	}
}

// --- buffer


BOOL CMainWindow::CheckUnreferencedAreas()
{
	BOOL fResult = TRUE;

	if ( m_pEditor )
	{
		std::vector<CNesLevel*> vpUnusedLevel;
		CUnusedAreasDlg dlg( GetInstance() );
		if ( m_pEditor->UnreferencedAreaList( vpUnusedLevel ) )
		{
			CNesLevelsKeeper * pKeeper = m_pEditor->GetAreaList();

			dlg.Set_Param( pKeeper, vpUnusedLevel );
			if ( dlg.Show( this ) )
			{
				dlg.Get_Param( vpUnusedLevel );

				for ( auto pLevel : vpUnusedLevel )
				{
					m_pctab->CloseTabWithLevel( pLevel );
					m_pEditor->DeleteArea( pLevel );
				}

				CV_LEVEL_ID cvli = { 0, 0, nullptr };
				auto pTab = m_pctab->Tab();
				if ( pTab )
				{
					m_pctab->Tab()->GetLevelId( cvli );
				}

				LoadLevel( cvli.bWorld, cvli.bLevel );
			}
			else
			{
				fResult = FALSE;
			}
			delete pKeeper;
		}
		else
		{
			ShowAlert( TEXT( "There is no unreferenced areas was found" ) );
		}
	}

	return fResult;
}

VOID CMainWindow::UpdateHeader( CCanvasTabDlg * pTab )
{
	CString strHeader;

	CV_LEVEL_ID cvli;
	pTab->GetLevelId( cvli );

	if ( 0xFF != cvli.bLevel )
	{
		strHeader.Format( TEXT( "%s - [%c] %s, World %d-%d" ),
						  Str( IDS_APP_TITLE ).c_str(),
						  m_pEditor->GetVersion(),
						  PathFindFileName( m_pEditor->Filename() ),
						  cvli.bWorld + 1,
						  cvli.bLevel + 1 );
	}
	else
	{
		strHeader.Format( TEXT( "%s - [%c] %s, World %d" ),
						  Str( IDS_APP_TITLE ).c_str(),
						  m_pEditor->GetVersion(),
						  PathFindFileName( m_pEditor->Filename() ),
						  cvli.bWorld + 1 );
	}

	wSendMessage( WM_SETTEXT, 0, (LPARAM)strHeader.GetString() );

	m_status.SetText( 0, PathFindFileName( m_pEditor->Filename() ) );
}
