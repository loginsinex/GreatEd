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
#include <shellapi.h>

VOID CMainWindow::OnButton( USHORT uId )
{
	BOOL fUpdateFocus = TRUE;
	switch ( uId )
	{
		case ID_FILE_OPEN: Ctl_OnOpen(); break;
		case ID_FILE_SAVE: Ctl_OnSave( FALSE ); break;
		case ID_FILE_SAVEAS: Ctl_OnSave( TRUE ); break;
		case ID_VIEW_SELECTLEVEL: Ctl_OnSelectLevel(); break;
		case ID_VIEW_SHOWMARIO: Ctl_SetPlayerPalOption( uId, npcMario ); break;
		case ID_VIEW_SHOWLUIGI: Ctl_SetPlayerPalOption( uId, npcLuigi ); break;
		case ID_VIEW_SHOWFIRE: Ctl_SetPlayerPalOption( uId, npcFire ); break;
		case ID_EDIT_DELETEOBJECT: Ctl_OnDeleteObject(); break;
		case ID_EDIT_INSERTOBJECT: Ctl_OnInsertObject(); break;
		case ID_EDIT_LOADAREAFROMFILE: Ctl_OnLoadAreaFromFile(); break;
		case ID_EDIT_DUMPAREAINTOAFILE: Ctl_OnDumpAreaIntoFile(); break;
		case ID_EDIT_CREATENEWAREA: Ctl_OnCreateNewArea(); break;
		case ID_EDIT_EDITWORLDCONFIGURATION: Ctl_OnWorldConfig(); break;
		case ID_EDIT_SELECTALL: Ctl_OnSelectAll(); break;
		case ID_EDIT_COPY: Ctl_OnBufferEvent( beCopy ); break;
		case ID_EDIT_CUT: Ctl_OnBufferEvent( beCut ); break;
		case ID_EDIT_PASTE: Ctl_OnBufferEvent( bePaste ); break;
		case ID_AREA_SETTINGS: Ctl_OnAreaSettings(); break;
		case ID_AREA_TYPE: Ctl_OnChangeAreaType(); break;
		case ID_TOOLS_PALETTEEDITOR: Ctl_OnPaletteEditor(); break;
		case ID_TOOLS_BACKGROUNDCOLORSEDITOR: Ctl_OnBgColorEditor(); break;
		case ID_EDIT_UNDO: Ctl_OnUndo(); break;
		case ID_TOOLS_AREAPOINTERSMANAGER: Ctl_OnAreaLinkManager(); break;
		case ID_ADVANCED_UNUSEDAREAS: Ctl_OnUnusedAreas(); break;
		case ID_TOOLS_DEMO: Ctl_OnDemoPlay(); fUpdateFocus = FALSE; break;
		case ID_TOOLS_GAMESSETTINGS: Ctl_OnGameSettings(); break;
		case ID_COMMAND_RUNEMULATORWITHROM: Ctl_OnExec(); break;
		case ID_COMMAND_RUNEMULATORFROMTHISPOINT: Ctl_OnExecFromPoint(); break;
		case ID_WINDOW_CLOSETAB: Ctl_OnCloseTab(); break;
		case ID_SELECTNEXTTAB: Ctl_OnSelectTab( FALSE ); break;
		case ID_SELECTPREVTAB: Ctl_OnSelectTab( TRUE ); break;
		case ID_VIEW_SHOWGAMETITLEIN1: Ctl_SetViewTitle( m_menu.IsItemChecked( uId ) ); break;
		case IDM_ABOUT: Ctl_OnAbout(); break;
		case IDM_EXIT: Ctl_OnExit(); break;
	}

	if ( fUpdateFocus && m_pctab )
	{
		CCanvasTabDlg * pTab = m_pctab->Tab();
		if ( pTab ) SetFocus( pTab->CanvasWindow() );
	}
}

VOID CMainWindow::Ctl_OnAbout()
{
	CAboutDlg dlgAbout( GetInstance() );
	dlgAbout.Show( this );
}

