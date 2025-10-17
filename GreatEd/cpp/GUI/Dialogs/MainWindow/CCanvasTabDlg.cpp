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

CObjBuffer::CObjBuffer()
{

}

VOID CObjBuffer::Copy( std::vector<CNesObject*>& vObject )
{
	POINT leftPoint = { 1024, 1024 };
	m_vObjects.clear();

	for ( auto pObj : vObject )
	{
		BUFFER_OBJECT bo = { 0 };
		const POINT ptObject = pObj->GetRealPoint();

		if ( ptObject.x < leftPoint.x ) leftPoint.x = ptObject.x;
		if ( ptObject.y < leftPoint.y ) leftPoint.y = ptObject.y;


		if ( pObj->IsSprite() )
		{
			CNesEnemyObject * pEnemy = static_cast<CNesEnemyObject*>( pObj );
			bo.fObject = TRUE;
			bo.nObject.item.objectId = pEnemy->Item().item.objectId;
			bo.nObject.pt = ptObject;
			bo.nObject.item.Y = LOBYTE( pEnemy->GetRealPoint().y );
		}
		else
		{
			CNesLevelItem * pItem = static_cast<CNesLevelItem*>( pObj );
			bo.fObject = FALSE;
			bo.nItem.item.itemId = pItem->Item().item.itemId;
			bo.nItem.pt = ptObject;
			bo.nItem.item.Y = LOBYTE( pItem->GetRealPoint().y );
		}

		m_vObjects.push_back( bo );
	}

	// normalize points
	for ( auto & bo : m_vObjects )
	{
		if ( bo.fObject )
		{
			bo.nObject.pt.x -= leftPoint.x;
			bo.nObject.pt.y -= leftPoint.y;
		}
		else
		{
			bo.nItem.pt.x -= leftPoint.x;
			bo.nItem.pt.y -= leftPoint.y;
		}
	}
}


BOOL CObjBuffer::Paste( CNesLevel * pLevel, const POINT & pt, std::vector<CNesObject*> & vpCreatedObjects )
{
	vpCreatedObjects.clear();
	int uMaxX = 16 * pLevel->PagesCount() - 1;

	for ( const auto & bo : m_vObjects )
	{
		CNesObject * pObj = nullptr;
		if ( bo.fObject )
		{
			NES_LEVEL_ENEMY_ITEM nEnemy = bo.nObject;
			nEnemy.pt.x = min( pt.x + nEnemy.pt.x, uMaxX );
			nEnemy.pt.y = min( NES_SPECIAL_OBJECT_D - 1, static_cast<int>( nEnemy.pt.y ) + pt.y );
			pObj = pLevel->AddEnemyItem( nEnemy );
		}
		else
		{
			NES_LEVEL_ITEM nItem = bo.nItem;
			nItem.pt.x = min( pt.x + nItem.pt.x, uMaxX );

			if ( nItem.item.Y < NES_SPECIAL_OBJECT_C )
			{
				nItem.pt.y = min( NES_SPECIAL_OBJECT_C - 1, static_cast<int>( nItem.pt.y ) + pt.y );
			}
			else
			{
				nItem.pt.y = nItem.item.Y;
			}
			pObj = pLevel->AddLevelItem( nItem );
		}

		if ( pObj )
		{
			vpCreatedObjects.push_back( pObj );
		}
	}

	return ( vpCreatedObjects.size() > 0 );
}

BOOL CObjBuffer::CanPaste()
{
	return ( m_vObjects.size() > 0 );
}

///////////////////////////

CCanvasTabDlg::CCanvasTabDlg( HINSTANCE hInstance, CCanvasTabCtl & tc, const CV_LEVEL_ID & cvli )
	: CDialog( hInstance, WS_CHILD | DS_CONTROL | WS_TABSTOP | DS_SHELLFONT, 0, 0, 0, 0, 0, TEXT(""), 0 ),
	m_tc( tc ), m_nEditor( tc.Editor() ), m_level( cvli ), m_buffer( tc.ObjBuffer() ), m_wnd( tc.Parent() ),
	m_canvas( hInstance, IDCU_MAINWINDOW_CANVAS, 0, 0, -1, -1, WS_VISIBLE | WS_TABSTOP, 0, ANCHOR_FULL )
{
	BOOL fRenderTitle = ( !cvli.bLevel && !cvli.bWorld );
	m_pInterface = m_nEditor.GetInterface( m_bmView, cvli.bWorld, cvli.pLevel, 0, fRenderTitle );
	pushctl( m_canvas );
}

CCanvasTabDlg::~CCanvasTabDlg()
{
	delete m_pInterface;
}

BOOL CCanvasTabDlg::OnInit( LPARAM lParam )
{
	m_canvas.SetView( m_pInterface );
	m_canvas.RenderView();
	InvalidateRect( m_canvas, nullptr, TRUE );

	SetFocus( m_canvas );
	return FALSE;
}

VOID CCanvasTabDlg::OnDestroy()
{
	m_canvas.DelView();
}

