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

#include "Controls.h"

CTabControl::CTabControl( HINSTANCE hInstance, USHORT uId, INT x, INT y, INT cx, INT cy, DWORD dwStyle, DWORD dwStyleEx, WORD wAnchor )
	: CControl( hInstance, uId, WC_TABCONTROL, nullptr, x, y, cx, cy, dwStyle, dwStyleEx, wAnchor )
{

}

VOID CTabControl::PostCreate()
{
#ifdef UNICODE
	cSendMessage( TCM_SETUNICODEFORMAT, TRUE );
#endif
}

VOID CTabControl::GetRect( RECT & rc, BOOL fFullArea )
{
	POINT pt = { 0 };
	HWND hParent = GetParent( *this );
	GetWindowRect( *this, &rc );
	ScreenToClient( hParent, &pt );
	OffsetRect( &rc, pt.x, pt.y );

	cSendMessage( TCM_ADJUSTRECT, fFullArea, (LPARAM)&rc );
}

VOID CTabControl::ResetContent()
{
	cSendMessage( TCM_DELETEALLITEMS );
}

VOID CTabControl::DeleteItem( UINT uIndex )
{
	cSendMessage( TCM_DELETEITEM, uIndex );
}

VOID CTabControl::DeselectAll( BOOL fExceptCurSel )
{
	cSendMessage( TCM_DESELECTALL, fExceptCurSel );
}

UINT CTabControl::CurSel() const
{
	return static_cast<UINT>( cSendMessage( TCM_GETCURSEL ) );
}

VOID CTabControl::CurSel( UINT uIndex )
{
	cSendMessage( TCM_SETCURSEL, uIndex );
}

UINT CTabControl::Count() const
{
	return static_cast<UINT>( cSendMessage( TCM_GETITEMCOUNT ) );
}

BOOL CTabControl::GetTabRect( UINT uIndex, RECT & rc ) const
{
	return static_cast<BOOL>( cSendMessage( TCM_GETITEMRECT, uIndex, (LPARAM)&rc ) );
}

UINT CTabControl::RowsCount() const
{
	return static_cast<UINT>( cSendMessage( TCM_GETROWCOUNT ) );
}

VOID CTabControl::HiliteItem( UINT uIndex, BOOL fHilite )
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_STATE;
	tci.dwStateMask = TCIS_HIGHLIGHTED;
	tci.dwState = ( fHilite ? TCIS_HIGHLIGHTED : 0 );
	cSendMessage( TCM_SETITEM, uIndex, (LPARAM)&tci );
}

UINT CTabControl::AddTab( LPCTSTR pszText, LPARAM lParam, INT iImage, BOOL fPressed, BOOL fHilite )
{
	return InsertTab( Count(), pszText, lParam, iImage, fPressed, fHilite );
}

HWND CTabControl::ToolTipWindow() const
{
	return (HWND) cSendMessage( TCM_GETTOOLTIPS );
}

VOID CTabControl::ToolTipWindow( HWND hTooltip )
{
	if ( !IsWindow( hTooltip ) ) return;

	cSendMessage( TCM_SETTOOLTIPS, (WPARAM)hTooltip );
}

UINT CTabControl::InsertTab( UINT uIndex, LPCTSTR pszText, LPARAM lParam, INT iImage, BOOL fPressed, BOOL fHilite )
{
	TCITEM tci = { 0 };
	tci.mask |= TCIF_PARAM;
	tci.lParam = lParam;

	if ( pszText && pszText[ 0 ] )
	{
		tci.mask |= TCIF_TEXT;
		tci.pszText = (LPTSTR) pszText;
	}

	if ( iImage >= 0 )
	{
		tci.mask |= TCIF_IMAGE;
		tci.iImage = iImage;
	}

	if ( fPressed || fHilite )
	{
		tci.mask |= TCIF_STATE;
		if ( fPressed ) tci.dwState |= TCIS_BUTTONPRESSED;
		if ( fHilite ) tci.dwState |= TCIS_HIGHLIGHTED;

		tci.dwStateMask = ( TCIS_BUTTONPRESSED | TCIS_HIGHLIGHTED );
	}

	return static_cast<UINT>( cSendMessage( TCM_INSERTITEM, uIndex, (LPARAM)&tci ) );
}

DWORD CTabControl::ExtendedStyle( DWORD dwExStyle )
{
	return static_cast<DWORD>( cSendMessage( TCM_SETEXTENDEDSTYLE, dwExStyle, dwExStyle ) );
}

VOID CTabControl::ImageList( HIMAGELIST himl )
{
	cSendMessage( TCM_SETIMAGELIST, 0, (LPARAM)himl );
}

HIMAGELIST CTabControl::ImageList() const
{
	return (HIMAGELIST) cSendMessage( TCM_GETIMAGELIST );
}

CString CTabControl::Text( UINT uIndex ) const
{
	CString sText;
	const size_t uTextSize = 1024;
	TCITEM tci = { 0 };
	tci.mask = TCIF_TEXT;
	tci.pszText = sText.GetBuffer( uTextSize + 1 );
	tci.cchTextMax = uTextSize;
	cSendMessage( TCM_GETITEM, uIndex, (LPARAM)&tci );
	sText.ReleaseBuffer();
	return sText;
}

VOID CTabControl::Text( UINT uIndex, const CString sText )
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_TEXT;
	tci.pszText = (LPTSTR) sText.GetString();
	cSendMessage( TCM_SETITEM, uIndex, (LPARAM)&tci );
}

LPARAM CTabControl::Param( UINT uIndex ) const
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_PARAM;
	cSendMessage( TCM_GETITEM, uIndex, (LPARAM)&tci );
	return tci.lParam;
}

VOID CTabControl::Param( UINT uIndex, LPARAM lParam )
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_PARAM;
	tci.lParam = lParam;
	cSendMessage( TCM_SETITEM, uIndex, (LPARAM)&tci );
}

INT CTabControl::Image( UINT uIndex ) const
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_IMAGE;
	cSendMessage( TCM_GETITEM, uIndex, (LPARAM)&tci );
	return tci.iImage;
}

VOID CTabControl::Image( UINT uIndex, INT iImage )
{
	TCITEM tci = { 0 };
	tci.mask = TCIF_IMAGE;
	tci.iImage = iImage;
	cSendMessage( TCM_SETITEM, uIndex, (LPARAM)&tci );
}

VOID CTabControl::SetItemsSize( UINT cx, UINT cy )
{
	cSendMessage( TCM_SETITEMSIZE, 0, MAKELPARAM( cx, cy ) );
}

VOID CTabControl::SetMinimumItemWidth( UINT cx )
{
	cSendMessage( TCM_SETMINTABWIDTH, 0, cx );
}

VOID CTabControl::SetItemIconPadding( UINT x, UINT y )
{
	cSendMessage( TCM_SETPADDING, 0, MAKELPARAM( x, y ) );
}