VOID CMainWindow::OnCancel()
{
	Ctl_OnExit();
}

BOOL CMainWindow::OnClose()
{
	Ctl_OnExit();
	return 0;
}

LRESULT CMainWindow::OnMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandle )
{
	switch ( uMsg )
	{
		case WM_TABNOTIFY:
			{
				fHandle = TRUE;
				return OnTabNotify( (CCanvasTabDlg*)wParam, (LPNMHDR)lParam );
			}
	}

	return 0;
}

VOID CMainWindow::Ctl_OnExit()
{
	if ( !m_pEditor )
	{
		Close( 0 );
	}
	else if ( ConfirmF( TEXT( "Do you want to close '%s' now?" ), FALSE, PathFindFileName( m_pEditor->Filename() ) ) )
	{
		Close( 0 );
	}
}

VOID CMainWindow::Ctl_OnOpen()
{
	if ( m_pEditor )
	{
		if ( !Confirm( TEXT( "File already opened. Do you want load another file?" ) ) ) return;
	}

	CString sFile = OpenFile( TEXT( "Super_Mario_Bros._(E).nes" ), TEXT( "NES files (*.nes)" ), TEXT( "*.nes" ), 0 ).c_str();
	if ( !sFile.GetLength() )
	{
		return;
	}
	
	CNesEditor * pEditor = new CNesEditor();

	try
	{
		pEditor->LoadFile( sFile );

		if ( m_pEditor )
		{
			m_pctab->CloseAllTabs();
			delete m_pctab;
			delete m_pEditor;			
		}

		m_pEditor = pEditor;
		m_pctab = new CCanvasTabCtl( GetInstance(), *this, *m_pEditor, m_buffer, m_tc );

		m_menu.EnableItems( TRUE, ID_FILE_SAVE, ID_FILE_SAVEAS, ID_VIEW_SELECTLEVEL, ID_EDIT_INSERTOBJECT, ID_EDIT_DELETEOBJECT, 
									ID_EDIT_LOADAREAFROMFILE, ID_EDIT_DUMPAREAINTOAFILE, 
									ID_EDIT_CREATENEWAREA, ID_EDIT_EDITWORLDCONFIGURATION, 
									ID_EDIT_SELECTALL, ID_TOOLS_PALETTEEDITOR, ID_TOOLS_BACKGROUNDCOLORSEDITOR,
									ID_TOOLS_AREAPOINTERSMANAGER, ID_ADVANCED_UNUSEDAREAS, ID_TOOLS_DEMO, ID_TOOLS_GAMESSETTINGS,
									ID_COMMAND_RUNEMULATORWITHROM, ID_VIEW_SHOWMARIO, ID_VIEW_SHOWLUIGI, ID_VIEW_SHOWFIRE,
									ID_VIEW_SHOWGAMETITLEIN1, ID_COMMAND_RUNEMULATORFROMTHISPOINT
									);

		m_tb.EnableAll( TRUE, ID_FILE_SAVE, ID_FILE_SAVEAS, ID_VIEW_SELECTLEVEL, ID_EDIT_INSERTOBJECT, ID_EDIT_DELETEOBJECT,
						ID_EDIT_LOADAREAFROMFILE, ID_EDIT_DUMPAREAINTOAFILE,
						ID_EDIT_CREATENEWAREA, ID_EDIT_EDITWORLDCONFIGURATION,
						ID_EDIT_SELECTALL, ID_TOOLS_PALETTEEDITOR, ID_TOOLS_BACKGROUNDCOLORSEDITOR,
						ID_TOOLS_AREAPOINTERSMANAGER, ID_ADVANCED_UNUSEDAREAS, ID_TOOLS_DEMO, ID_TOOLS_GAMESSETTINGS,
						ID_COMMAND_RUNEMULATORWITHROM, ID_COMMAND_RUNEMULATORFROMTHISPOINT,
						0 );

		m_menu.RadioCheckItems(ID_VIEW_SHOWMARIO, ID_VIEW_SHOWMARIO, ID_VIEW_SHOWLUIGI, ID_VIEW_SHOWFIRE);

		LoadLevel( 0, static_cast<BYTE>( 0 ) );
		UpdateBufferMenu( m_pctab->Tab() );
	}
	catch ( std::exception & e )
	{
		CString sMessage = e.what();
		ShowErrorF( TEXT( "Loader error. Is the file corrupted or not valid NES file?\n\n%s" ), sMessage.GetString() );
		delete pEditor;
	}
}