INT_PTR CCanvasTabDlg::OnNotify( LPNMHDR lpnm )
{
	switch ( lpnm->idFrom )
	{
	case IDCU_MAINWINDOW_CANVAS:
		{
			switch ( lpnm->code )
			{
			case NCVN_OBJECT_HILITE: return Ctl_OnHilite( (PNCVNVIEW)lpnm );
			case NCVN_OBJECT_SELECT: return Ctl_OnSelect( (PNCVNVIEW)lpnm );
			case NCVN_REDRAW_REQUEST: return Ctl_OnRedraw( (PNCVNVIEW)lpnm );
			case NCVN_RIGHT_CLICK: return Ctl_OnRightClick( (PNCVNVIEW)lpnm );
			case NCVN_DBL_CLICK: return Ctl_OnDblClick( (PNCVNVIEW)lpnm );
			case NCVN_MAP_MOVE: return Ctl_OnMapMove( (PNCNVIEWMAP)lpnm );
			case NCVN_BEGIN_MOVE: return Ctl_OnBeginMove( (PNCVNVIEW)lpnm );
			}
			break;
		}
	}
	
	return 0;
}

INT_PTR CCanvasTabDlg::TranslateNotify( LPNMHDR lpnm )
{
	return SendMessage( m_wnd, WM_TABNOTIFY, (WPARAM) this, (LPARAM) lpnm );
}

INT_PTR CCanvasTabDlg::Ctl_OnHilite( PNCVNVIEW pnc )
{
	return TranslateNotify( (LPNMHDR) pnc );
}

INT_PTR CCanvasTabDlg::Ctl_OnSelect( PNCVNVIEW pnc )
{
	return TranslateNotify( (LPNMHDR)pnc );
}

INT_PTR CCanvasTabDlg::Ctl_OnRedraw( PNCVNVIEW pnc )
{
	return 0;
}

INT_PTR CCanvasTabDlg::Ctl_OnMapMove( PNCNVIEWMAP pncm )
{
	return TranslateNotify( (LPNMHDR)pncm );
}

INT_PTR CCanvasTabDlg::Ctl_OnDblClick( PNCVNVIEW pncm )
{
	return TranslateNotify( (LPNMHDR)pncm );
}

INT_PTR CCanvasTabDlg::Ctl_OnRightClick( PNCVNVIEW pncm )
{
	HMENU hMenu = CreatePopupMenu();
	HMENU hSubMenu = CreatePopupMenu();
	MENUITEMINFO mi = { 0 };

	mi.cbSize = sizeof( mi );
	mi.fMask = MIIM_SUBMENU | MIIM_STRING;
	mi.fType = MFT_STRING;
	mi.hSubMenu = hSubMenu;
	mi.dwTypeData = (LPWSTR)TEXT( "Pages" );

	AppendMenu( hMenu, 0, 0x1001, TEXT( "Add an object...\tInsert" ) );

	if ( pncm->pObject )
	{
		UpdateBufferMenu();

		if ( !pncm->pObject->IsSprite() )
		{
			CNesLevelItem * pObj = static_cast<CNesLevelItem*>( pncm->pObject );
			if ( pObj->EntranceType() != etNotEntrance )
			{
				AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
				AppendMenu( hMenu, 0, 0x1003, TEXT( "Change entrance pointer..." ) );
			}

			if ( 0x4B == pObj->Item().item.itemId && NES_SPECIAL_OBJECT_D == pObj->Item().item.Y )
			{
				AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
				AppendMenu( hMenu, 0, 0x1004, TEXT( "Change loop settings..." ) );
			}
		}
	}

	if ( pncm->pt.x > 0 )
	{
		std::map<NES_GROUND_TYPE, NES_GROUND_INFO> gr;
		AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );

		if ( m_level.pLevel->GetGround( pncm->pt.x, gr ) )
		{
			AppendMenu( hMenu, 0, 0x1200, TEXT( "Modify this ground..." ) );
			AppendMenu( hMenu, 0, 0x1202, TEXT( "Remove this ground..." ) );
		}
		else
		{
			AppendMenu( hMenu, 0, 0x1201, TEXT( "Add new ground..." ) );
		}
	}

	AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
	AppendMenu( hSubMenu, 0, 0x1101, TEXT( "Insert page here" ) );

	if ( !m_nEditor.IsPageReferenced( m_level.pLevel, pncm->pt.x / 16 ) )
	{
		AppendMenu( hSubMenu, 0, 0x1102, TEXT( "Remove this page" ) );
		AppendMenu( hSubMenu, MFS_DISABLED | MFS_GRAYED, 0x1103, TEXT( "References..." ) );
	}
	else
	{
		AppendMenu( hSubMenu, MFS_DISABLED | MFS_GRAYED, 0x1102, TEXT( "Remove this page" ) );
		AppendMenu( hSubMenu, 0, 0x1103, TEXT( "References..." ) );
	}

	if ( m_level.pLevel->Header().IsCloudLevel() )
	{
		AppendMenu( hMenu, 0, 0x1050, TEXT( "Change area pointer..." ) );
		AppendMenu( hMenu, 0, 0x1051, TEXT( "Follow the pointer..." ) );
		AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
	}

	// AppendMenu( hMenu, 0, 0x1005, TEXT( "Area properties..." ) );
	InsertMenuItem( hMenu, 0x1005, FALSE, &mi );
	// AppendMenu( hMenu, 0, 0x1105, TEXT( "Change area type..." ) );

	AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
	AppendMenu( hMenu, ( pncm->pObject ? 0 : MFS_DISABLED | MFS_GRAYED ), 0x1006, TEXT( "Copy\tCtrl+C" ) );
	AppendMenu( hMenu, ( pncm->pObject ? 0 : MFS_DISABLED | MFS_GRAYED ), 0x1007, TEXT( "Cut\tCtrl+X" ) );
	AppendMenu( hMenu, ( m_buffer.CanPaste() ? 0 : MFS_DISABLED | MFS_GRAYED ), 0x1008, TEXT( "Paste\tCtrl+V" ) );
	AppendMenu( hMenu, ( pncm->pObject ? 0 : MFS_DISABLED | MFS_GRAYED ), 0x1002, TEXT( "Delete object(s)...\tDel" ) );
	AppendMenu( hMenu, 0, 0x1009, TEXT( "Select all\tCtrl+A" ) );
	AppendMenu( hMenu, MF_SEPARATOR, -1, nullptr );
	AppendMenu( hMenu, ( IsCanUndo() ? 0 : MFS_DISABLED | MFS_GRAYED ), 0x100A, TEXT( "Undo\tCtrl+Z" ) );

	POINT pt{};
	GetCursorPos(&pt);
	switch (TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, *this, nullptr))
	{
	case 0x1001: InsertObject( pncm->pt ); break;
	case 0x1002: DeleteObject(); break;
	case 0x1003: ChangePointer( (CNesLevelItem*)pncm->pObject ); break;
	case 0x1004: LoopSettings( (CNesLoopCommand*)pncm->pObject ); break;
	case 0x1005: AreaSettings(); break;
	case 0x1006: Cmd_BufferEvent( beCopy ); break;
	case 0x1007: Cmd_BufferEvent( beCut ); break;
	case 0x1008: Cmd_BufferEvent( bePaste ); break;
	case 0x1009: Cmd_SelectAll(); break;
	case 0x100A: DoUndo(); break;
	case 0x1050: ChangeAreaPointer(); break;
	case 0x1051: FollowTheAreaPointer(); break;
	case 0x1101: IncreaseAreaSize( pncm->pt.x / 16 ); break;
	case 0x1102: DecreaseAreaSize( pncm->pt.x / 16 ); break;
	case 0x1103: ShowReferences( pncm->pt.x / 16 ); break;
	case 0x1105: ChangeLevelType(); break;
	case 0x1200: ModifyGround( pncm->pt.x, FALSE ); break;
	case 0x1201: ModifyGround( pncm->pt.x, TRUE ); break;
	case 0x1202: RemoveGround( pncm->pt.x ); break;
	}

	DestroyMenu( hSubMenu );
	DestroyMenu( hMenu );

	return 0;
}

