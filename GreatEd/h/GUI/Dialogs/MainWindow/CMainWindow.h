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

#define IDCU_MAINWINDOW_STATUS		0x2010
#define IDCU_MAINWINDOW_MENU		0x2020
#define IDCU_MAINWINDOW_REBAR		0x2030
#define IDCU_MAINWINDOW_CANVAS		0x1000
#define IDCU_MAINWINDOW_PANEL		0x1001
#define IDCU_MAINWINDOW_TOOLBAR		0x1002
#define IDCU_MAINWINDOW_TAB			0x1003



class CMainWindow : public CWindow
{
	CPaletteBitmap	m_bmLevelView;
	CMenu			m_menu;
	CToolbar		m_tb;
	CStatusBar		m_status;
	CRebarWindow	m_rb;
	CObjBuffer		m_buffer;
	CTabControl		m_tc;
	CDemoPlayEditorDlg	m_dlgDemo;

	CNesEditor			* m_pEditor;
	CCanvasTabCtl		* m_pctab;
	CNesObjectsNames	m_nNames;

	BOOL			OnCreate( LPCREATESTRUCT lpcs ) override;
	VOID			OnDestroy() override;
	VOID			OnCommand( USHORT uCmd, USHORT uId, HWND hCtl ) override;
	VOID			OnButton( USHORT uId ) override;
	VOID			OnCancel() override;
	VOID			OnSize( INT dwState, USHORT cx, USHORT cy ) override;
	VOID			OnSizing( INT dwState, USHORT cx, USHORT cy ) override;
	BOOL			OnClose() override;
	LRESULT			OnMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandle );

	VOID			OnRebarHeightChange();

	//				Controls
	VOID			Ctl_OnAbout();
	VOID			Ctl_OnExit();
	VOID			Ctl_OnOpen();
	VOID			Ctl_OnSelectLevel();
	VOID			Ctl_OnSave( BOOL fSaveAs );
	VOID			Ctl_OnDeleteObject();
	VOID			Ctl_OnInsertObject();
	VOID			Ctl_OnLoadAreaFromFile();
	VOID			Ctl_OnDumpAreaIntoFile();
	VOID			Ctl_OnCreateNewArea();
	VOID			Ctl_OnWorldConfig();
	VOID			Ctl_OnBufferEvent( BUFFER_EVENT be );
	VOID			Ctl_OnSelectAll();
	VOID			Ctl_OnAreaSettings();
	VOID			Ctl_OnChangeAreaType();
	VOID			Ctl_OnPaletteEditor();
	VOID			Ctl_OnBgColorEditor();
	VOID			Ctl_OnUndo();
	VOID			Ctl_OnAreaLinkManager();
	VOID			Ctl_OnDemoPlay();
	VOID			Ctl_OnUnusedAreas();
	VOID			Ctl_OnGameSettings();
	VOID			Ctl_OnToolbarTip( UINT_PTR cmd, LPNMTTDISPINFO lpnmtt, BOOL fAnsi );
	VOID			Ctl_OnCloseTab();
	VOID			Ctl_OnSelectTab( BOOL fReverse );
	VOID			Ctl_SetPlayerPalOption( UINT uCheckCmd, NES_PLAYER_COL_T npcOption );
	VOID			Ctl_SetViewTitle( BOOL bTitleChecked );
	VOID			Ctl_OnExec();
	VOID			Ctl_OnExecFromPoint();

	INT_PTR			OnNotify( LPNMHDR lpnm ) override;

	LRESULT			OnTabNotify( CCanvasTabDlg * pTab, LPNMHDR lpnm );
	INT_PTR			Ctl_OnHilite( CCanvasTabDlg * pTab, PNCVNVIEW pnc );
	INT_PTR			Ctl_OnSelect( CCanvasTabDlg * pTab, PNCVNVIEW pnc );
	INT_PTR			Ctl_OnRedraw( CCanvasTabDlg * pTab, PNCVNVIEW pnc );
	INT_PTR			Ctl_OnMapMove( CCanvasTabDlg * pTab, PNCNVIEWMAP pncm );
	INT_PTR			Ctl_OnDblClick( CCanvasTabDlg * pTab, PNCVNVIEW pncm );
	INT_PTR			Ctl_OnFollowLevel( CCanvasTabDlg * pTab, PCVNTFNM_FOLLOW pcfl );

	VOID			LoadLevel( BYTE bWorld, BYTE bLevel );
	VOID			LoadLevel( BYTE bWorld, BYTE bLevel, CNesLevel * pLevel );
	VOID			LoadSubLevel( BYTE bWorld, CNesLevel * pLevel );
	VOID			SetLoadedLevelInfo( CCanvasTabDlg * pTab );
	VOID			SelectLevelTab( const CV_LEVEL_ID & cvli );

	VOID			DumpArea( LPCTSTR pszFile );
	VOID			LoadArea( LPCTSTR pszFile );
	VOID			AreaLinkManager();
	BOOL			CheckUnreferencedAreas();
	VOID			UpdateBufferMenu( CCanvasTabDlg * pTab );
	VOID			UpdateUndoMenu( CCanvasTabDlg * pTab );
	VOID			UpdateHeader( CCanvasTabDlg * pTab );
	VOID			UpdateAreaPanel( CCanvasTabDlg * pTab );

public:
	CMainWindow( HINSTANCE hInstance );
};