VOID CMainWindow::Ctl_OnSave( BOOL fSaveAs )
{
	if ( !m_pEditor ) return;

	CString sFile;

	if ( fSaveAs )
	{
		sFile = SaveFile( m_pEditor->Filename(), TEXT( "NES files (*.nes)" ), TEXT( "*.nes" ), 0 ).c_str();
		if ( !sFile.GetLength() )
		{
			return;
		}
	}

	try
	{
		m_pEditor->SaveFile( sFile.GetLength() ? sFile.GetString() : nullptr );
		UpdateHeader( m_pctab->Tab() );
	}
	catch ( std::exception & e )
	{
		CString sMessage = e.what();
		ShowErrorF( TEXT( "Unable to save the file.\n\n%s" ), sMessage.GetString() );
	}
}

VOID CMainWindow::Ctl_OnSelectLevel()
{
	if ( m_pEditor )
	{
		NES_GAME_AREA_LIST nAreaList;
		CSelectLevelDlg dlg( GetInstance() );

		m_pEditor->GetAreaList( nAreaList );
		CV_LEVEL_ID clvi; 
		m_pctab->Tab()->GetLevelId( clvi );

		dlg.SetParam( nAreaList, clvi.bWorld, clvi.bLevel, clvi.pLevel );

		if ( dlg.Show( this ) )
		{
			dlg.GetParam( &clvi.pLevel, clvi.bWorld, clvi.bLevel );
			LoadLevel( clvi.bWorld, clvi.bLevel, clvi.pLevel );			
		}
	}
}

INT_PTR CMainWindow::Ctl_OnHilite( CCanvasTabDlg * pTab, PNCVNVIEW pnc )
{
	CString str;

	if ( pnc->pObject )
	{
		RECT rcObject = pnc->pObject->GetObjectRect();

		if ( pnc->pObject->IsSprite() )
		{
			str.Format( TEXT( "Object: %s, [%d:%d]-[%d:%d]" ),
				m_nNames.GetNameOfEnemy( static_cast<CNesEnemyObject*>( pnc->pObject ) ).GetString(),
				rcObject.left, rcObject.top, rcObject.right - rcObject.left, rcObject.bottom - rcObject.top );
		}
		else
		{
			str.Format( TEXT( "Item: %s, [%d:%d]-[%d:%d]" ),
				m_nNames.GetNameOfItem( static_cast<CNesLevelItem*> ( pnc->pObject ) ).GetString(),
				rcObject.left, rcObject.top, rcObject.right - rcObject.left, rcObject.bottom - rcObject.top );
		}
	}
	else
	{
		str = TEXT( "No item at this point" );
	}

	m_status.SetText( 1, str );

	return 0;
}