INT_PTR CCanvasTabDlg::Ctl_OnBeginMove( PNCVNVIEW pncm )
{
	TakeLevelSnapshot();
	return 0;
}

////////////////////////////////////////////////////

VOID CCanvasTabDlg::InsertObject( POINT pt )
{
	CNesLevel * pCopy = m_level.pLevel->Copy();
	if ( pCopy )
	{
		CNewObjectDlg dlg( GetInstance() );
		CPaletteBitmap bm;
		CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, pCopy );

		dlg.Set_Param( pCopy, pInterface, pt );

		if ( dlg.Show( this ) )
		{
			CNesObject * pNewObject = nullptr;

			TakeLevelSnapshot();
			if ( !dlg.IsSpriteCreated() )
			{
				NES_LEVEL_ITEM item = { 0 };
				dlg.Get_Param( item );
				pNewObject = m_level.pLevel->AddLevelItem( item );
			}
			else
			{
				NES_LEVEL_ENEMY_ITEM item = { 0 };
				dlg.Get_Param( item );
				pNewObject = m_level.pLevel->AddEnemyItem( item );
			}

			if ( !pNewObject )
			{
				PopLevelSnapshot();
				ShowError( TEXT( "Too many objects for this area.\nCan't add new object." ) );
			}
			else
			{
				m_canvas.RenderView();
				InvalidateRect( m_canvas, nullptr, TRUE );
			}
		}

		delete pInterface;
		delete pCopy;
	}
}

VOID CCanvasTabDlg::DeleteObject()
{
	CNesObjectsNames nm( GetInstance() );
	std::vector<CNesObject*> vpObject;
	if ( !m_canvas.GetSelectedObject( vpObject ) )
	{
		return;
	}

	CNesObject * pObj = vpObject[ 0 ];
	const BOOL fMultipleSelection = ( vpObject.size() > 1 );

	CString str, strFormat;
	if ( !fMultipleSelection )
	{
		if ( pObj->IsSprite() )
			str = nm.GetNameOfEnemy( (CNesEnemyObject*)pObj );
		else
			str = nm.GetNameOfItem( (CNesLevelItem*)pObj );

		strFormat.Format( TEXT( "Are you sure that you want to delete '%s'?" ), str );
	}
	else
	{
		strFormat = TEXT( "Are you sure that you want to delete selected objects?" );
	}

	if ( ConfirmF( strFormat, TRUE, str.GetString() ) )
	{
		DeleteObjects( vpObject );
	}
}

VOID CCanvasTabDlg::DeleteObjects( std::vector<CNesObject*>& vpObj )
{
	TakeLevelSnapshot();
	for ( auto pObject : vpObj )
	{
		m_level.pLevel->DeleteObject( pObject );
	}

	m_canvas.DropSelection();
	m_canvas.RenderView();
	InvalidateRect( m_canvas, nullptr, TRUE );
}

