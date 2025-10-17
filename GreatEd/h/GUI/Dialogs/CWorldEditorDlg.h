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

typedef enum _tagTVITEM_TYPE
{
	tviWorld,
	tviLevel,
	tviArea
} TVITEM_TYPE, *PTVITEM_TYPE;

typedef struct _tagTVITEM_LEVEL
{
	TVITEM_TYPE			tvi;
	size_t				bWorld;
	size_t				bLevel;
	size_t				bArea;
	CNesLevel			*pLevel;
} TVITEM_LEVEL, *PTVITEM_LEVEL;

class CWorldEditorDlg : public CDialog
{
	CPaletteBitmap			m_bmView;
	CCanvasCtl				m_cv;
	CTreeView				m_tv;
	CButtonControl			m_btnAddLevel;
	CButtonControl			m_btnRemoveLevel;
	CStaticControl			m_stHalfPage;
	CEditControl			m_edHalfPage;
	CUpDownControl			m_udHalfPage;
	BOOL					m_fModified;

	CNesLevelsKeeper		*m_pKeeper;
	NES_GAME_AREA_LIST		m_nagWorlds;
	NES_GAME_AREA_HP_LIST	m_nagHP;
	std::map<PTVITEM_LEVEL, HTREEITEM>	m_tvItem;
	PTVITEM_LEVEL			m_ptvSelected;
	TVITEM_LEVEL			m_tvSelected;
	BOOL					m_fUpdatePending;

	VOID					BeginUpdate();
	VOID					EndUpdate();
	BOOL					IsUpdatePending();

	BOOL					OnInit( LPARAM lParam ) override;
	VOID					OnOK() override;
	VOID					OnButton( USHORT uId ) override;
	INT_PTR					OnNotify( LPNMHDR lpnm ) override;
	VOID					OnCommand( USHORT uCmd, USHORT uId, HWND hCtl );
	VOID					OnNCDestroy() override;

	VOID					UpdateTree();
	VOID					FreeTVItems();

	PTVITEM_LEVEL			AddWorld( size_t bWorld );
	PTVITEM_LEVEL			AddLevel( size_t bWorld, size_t bLevel );
	PTVITEM_LEVEL			AddArea( size_t bWorld, size_t bLevel, size_t uArea );
	HTREEITEM				AddItem( HTREEITEM hParentItem, PTVITEM_LEVEL pItem, BOOL fBold, LPCTSTR pszString, ... );
	VOID					SelectLevel( HTREEITEM hItem, PTVITEM_LEVEL pItem );
	VOID					SelectLevel( HTREEITEM hItem );

	VOID					NewLevel( PTVITEM_LEVEL pItem );
	VOID					DelLevel( PTVITEM_LEVEL pItem );

	BOOL					CreateLevelItem( BYTE bWorld, CNesLevel * pSelectedLevel, std::vector<CNesLevel*> & vpLevel );

public:
	CWorldEditorDlg( HINSTANCE hInstance );
	
	VOID					Set_Param( const NES_GAME_AREA_LIST & nag, const NES_GAME_AREA_HP_LIST & nhp, CNesLevelsKeeper * pKeeper, BYTE bWorld, BYTE bLevel, CNesLevel * pSelected );
	VOID					Get_Param( NES_GAME_AREA_LIST & nag, NES_GAME_AREA_HP_LIST & nhp, BYTE & bWorld, BYTE & bLevel, CNesLevel ** pLevel );
	BOOL					IsModified() const;
};