INT_PTR CMainWindow::Ctl_OnSelect( CCanvasTabDlg * pTab, PNCVNVIEW pnc )
{
	CString str;
	if ( pnc->pObject && !pnc->pObject->IsSprite() )
	{
		CNesLevelItem * pObj = static_cast<CNesLevelItem*>( pnc->pObject );
		if ( pObj )
		{
			BYTE uPage = 0;
			CV_LEVEL_ID cvli;
			pTab->GetLevelId( cvli );
			CNesLevel * pTarget = pObj->GetTargetLevel( cvli.bWorld, uPage );
			if ( pTarget )
			{
				str.Format( TEXT( "Points to area 0x%02X (Type: %d, ID: 0x%02X), Page: %d" ),
							pTarget->AreaPtr().bPtr, pTarget->AreaPtr().bType, pTarget->AreaPtr().bAreaId, uPage );
			}
			else if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
			{
				CNesLoopCommand * pLoop = static_cast<CNesLoopCommand*>( pObj );
				str.Format( TEXT( "Trigger page: %02d, Allowed height: %d. Trigger is %s" ),
							pLoop->TriggerPage(), pLoop->AllowedHeight(), ( pLoop->LoopConnected() ? TEXT( "slave" ) : TEXT( "standalone" ) ) );
			}
		}
	}

	m_status.SetText( 3, str );
	UpdateBufferMenu( pTab );

	return 0;
}

INT_PTR CMainWindow::Ctl_OnRedraw( CCanvasTabDlg * pTab, PNCVNVIEW pnc )
{
	return 0;
}

INT_PTR CMainWindow::Ctl_OnMapMove( CCanvasTabDlg * pTab, PNCNVIEWMAP pncm )
{
	CString str;
	if ( pncm->tile.bMetatile )
	{
		str.Format(
			TEXT( "Tile: 0x%02X, Attribute: %d, ID: 0x%02X [%d:%d]" ),
			pncm->tile.bMetatile,
			pncm->tile.attr,
			pncm->tile.tileId,
			pncm->pt.x, pncm->pt.y
		);
	}

	m_status.SetText( 2, str );

	return 0;
}

INT_PTR CMainWindow::Ctl_OnDblClick( CCanvasTabDlg * pTab, PNCVNVIEW pncm )
{
	if ( pncm->pObject )
	{
		if ( !pncm->pObject->IsSprite() )
		{
			CV_LEVEL_ID cvli;
			pTab->GetLevelId( cvli );
			CNesLevelItem * pObj = static_cast<CNesLevelItem*>( pncm->pObject );
			if ( pObj->IsEnterableObject() )
			{
				BYTE uPage = 0;
				CNesLevel * pLevel = pObj->GetTargetLevel( cvli.bWorld, uPage );
				if ( pLevel )
				{
					LoadSubLevel( cvli.bWorld, pLevel );
					m_pctab->Tab()->ScrollToPage( uPage );
				}
			}
		}
		else
		{
			CNesEnemyObject * pObj = static_cast<CNesEnemyObject*>( pncm->pObject );
			pObj->SetHardObject( !pObj->IsHardObject() );
			m_pctab->Tab()->Update();
		}
	}

	UpdateBufferMenu( pTab );

	return 0;
}

INT_PTR CMainWindow::Ctl_OnFollowLevel( CCanvasTabDlg * pTab, PCVNTFNM_FOLLOW pcfl )
{
	LoadSubLevel( pcfl->bWorld, pcfl->pLevel );
	m_pctab->Tab()->ScrollToPage( pcfl->uPage );
	return 0;
}


VOID CMainWindow::Ctl_OnDeleteObject()
{
	if ( m_pEditor )
	{
		m_pctab->Tab()->Cmd_DeleteObject();
	}
}

VOID CMainWindow::Ctl_OnInsertObject()
{
	if ( m_pEditor )
	{
		m_pctab->Tab()->Cmd_InsertObject();
	}
}

VOID CMainWindow::Ctl_OnLoadAreaFromFile()
{
	if ( m_pEditor )
	{
		CString strFile;
		CV_LEVEL_ID cvli;
		m_pctab->Tab()->GetLevelId( cvli );
		strFile.Format( TEXT( "area_%02x_world_%d.bin" ), cvli.pLevel->AreaPtr().bPtr, cvli.bWorld );
		CString str = OpenFile( strFile, TEXT( "BIN files(*.bin)" ), TEXT( "*.bin" ), TEXT( "All files(*.*)" ), TEXT( "*.*" ), 0 ).c_str();

		if ( str.GetLength() )
		{
			LoadArea( str );
		}
	}
}