VOID CCanvasTabDlg::ChangePointer( CNesLevelItem * pObject )
{
	CNesLevelsKeeper * pKeeper = m_nEditor.GetAreaList();
	CAreasDlg dlg( GetInstance(), pKeeper );
	dlg.Set_Obj( m_level.bWorld, pObject );
	if ( dlg.Show( this ) )
	{
		CNesLevel * pLevel = nullptr;
		BYTE uTargetPage = 0;
		BOOL fFreePointers = FALSE;

		dlg.Get_Param( &pLevel, uTargetPage, fFreePointers );

		TakeLevelSnapshot();

		if ( fFreePointers && etVerticalPipe == pObject->EntranceType() )
		{
			pObject->RemoveLinks();
			m_canvas.RenderView();
		}
		else if ( pKeeper->SetObjectTarget( m_level.bWorld, pObject, pLevel, uTargetPage ) )
		{
			m_canvas.RenderView();
		}
		InvalidateRect( m_canvas, nullptr, TRUE );
	}

	delete pKeeper;
}

VOID CCanvasTabDlg::LoopSettings( CNesLoopCommand * pLoop )
{
	CNesLevel * pCopy = m_level.pLevel->Copy();
	if ( pCopy )
	{
		CLoopSettingsDlg dlg( GetInstance() );
		CPaletteBitmap bm;
		CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, pCopy );
		size_t uObjectId = 0;
		BOOL fSprite = FALSE;

		if ( pCopy->FindLoopObjectId( pLoop, uObjectId ) )
		{
			NES_LOOP_COMMAND nlp = { 0 };
			pLoop->GetLoop( nlp );
			dlg.Set_Param( pCopy, pInterface, uObjectId );
			if ( dlg.Show( this ) )
			{
				TakeLevelSnapshot();
				dlg.Get_Param( nlp );
				pLoop->TriggerPage( nlp.bPage );
				pLoop->AllowedHeight( nlp.Y / 16 );
				pLoop->LoopConnected( nlp.bIdInGroup );
				pLoop->InitObject();
				m_canvas.RenderView();
				InvalidateRect( m_canvas, nullptr, TRUE );
			}
		}
		delete pInterface;
		delete pCopy;
	}
}

VOID CCanvasTabDlg::AreaSettings()
{
	CNesLevel * pLevel = m_level.pLevel->Copy();
	if ( pLevel )
	{
		CPaletteBitmap bm;
		CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, pLevel );
		CAreaPropertiesDlg dlg( GetInstance() );

		dlg.Set_Param( pLevel, pInterface );

		if ( dlg.Show( this ) )
		{
			CNesLevelHeader header;
			CNesLevel * pTargetLevel = nullptr;
			BYTE uTargetPage = 0;
			dlg.Get_Param( header );
			TakeLevelSnapshot();
			m_level.pLevel->UpdateHeader( header );
			m_canvas.DropSelection();
			m_canvas.RenderView();
			InvalidateRect( m_canvas, nullptr, TRUE );
			UpdateAreaPanel();
		}

		delete pInterface;
		delete pLevel;
	}
}

VOID CCanvasTabDlg::ChangeAreaPointer()
{
	if ( !m_level.pLevel->Header().IsCloudLevel() )
	{
		return;
	}

	CNesLevelsKeeper * pKeeper = m_nEditor.GetAreaList();
	CNesLevel * pTargetLevel = nullptr;
	BYTE uPage = 0;
	m_level.pLevel->GetAreaPointer( m_level.bWorld, &pTargetLevel, uPage );

	CAreasDlg dlg( GetInstance(), pKeeper );

	dlg.Set_Obj( m_level.bWorld, m_level.pLevel, pTargetLevel, uPage );

	if ( dlg.Show( this ) )
	{
		BOOL dummy = FALSE;
		dlg.Get_Param( &pTargetLevel, uPage, dummy );
		TakeLevelSnapshot();
		m_level.pLevel->SetAreaPointer( m_level.bWorld, pTargetLevel, uPage );
		m_canvas.RenderView();
		InvalidateRect( m_canvas, nullptr, TRUE );
	}

	delete pKeeper;
}

VOID CCanvasTabDlg::FollowTheAreaPointer()
{
	CNesLevel * pTargetLevel = nullptr;
	BYTE uPage = 0;

	if ( !m_level.pLevel->Header().IsCloudLevel() )
	{
		return;
	}

	if ( m_level.pLevel->GetAreaPointer( m_level.bWorld, &pTargetLevel, uPage ) )
	{
		// pointer can be NULL for current world
		if ( pTargetLevel )
		{
			FollowLevel( pTargetLevel, uPage );
		}
	}
	else
	{
		m_canvas.ScrollTo( 0 );
	}
}

VOID CCanvasTabDlg::IncreaseAreaSize( UINT uPage )
{
	TakeLevelSnapshot();
	m_nEditor.AddPage( m_level.pLevel, uPage );
	m_canvas.RenderView();
}

VOID CCanvasTabDlg::DecreaseAreaSize( UINT uPage )
{
	if ( Confirm( TEXT( "WARNING: All objects within this page will be removed.\nDo you want to continue?" ) ) )
	{
		TakeLevelSnapshot();
		if ( !m_nEditor.DelPage( m_level.pLevel, uPage ) )
		{
			PopLevelSnapshot();
			ShowError( TEXT( "Page cannot be deleted,\nbecause some pointers referenced to it." ) );
		}
		else
		{
			m_canvas.RenderView();
		}
	}
}

