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

class CTabControl : public CControl
{
	VOID		PostCreate() override;

public:
	CTabControl( HINSTANCE hInstance, USHORT uId,
		INT x, INT y, INT cx, INT cy,
		DWORD dwStyle, DWORD dwStyleEx = 0, WORD wAnchor = 0 );

	VOID		GetRect( RECT & rc, BOOL fFullArea = FALSE );
	VOID		ResetContent();
	VOID		DeleteItem( UINT uIndex );
	VOID		DeselectAll( BOOL fExceptCurSel = TRUE );
	UINT		CurSel() const;
	VOID		CurSel( UINT uIndex );
	UINT		Count() const;
	BOOL		GetTabRect( UINT uIndex, RECT & rc ) const;
	UINT		RowsCount() const;
	HWND		ToolTipWindow() const;
	VOID		ToolTipWindow( HWND hTooltip );
	VOID		HiliteItem( UINT uIndex, BOOL fHilite );
	UINT		AddTab( LPCTSTR pszText, LPARAM lParam, INT iImage = -1, BOOL fPressed = FALSE, BOOL fHilite = FALSE );
	UINT		InsertTab( UINT uIndex, LPCTSTR pszText, LPARAM lParam, INT iImage = -1, BOOL fPressed = FALSE, BOOL fHilite = FALSE );
	DWORD		ExtendedStyle( DWORD dwExStyle );
	VOID		ImageList( HIMAGELIST himl );
	HIMAGELIST	ImageList() const;
	CString		Text( UINT uIndex ) const;
	VOID		Text( UINT uIndex, const CString sText );
	LPARAM		Param( UINT uIndex ) const;
	VOID		Param( UINT uIndex, LPARAM lParam );
	INT			Image( UINT uIndex ) const;
	VOID		Image( UINT uIndex, INT iImage );
	VOID		SetItemsSize( UINT cx, UINT cy );
	VOID		SetMinimumItemWidth( UINT cx );
	VOID		SetItemIconPadding( UINT x, UINT y );


};