VOID CMainWindow::Ctl_OnDumpAreaIntoFile()
{
	if ( m_pEditor )
	{
		CString strFile;
		CV_LEVEL_ID cvli;
		m_pctab->Tab()->GetLevelId( cvli );
		strFile.Format( TEXT( "area_%02x_world_%d.bin" ), cvli.pLevel->AreaPtr().bPtr, cvli.bWorld );
		CString str = SaveFile( strFile, TEXT( "BIN files(*.bin)" ), TEXT( "*.bin" ), TEXT( "All files(*.*)" ), TEXT( "*.*" ), 0 ).c_str();

		if ( str.GetLength() )
		{
			DumpArea( str );
		}
	}
}

VOID CMainWindow::Ctl_OnCreateNewArea()
{
	if ( m_pEditor )
	{
		CV_LEVEL_ID clvi;
		CNesLevelsKeeper * pKeeper = m_pEditor->GetAreaList();
		CCreateNewAreaDlg dlg( GetInstance() );
		CNesLevel * pLevel = nullptr;
		BOOL fAutoLevel = FALSE;
		NES_LEVEL_TYPE type = nltWater;
		m_pctab->Tab()->GetLevelId( clvi );

		dlg.Set_Param( clvi.bWorld, pKeeper, clvi.pLevel );

		LRESULT code = dlg.Show( this );
		dlg.Get_Param( type, fAutoLevel, &pLevel );

		switch ( code )
		{
			case 1: pLevel = m_pEditor->CreateLevel( type, fAutoLevel ); break;
			case 2: pLevel = m_pEditor->CreateLevel( pLevel ); break;
		}

		if ( code )
		{
			if ( !pLevel )
			{
				ShowError( TEXT( "Too many areas for this type" ) );
			}
			else
			{
				LoadSubLevel( clvi.bWorld, pLevel );
			}
		}
		
		delete pKeeper;
	}
}

VOID CMainWindow::Ctl_OnWorldConfig()
{
	if ( m_pEditor )
	{
		CV_LEVEL_ID clvi;
		CNesLevelsKeeper * pKeeper = m_pEditor->GetAreaList();
		NES_GAME_AREA_LIST nag;
		NES_GAME_AREA_HP_LIST nhp;
		if ( m_pEditor->GetAreaList( nag, nhp ) )
		{
			CWorldEditorDlg dlg( GetInstance() );
			m_pctab->Tab()->GetLevelId( clvi );
			dlg.Set_Param( nag, nhp, pKeeper, clvi.bWorld, clvi.bLevel, clvi.pLevel );

			if ( dlg.Show( this ) )
			{
				CNesLevel * pLevel;
				BYTE bWorld = 0, bLevel = 0;
				dlg.Get_Param( nag, nhp, bWorld, bLevel, &pLevel );
				m_pEditor->SetAreaList( nag, nhp );
				
				if ( dlg.IsModified() ) m_pctab->CloseAllTabs();

				if ( !pLevel )
				{
					LoadLevel( bWorld, bLevel );
				}
				else
				{
					LoadLevel( bWorld, bLevel, pLevel );
				}
			}		
		}

		delete pKeeper;
	}
}

VOID CMainWindow::Ctl_OnBufferEvent( BUFFER_EVENT be )
{
	m_pctab->Tab()->Cmd_BufferEvent( be );
}

VOID CMainWindow::Ctl_OnSelectAll()
{
	if ( m_pEditor )
	{
		m_pctab->Tab()->Cmd_SelectAll();
	}
}

VOID CMainWindow::Ctl_OnAreaSettings()
{
	if ( m_pEditor )
	{
		m_pctab->Tab()->Cmd_AreaSettings();
	}
}

VOID CMainWindow::Ctl_OnChangeAreaType()
{
	if ( m_pEditor )
	{
		m_pctab->Tab()->Cmd_ChangeLevelType();
	}
}