VOID CCanvasTabDlg::ShowReferences( UINT uPage )
{
	if ( m_nEditor.IsFileLoaded() )
	{
		std::vector<std::pair<BYTE, CNesLevel*>> vLinks;

		if ( m_nEditor.GetSourceLinkList( m_level.pLevel, uPage, vLinks ) )
		{
			CPaletteBitmap bm;
			CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, m_level.pLevel );
			CPageReferencesDlg dlg( GetInstance() );
			std::pair<BYTE, CNesLevel*> tg;

			dlg.Set_Param( pInterface, uPage, m_level.pLevel->PagesCount() - 1, vLinks );
			LRESULT result = dlg.Show( this );

			if ( result )
			{
				UINT uNewPage = 0;
				BOOL fResult = TRUE;
				dlg.Get_Param( uNewPage, tg );
				TakeLevelSnapshot();
				switch ( result )
				{
				case 1: fResult = m_nEditor.SetSourcePageLink( m_level.pLevel, uPage, uNewPage ); break;
				case 2: FollowLevel( tg.first, tg.second, 0 ); break;
				}

				if ( !fResult )		// WARNING! Loaded area can be changed in case of result == 2! DON'T POP SNAPSHOT IN THIS CASE!
				{
					PopLevelSnapshot();
				}
			}

			delete pInterface;
		}
	}
}

VOID CCanvasTabDlg::ChangeLevelType()
{
	CNesLevel * pLevel = m_level.pLevel->Copy();
	if ( pLevel )
	{
		CPaletteBitmap bm;
		CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, pLevel );
		CChangeAreaTypeDlg dlg( GetInstance() );
		dlg.Set_Param( pLevel, pInterface );

		if ( dlg.Show( this ) )
		{
			NES_LEVEL_TYPE type;
			dlg.Get_Param( type );
			m_nEditor.UpdateAreaType( m_level.pLevel, type );
			UpdateAreaPanel();
			ReloadLevel();
		}

		delete pLevel;
		delete pInterface;
	}
}

VOID CCanvasTabDlg::ModifyGround( int x, BOOL fNewGround )
{
	NES_GROUND_INFO ngi = { 0 };
	CNesLevel * pLevel = m_level.pLevel->Copy();
	if ( pLevel )
	{
		CPaletteBitmap bm;
		CNesLevelInterface * pInterface = m_nEditor.GetInterface( bm, m_level.bWorld, pLevel );
		CGroundDlg dlg( GetInstance() );
		NES_GROUND_PLACE ngp = { 0 };
		std::map<NES_GROUND_TYPE, NES_GROUND_INFO> mngi;

		if ( !fNewGround )
		{
			m_level.pLevel->GetGround( x, mngi );
		}
		else
		{
			ngi.bGroundCtl = 1;
			mngi[ ngtGroundMod ] = ngi;
		}

		ngp.X = x;
		ngp.gr = mngi;
		dlg.Set_Param( pLevel, pInterface, ngp );

		if ( dlg.Show( this ) )
		{
			dlg.Get_Param( ngp );
			TakeLevelSnapshot();
			m_level.pLevel->SetGround( x, ngp.gr );
			m_canvas.RenderView();
			InvalidateRect( m_canvas, nullptr, TRUE );
		}

		delete pInterface;
		delete pLevel;
	}
}

VOID CCanvasTabDlg::RemoveGround( int x )
{
	if ( Confirm( TEXT( "Remove this ground modifier?" ) ) )
	{
		TakeLevelSnapshot();
		if ( m_level.pLevel->EraseGround( x ) )
		{
			m_canvas.RenderView();
			InvalidateRect( m_canvas, nullptr, TRUE );
		}
		else
		{
			PopLevelSnapshot();
			ShowAlert( TEXT( "Unable to delete this modifier!" ) );
		}
	}
}

BOOL CCanvasTabDlg::TakeLevelSnapshot()
{
	BOOL fResult = m_nEditor.TakeLevelSnapshot( m_level.pLevel );
	UpdateUndoMenu();
	return fResult;
}

BOOL CCanvasTabDlg::PopLevelSnapshot()
{
	BOOL fResult = m_nEditor.PopLevelSnapshot( m_level.pLevel );
	UpdateUndoMenu();
	return fResult;
}

