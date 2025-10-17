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

typedef struct _tagBUFFER_OBJECT
{
	NES_LEVEL_ITEM			nItem;
	NES_LEVEL_ENEMY_ITEM	nObject;
	BOOL					fObject;
} BUFFER_OBJECT, *PBUFFER_OBJECT;

typedef enum _tagBUFFER_EVENT
{
	beCopy,
	beCut,
	bePaste
} BUFFER_EVENT, *PBUFFER_EVENT;

#define		WM_TABNOTIFY		( WM_USER + 0x544 )

#define		TCVNTF_FIRST				( -1000 )
#define		TCVNTF_UPDATEBUFFERMENU		( TCVNTF_FIRST - 1 )
#define		TCVNTF_UPDATEUNDOMENU		( TCVNTF_FIRST - 2 )
#define		TCVNTF_UPDATEPANEL			( TCVNTF_FIRST - 3 )
#define		TCVNTF_FOLLOWLEVEL			( TCVNTF_FIRST - 4 )

typedef struct _tagCVNTFNM_FOLLOW
{
	NMHDR			nm;
	CNesLevel *		pLevel;
	BYTE			bWorld;
	UINT			uPage;
} CVNTFNM_FOLLOW, *PCVNTFNM_FOLLOW;

class CCanvasTabCtl;

class CObjBuffer
{
	std::vector<BUFFER_OBJECT>		m_vObjects;

public:
	CObjBuffer();

	VOID			Copy( std::vector<CNesObject*> & vObject );
	BOOL			Paste( CNesLevel * pLevel, const POINT & pt, std::vector<CNesObject*> & vpCreatedObjects );
	BOOL			CanPaste();
};

typedef struct _tagCV_LEVEL_ID
{
	BYTE			bWorld;
	BYTE			bLevel;
	CNesLevel		*pLevel;
} CV_LEVEL_ID, *PCV_LEVEL_ID;


class CCanvasTabDlg : public CDialog
{
	CWindow				&	m_wnd;
	CCanvasTabCtl		&	m_tc;
	CNesEditor			&	m_nEditor;
	CPaletteBitmap			m_bmView;
	CCanvasCtl				m_canvas;
	CNesLevelInterface		* m_pInterface;
	CV_LEVEL_ID				m_level;
	CObjBuffer			&	m_buffer;

	INT_PTR					OnNotify( LPNMHDR lpnm ) override;
	BOOL					OnInit( LPARAM lParam ) override;
	VOID					OnDestroy() override;

	INT_PTR					Ctl_OnHilite( PNCVNVIEW pnc );
	INT_PTR					Ctl_OnSelect( PNCVNVIEW pnc );
	INT_PTR					Ctl_OnRedraw( PNCVNVIEW pnc );
	INT_PTR					Ctl_OnMapMove( PNCNVIEWMAP pncm );
	INT_PTR					Ctl_OnDblClick( PNCVNVIEW pncm );
	INT_PTR					Ctl_OnRightClick( PNCVNVIEW pncm );
	INT_PTR					Ctl_OnBeginMove( PNCVNVIEW pncm );

	VOID					InsertObject( POINT pt );
	VOID					DeleteObject();
	VOID					DeleteObjects( std::vector<CNesObject*> & vpObj );
	VOID					ChangePointer( CNesLevelItem * pObject );
	VOID					LoopSettings( CNesLoopCommand * pLoop );
	VOID					AreaSettings();
	VOID					ChangeAreaPointer();
	VOID					FollowTheAreaPointer();
	VOID					IncreaseAreaSize( UINT uPage );
	VOID					DecreaseAreaSize( UINT uPage );
	VOID					ShowReferences( UINT uPage );
	VOID					ChangeLevelType();
	VOID					ModifyGround( int x, BOOL fNewGround );
	VOID					RemoveGround( int x );
	BOOL					TakeLevelSnapshot();
	BOOL					PopLevelSnapshot();
	VOID					AreaLinksManager();

	VOID					UpdateUndoMenu();
	VOID					UpdateBufferMenu();
	VOID					UpdateAreaPanel();
	VOID					FollowLevel( CNesLevel * pLevel, UINT uPage );
	VOID					FollowLevel( BYTE bWorld, CNesLevel * pLevel, UINT uPage );

	INT_PTR					TranslateNotify( LPNMHDR lpnm );


public:
	CCanvasTabDlg( HINSTANCE hInstance, CCanvasTabCtl & tc, const CV_LEVEL_ID & cvli );
	~CCanvasTabDlg();
	VOID					Hide();
	VOID					ShowLevelTitle();
	VOID					HideLevelTitle();
	VOID					ShowTab();
	VOID					SizeChanged();
	CNesLevelInterface *	Interface();
	VOID					GetLevelId( CV_LEVEL_ID & cvli ) const;
	BOOL					IsLevel( const CV_LEVEL_ID & cvli ) const;
	HWND					CanvasWindow();

	BOOL					IsCanUndo() const;
	VOID					DoUndo();
	VOID					Update();
	VOID					ReloadLevel();

	VOID					Cmd_InsertObject();
	VOID					Cmd_DeleteObject();
	VOID					Cmd_BufferEvent( BUFFER_EVENT be );
	VOID					Cmd_SelectAll();
	VOID					Cmd_AreaSettings();
	VOID					Cmd_AreaLinksManager();
	VOID					Cmd_ChangeLevelType();
	VOID					Cmd_LoadArea( const std::vector<BYTE> & vLevelData, const std::vector<BYTE> & vObjData );
	VOID					Cmd_DumpArea( std::vector<BYTE> & vLevelData, std::vector<BYTE> & vObjData );

	NES_AREA_PTR			GetAreaPtr() const;
	const CNesLevelHeader & GetAreaHeader() const;
	BYTE					GetAreaType() const;

	BOOL					IsObjectSelected();
	VOID					SetLevelNumber( BYTE bLevel );
	VOID					ScrollToPage( UINT uPage );
	BYTE					GetCurrentPage();
};

class CCanvasTabCtl
{
	const HINSTANCE						m_hInstance;
	CWindow 					&		m_wnd;
	CTabControl					&		m_tc;
	CNesEditor					&		m_nEditor;
	std::vector<CCanvasTabDlg*>			m_vpTab;
	CObjBuffer					&		m_buffer;
	BOOL								m_fLevelTitleVisible;

	VOID		SetSelected( UINT uTab );
	VOID		HideAll();

	CCanvasTabDlg * CurTab() const;

	CString		GetLevelCaption( const CV_LEVEL_ID & cvli );


public:
	CCanvasTabCtl( HINSTANCE hInstance, CWindow & parent, CNesEditor & nEditor, CObjBuffer & objBuffer, CTabControl & tc );

	VOID		GetRect( RECT & rc );

	BOOL		SelectTab( const CV_LEVEL_ID & cvli );
	VOID		SelectNextTab( BOOL fReverse );
	VOID		AddTab( const CV_LEVEL_ID & cvli );
	VOID		UpdateTabCaption( CCanvasTabDlg * pTab );
	INT_PTR		OnNotify( LPNMHDR lpnm );
	CV_LEVEL_ID CurSel() const;
	CCanvasTabDlg * Tab();
	CNesEditor & Editor();
	CObjBuffer & ObjBuffer();
	CWindow &	 Parent();
	BOOL		IsOnTab() const;
	VOID		CloseTab( UINT i );
	VOID		CloseAllTabs();
	VOID		CloseTabWithLevel( CNesLevel * pLevel );
	VOID		SetLevelTitleVisible( BOOL fVisible );
};