VOID CMainWindow::Ctl_OnPaletteEditor()
{
	if ( m_pEditor )
	{
		struct pl
		{
			NES_LEVEL_TYPE nlt;
			NES_AREA_STYLE nas;
			NES_BG_COLOR_CTL bgModifier;
		};

		std::map<NES_PAL_TABLE, NES_PAL_ARRAY> mPal;
		std::map<NES_PAL_TABLE, pl> mPalId;
		NES_PLAYER_PAL npc;

		NES_PAL_TABLE table[] = { nptGround, nptNightSnowGround, nptDaySnowGround, nptMushroomGround, nptWater, nptUnderground, nptCastle };

		CPaletteDlg dlg(GetInstance());

		for (const auto & t : table)
		{
			pl plData;
			switch (t)
			{
			case nptGround: plData.nlt = nltGround; plData.nas = areaStyle0; plData.bgModifier = bgCtl0; break;
			case nptWater: plData.nlt = nltWater; plData.nas = areaStyle0; plData.bgModifier = bgCtl0; break;
			case nptUnderground: plData.nlt = nltUnderground; plData.nas = areaStyle0; plData.bgModifier = bgCtl0; break;
			case nptCastle: plData.nlt = nltCastle; plData.nas = areaStyle0; plData.bgModifier = bgCtl0; break;
			case nptMushroomGround: plData.nlt = nltGround; plData.nas = areaStyle1; plData.bgModifier = bgCtl0; break;
			case nptDaySnowGround: plData.nlt = nltGround; plData.nas = areaStyle0; plData.bgModifier = bgCtl2; break;
			case nptNightSnowGround: plData.nlt = nltGround; plData.nas = areaStyle0; plData.bgModifier = bgCtl3; break;
			}
			mPalId[t] = plData;

			m_pEditor->GetPalette( plData.nlt, plData.nas, plData.bgModifier, mPal[ t ] );
		}

		m_pEditor->GetPlayerPalette( npc );

		dlg.Set_Param( mPal, npc );

		if (dlg.Show(this))
		{
			dlg.Get_Param( mPal, npc );

			for (const auto & pal : mPal)
			{
				const auto & plData = mPalId[pal.first];
				m_pEditor->SetPalette(plData.nlt, plData.nas, plData.bgModifier, pal.second);
			}

			m_pEditor->SetPlayerPalette( npc );
			m_pctab->Tab()->Update();
		}
	}
}

VOID CMainWindow::Ctl_OnBgColorEditor()
{
	if ( m_pEditor )
	{
		NES_BG_COLORS nbg;
		CPaletteBgDlg dlg( GetInstance() );

		m_pEditor->GetBgColors( nbg );
		dlg.Set_Param( nbg );

		if ( dlg.Show( this ) )
		{
			dlg.Get_Param( nbg );
			m_pEditor->SetBgColors( nbg );
			m_pctab->Tab()->Update();
		}
	}
}

VOID CMainWindow::Ctl_OnUndo()
{
	CCanvasTabDlg * pTab = m_pctab->Tab();
	if ( pTab )
	{
		if ( pTab->IsCanUndo() ) pTab->DoUndo();
	}
}

VOID CMainWindow::Ctl_OnAreaLinkManager()
{
	if ( m_pEditor )
	{
		AreaLinkManager();
	}
}

VOID CMainWindow::Ctl_OnUnusedAreas()
{
	if ( m_pEditor )
	{
		CheckUnreferencedAreas();
	}
}

VOID CMainWindow::Ctl_OnDemoPlay()
{
	if ( m_pEditor )
	{
		std::vector<std::pair<BYTE, BYTE>> vDemoPlay;

		if ( !static_cast<HWND>( m_dlgDemo ) )
		{
			m_pEditor->GetDemoData( vDemoPlay );
			m_dlgDemo.Set_Param( vDemoPlay );
			m_dlgDemo.Create( this );
			ShowWindow( m_dlgDemo, SW_SHOW );
			SetForegroundWindow( m_dlgDemo );
		}

		m_dlgDemo.Get_Param( vDemoPlay );
		m_pEditor->SetDemoData( vDemoPlay );
	}
}