VOID CCanvasTabDlg::AreaLinksManager()
{
	size_t id = 1;
	CNesLevelItem * pObject = static_cast<CNesLevelItem*>( m_level.pLevel->ObjectById( id++, FALSE ) );
	LEVEL_ITEM_LINK mLinks;

	std::map<BYTE, std::pair<UINT, CNesLevel*>> mAreaLinks;
	m_level.pLevel->ReadAreaLinks( mAreaLinks );
	if ( mAreaLinks.size() > 0 )
	{
		WORLD_OBJECT_LINK & wol = mLinks[ nullptr ];
		for ( const auto & lnk : mAreaLinks )
		{
			wol[ lnk.first ] = { lnk.second, FALSE };
		}
	}

	while ( pObject )
	{
		if ( pObject->IsEnterableObject() )
		{
			WORLD_OBJECT_LINK & wol = mLinks[ pObject ];
			std::map<BYTE, std::pair<UINT, CNesLevel*>> mObjLinks;
			pObject->ReadLinks( mObjLinks );
			for ( const auto & lnk : mObjLinks )
			{
				wol[ lnk.first ] = { lnk.second, FALSE };
			}
		}
		pObject = static_cast<CNesLevelItem*>( m_level.pLevel->ObjectById( id++, FALSE ) );
	};

	if ( !mLinks.size() )
	{
		ShowError( TEXT( "No pointers found in this area" ) );
		return;
	}

	CNesLevelsKeeper * pKeeper = m_nEditor.GetAreaList();
	CLinkManagerDlg dlg( GetInstance() );
	dlg.Set_Param( pKeeper, mLinks, m_level.pLevel, m_level.bWorld );

	if ( dlg.Show( this ) )
	{
		TakeLevelSnapshot();
		dlg.Get_Param( mLinks );

		for ( const auto & obj : mLinks )
		{
			CNesLevelItem * pItem = obj.first;
			for ( const auto & wol : obj.second )
			{
				const BOOL fDelete = wol.second.second;
				const BYTE bWorld = wol.first;

				if ( fDelete )
				{
					if ( pItem )
					{
						pItem->RemoveLink( bWorld );
					}
					else
					{
						m_level.pLevel->RemoveAreaLink( bWorld );
					}
				}
			}
		}

		m_nEditor.PointersChanged();

		m_canvas.RenderView();
		InvalidateRect( m_canvas, nullptr, TRUE );
	}

	delete pKeeper;

}

////////////////////////////////////////////////////

VOID CCanvasTabDlg::Cmd_InsertObject()
{
	TakeLevelSnapshot();
	InsertObject( m_canvas.GetLastPoint() );
}

VOID CCanvasTabDlg::Cmd_DeleteObject()
{
	DeleteObject();
}

VOID CCanvasTabDlg::Cmd_BufferEvent( BUFFER_EVENT be )
{
	std::vector<CNesObject*> vpObj;
	const BOOL fSelected = m_canvas.GetSelectedObject( vpObj );

	switch ( be )
	{
	case beCopy:
		{
			if ( fSelected )
			{
				m_buffer.Copy( vpObj );
			}
			break;
		}
	case beCut:
		{
			if ( fSelected )
			{
				m_buffer.Copy( vpObj );
				DeleteObjects( vpObj );
			}
			break;
		}
	case bePaste:
		{
			const POINT pt = m_canvas.GetLastPoint();
			TakeLevelSnapshot();
			if ( m_buffer.Paste( m_level.pLevel, pt, vpObj ) )
			{
				m_canvas.SetSelection( vpObj );
				Update();
			}
			else
			{
				PopLevelSnapshot();
				ShowError( TEXT( "Too many objects for this area.\nCan't paste object." ) );
			}
			break;
		}
	}

	UpdateBufferMenu();
}

VOID CCanvasTabDlg::Cmd_SelectAll()
{
	std::vector<CNesObject*> vpObject;
	CNesObject * pObject = nullptr;

	// retreive all objects from level
	size_t idObj = 1;
	while ( pObject = m_level.pLevel->ObjectById( idObj++, FALSE ) )
	{
		vpObject.push_back( pObject );
	}

	idObj = 0;
	while ( pObject = m_level.pLevel->ObjectById( idObj++, TRUE ) )
	{
		vpObject.push_back( pObject );
	}

	// send this list to canvas
	m_canvas.SetSelection( vpObject );
	Update();
}

VOID CCanvasTabDlg::Cmd_AreaSettings()
{
	AreaSettings();
}

VOID CCanvasTabDlg::Cmd_ChangeLevelType()
{
	ChangeLevelType();
}

VOID CCanvasTabDlg::Cmd_LoadArea( const std::vector<BYTE>& vLevelData, const std::vector<BYTE>& vObjData )
{
	try
	{
		std::vector<NES_LINK> vLink;
		TakeLevelSnapshot();
		m_level.pLevel->LoadLevelData( vLevelData, vObjData, vLink );
		m_level.pLevel->InitObjects();
		ReloadLevel();
	}
	catch ( std::exception & e )
	{
		PopLevelSnapshot();
		UNREFERENCED_PARAMETER( e );
	}
}

VOID CCanvasTabDlg::Cmd_DumpArea( std::vector<BYTE>& vLevelData, std::vector<BYTE>& vObjData )
{
	m_level.pLevel->GetLevelBinaryData( vLevelData, vObjData );
}

VOID CCanvasTabDlg::Cmd_AreaLinksManager()
{
	AreaLinksManager();
}

NES_AREA_PTR CCanvasTabDlg::GetAreaPtr() const
{
	return m_level.pLevel->AreaPtr();
}

const CNesLevelHeader & CCanvasTabDlg::GetAreaHeader() const
{
	return m_level.pLevel->Header();
}

BYTE CCanvasTabDlg::GetAreaType() const
{
	return m_level.pLevel->AreaType();
}

BOOL CCanvasTabDlg::IsObjectSelected()
{
	std::vector<CNesObject*> vpObject;
	return m_canvas.GetSelectedObject( vpObject );
}

VOID CCanvasTabDlg::SetLevelNumber( BYTE bLevel )
{
	m_level.bLevel = bLevel;
	m_tc.UpdateTabCaption( this );
}

VOID CCanvasTabDlg::ScrollToPage( UINT uPage )
{
	m_canvas.ScrollTo( uPage );
}

BYTE CCanvasTabDlg::GetCurrentPage()
{
	return m_canvas.GetPageFromScroll();
}

VOID CCanvasTabDlg::UpdateUndoMenu()
{
	NMHDR nm;
	nm.code = TCVNTF_UPDATEUNDOMENU;
	nm.hwndFrom = *this;
	TranslateNotify( &nm );
}

VOID CCanvasTabDlg::UpdateBufferMenu()
{
	NMHDR nm;
	nm.code = TCVNTF_UPDATEBUFFERMENU;
	nm.hwndFrom = *this;
	TranslateNotify( &nm );
}

VOID CCanvasTabDlg::UpdateAreaPanel()
{
	NMHDR nm;
	nm.code = TCVNTF_UPDATEPANEL;
	nm.hwndFrom = *this;
	TranslateNotify( &nm );
}

VOID CCanvasTabDlg::FollowLevel( CNesLevel * pLevel, UINT uPage )
{
	FollowLevel( m_level.bWorld, pLevel, uPage );
}

VOID CCanvasTabDlg::FollowLevel( BYTE bWorld, CNesLevel * pLevel, UINT uPage )
{
	CVNTFNM_FOLLOW nm;
	nm.nm.code = TCVNTF_FOLLOWLEVEL;
	nm.nm.hwndFrom = *this;
	nm.bWorld = bWorld;
	nm.pLevel = pLevel;
	nm.uPage = uPage;
	TranslateNotify( (LPNMHDR)&nm );
}


VOID CCanvasTabDlg::Hide()
{
	Visible( FALSE );
}

VOID CCanvasTabDlg::ShowLevelTitle()
{
	BOOL fRenderTitle = ( !m_level.bLevel && !m_level.bWorld );
	if ( fRenderTitle )
	{
		m_pInterface->SetLevelTitleVisible( TRUE );
	}
}

VOID CCanvasTabDlg::HideLevelTitle()
{
	m_pInterface->SetLevelTitleVisible( FALSE );
}

VOID CCanvasTabDlg::ShowTab()
{
	m_canvas.DropSelection();
	m_canvas.RenderView();
	SizeChanged();
	UpdateUndoMenu();
	UpdateAreaPanel();
	UpdateBufferMenu();
	m_tc.UpdateTabCaption( this );
	Visible( TRUE );
}

VOID CCanvasTabDlg::SizeChanged()
{
	RECT rc = { 0 };
	m_tc.GetRect( rc );
	SetWindowPos( *this, nullptr, rc.left, rc.top, ( rc.right - rc.left ), ( rc.bottom - rc.top ), 0 );
}

CNesLevelInterface * CCanvasTabDlg::Interface()
{
	return m_pInterface;
}

VOID CCanvasTabDlg::GetLevelId( CV_LEVEL_ID & cvli ) const
{
	cvli = m_level;
}

BOOL CCanvasTabDlg::IsLevel( const CV_LEVEL_ID & cvli ) const
{
	return ( m_level.bWorld == cvli.bWorld && m_level.pLevel == cvli.pLevel );
}

HWND CCanvasTabDlg::CanvasWindow()
{
	return m_canvas;
}

BOOL CCanvasTabDlg::IsCanUndo() const
{
	return m_nEditor.IsLevelCanReverted( m_level.pLevel );
}

VOID CCanvasTabDlg::DoUndo()
{
	if ( IsCanUndo() )
	{
		m_nEditor.RevertLevelState( m_level.pLevel );
		// PopLevelSnapshot();
		m_canvas.DropSelection();
		Update();
	}
}

VOID CCanvasTabDlg::Update()
{
	m_canvas.RenderView();
	InvalidateRect( *this, nullptr, TRUE );
}

VOID CCanvasTabDlg::ReloadLevel()
{
	m_canvas.DelView();
	delete m_pInterface;

	m_pInterface = m_nEditor.GetInterface( m_bmView, m_level.bWorld, m_level.pLevel );
	m_canvas.SetView( m_pInterface );
	Update();
	m_tc.UpdateTabCaption( this );
}

CCanvasTabCtl::CCanvasTabCtl( HINSTANCE hInstance, CWindow & parent, CNesEditor & nEditor, CObjBuffer & objBuffer, CTabControl & tc )
	: m_hInstance( hInstance ), m_wnd( parent ), m_tc( tc ), m_nEditor( nEditor ), m_buffer( objBuffer ), m_fLevelTitleVisible( TRUE )
{

}

VOID CCanvasTabCtl::GetRect( RECT & rc )
{
	m_tc.GetRect( rc );
}

VOID CCanvasTabCtl::HideAll()
{
	for ( auto & dlg : m_vpTab )
	{
		dlg->Hide();
	}
}

VOID CCanvasTabCtl::SetSelected( UINT uTab )
{
	CCanvasTabDlg * pTab = (CCanvasTabDlg*)m_tc.Param( uTab );

	HideAll();

	if ( pTab )
	{
		if ( m_fLevelTitleVisible ) pTab->ShowLevelTitle();
		else pTab->HideLevelTitle();
		pTab->ShowTab();
	}
}

INT_PTR CCanvasTabCtl::OnNotify( LPNMHDR lpnm )
{
	switch ( lpnm->code )
	{
		case TCN_SELCHANGE:
			{
				SetSelected( m_tc.CurSel() );
				break;
			}
	}

	return 0;
}