VOID CMainWindow::Ctl_OnGameSettings()
{
	if ( m_pEditor )
	{
		NES_ENGINE_HACK hack;
		CEngineHackSheet dlg( GetInstance() );

		m_pEditor->GetHack( hack );
		dlg.Set_Param( hack );

		if ( dlg.Show( this ) )
		{
			dlg.Get_Param( hack );
			m_pEditor->SetHack( hack );
			m_pctab->Tab()->Update();
			UpdateAreaPanel( m_pctab->Tab() );
		}
	}
}

VOID CMainWindow::Ctl_OnToolbarTip( UINT_PTR cmd, LPNMTTDISPINFO lpnmtt, BOOL fAnsi )
{
	TBBUTTONINFO tbi = { 0 };
	tbi.cbSize = sizeof( tbi );
	tbi.dwMask = TBIF_LPARAM;
	m_tb.cSendMessage( TB_GETBUTTONINFO, cmd, (LPARAM)&tbi );

	if ( tbi.lParam )
		lstrcpy( lpnmtt->lpszText, Str( LODWORD( tbi.lParam ) ).c_str() );
}

VOID CMainWindow::Ctl_OnExec()
{
	if ( m_pEditor )
	{
		ShellExecute( *this, TEXT( "open" ), m_pEditor->Filename(), NULL, NULL, SW_SHOW );
	}
}

VOID CMainWindow::Ctl_OnExecFromPoint()
{
	if ( m_pEditor )
	{
		CString strPath, strFilename;
		if ( GetEnvironmentVariable( TEXT( "TEMP" ), strPath.GetBuffer( MAX_PATH + 1 ), MAX_PATH ) > 0 )
		{
			strPath.ReleaseBuffer();
			CV_LEVEL_ID clvi;
			m_pctab->Tab()->GetLevelId( clvi );
			strFilename.Format( TEXT( "%s\\__test_world%d-%d_area%d_page_%d.nes" ),
								strPath.GetString(), clvi.bWorld + 1, clvi.bLevel + 1, clvi.pLevel->AreaPtr().bPtr, m_pctab->Tab()->GetCurrentPage() );

			if ( m_pEditor->MakeTestROM( strFilename, MAKEWORD( clvi.bLevel, clvi.bWorld ), *clvi.pLevel, m_pctab->Tab()->GetCurrentPage(), 5 ) )
			{
				ShellExecute( *this, TEXT( "open" ), strFilename, NULL, NULL, SW_SHOW );
			}
		}
		else
		{
			strPath.ReleaseBuffer();
		}

	}
}

VOID CMainWindow::Ctl_OnCloseTab()
{
	if ( m_tc.Count() > 1 && Confirm( TEXT( "Are you sure that you want close this tab?" ) ) )
	{
		m_pctab->CloseTab( m_tc.CurSel() );
	}
}


VOID CMainWindow::Ctl_OnSelectTab( BOOL fReverse )
{
	m_pctab->SelectNextTab( fReverse );
}

VOID CMainWindow::Ctl_SetPlayerPalOption( UINT uCheckCmd, NES_PLAYER_COL_T npcOption )
{
	if ( m_pEditor )
	{
		m_menu.RadioCheckItems(uCheckCmd, ID_VIEW_SHOWMARIO, ID_VIEW_SHOWLUIGI, ID_VIEW_SHOWFIRE);
		m_pEditor->SetPlayerPalOption( npcOption );
		m_pctab->Tab()->Update();
	}
}

VOID CMainWindow::Ctl_SetViewTitle( BOOL bTitleChecked )
{
	m_menu.CheckItems( !bTitleChecked, ID_VIEW_SHOWGAMETITLEIN1 );
	m_pctab->SetLevelTitleVisible( !bTitleChecked );
	m_pctab->SelectTab( m_pctab->CurSel() );
}