BOOL CCanvasTabCtl::SelectTab( const CV_LEVEL_ID & cvli )
{
	UINT c = m_tc.Count();
	for ( UINT i = 0; i < c; ++i )
	{
		CCanvasTabDlg * pTab = (CCanvasTabDlg*)m_tc.Param( i );
		if ( pTab && pTab->IsLevel( cvli ) )
		{
			m_tc.CurSel( i );
			SetSelected( i );
			return TRUE;
		}
	}

	return FALSE;
}

VOID CCanvasTabCtl::SelectNextTab( BOOL fReverse )
{
	UINT c = m_tc.Count(), sel = m_tc.CurSel();
	if ( c < 2 ) return;

	if ( fReverse )
	{
		if ( sel > 0 ) sel--;
		else sel = c - 1;
	}
	else
	{
		sel++;
		if ( sel >= c ) sel = 0;
	}

	SetSelected( sel );
	m_tc.CurSel( sel );

}

CString CCanvasTabCtl::GetLevelCaption( const CV_LEVEL_ID & cvli )
{
	CString strWorld;
	CString strType[] = { TEXT( "Water" ), TEXT( "Ground" ), TEXT( "Underground" ), TEXT( "Castle" ) };

	if ( cvli.bLevel != 0xFF )
	{
		strWorld.Format( TEXT( "World %d-%d [%s #%d]" ), cvli.bWorld + 1, cvli.bLevel + 1, strType[ cvli.pLevel->AreaType() ].GetString(), cvli.pLevel->AreaPtr().bAreaId + 1 );
	}
	else
	{
		strWorld.Format( TEXT( "World %d [%s #%d]" ), cvli.bWorld + 1, strType[ cvli.pLevel->AreaType() ].GetString(), cvli.pLevel->AreaPtr().bAreaId + 1 );
	}

	return strWorld;
}

VOID CCanvasTabCtl::AddTab( const CV_LEVEL_ID & cvli )
{
	CCanvasTabDlg * pDlg = new CCanvasTabDlg( m_hInstance, *this, cvli );
	pDlg->Create( &m_wnd );
	m_vpTab.push_back( pDlg );
	
	UINT uCurSel = 0;
	uCurSel = m_tc.AddTab( GetLevelCaption( cvli ), (LPARAM)pDlg );
	m_tc.CurSel( uCurSel );
	SetSelected( uCurSel );
}

VOID CCanvasTabCtl::UpdateTabCaption( CCanvasTabDlg * pTab )
{
	UINT c = m_tc.Count();
	for ( UINT i = 0; i < c; ++i )
	{
		CCanvasTabDlg * _pTab = (CCanvasTabDlg*)m_tc.Param( i );
		if ( _pTab == pTab )
		{
			CV_LEVEL_ID cvli;
			pTab->GetLevelId( cvli );

			m_tc.Text( i, GetLevelCaption( cvli ) );
			break;
		}
	}
}

CCanvasTabDlg * CCanvasTabCtl::CurTab() const
{
	return (CCanvasTabDlg*)m_tc.Param( m_tc.CurSel() );
}

CV_LEVEL_ID CCanvasTabCtl::CurSel() const
{
	CCanvasTabDlg * pTab = CurTab();
	CV_LEVEL_ID cvli = { 0, 0, nullptr };
	if ( pTab )
	{
		pTab->GetLevelId( cvli );
	}

	return cvli;
}

CCanvasTabDlg * CCanvasTabCtl::Tab()
{
	return CurTab();
}

CNesEditor & CCanvasTabCtl::Editor()
{
	return m_nEditor;
}

CObjBuffer & CCanvasTabCtl::ObjBuffer()
{
	return m_buffer;
}

CWindow & CCanvasTabCtl::Parent()
{
	return m_wnd;
}

BOOL CCanvasTabCtl::IsOnTab() const
{
	return ( NULL != m_tc.Param( m_tc.CurSel() ) );
}

VOID CCanvasTabCtl::CloseTab( UINT i )
{
	CCanvasTabDlg * pTab = (CCanvasTabDlg*)m_tc.Param( i );
	if ( pTab )
	{
		DestroyWindow( *pTab );
		m_tc.DeleteItem( i );

		auto it = std::find( m_vpTab.begin(), m_vpTab.end(), pTab );
		if ( it != m_vpTab.end() ) m_vpTab.erase( it );

		delete pTab;

		UINT c = m_tc.Count();
		if ( c )
		{
			while ( i >= c ) i--;
			SetSelected( i );
			m_tc.CurSel( i );
		}
	}
}

VOID CCanvasTabCtl::CloseAllTabs()
{
	for ( auto & pTab : m_vpTab )
	{
		DestroyWindow( *pTab );
		delete pTab;
	}

	m_vpTab.clear();
	m_tc.ResetContent();
}

VOID CCanvasTabCtl::CloseTabWithLevel( CNesLevel * pLevel )
{
	UINT c = m_tc.Count();
	for ( UINT _i = c; _i > 0; _i-- )
	{
		UINT i = _i - 1;
		CCanvasTabDlg * pTab = (CCanvasTabDlg*)m_tc.Param( i );
		
		if ( pTab )
		{
			CV_LEVEL_ID cvli;
			pTab->GetLevelId( cvli );
			if ( cvli.pLevel == pLevel ) CloseTab( i );
		}		
	}
}

VOID CCanvasTabCtl::SetLevelTitleVisible( BOOL fVisible )
{
	m_fLevelTitleVisible = fVisible;
}