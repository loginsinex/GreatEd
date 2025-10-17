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

#define ASSERT_EOF(it,end)	{ if ( it >= end ) throw std::exception("Parse data failed"); }

typedef struct _tagNES_OBJECTS
{
	std::vector<CNesEnemyObject*>			vpEnemy;
	std::vector<CNesLevelItem*>				vpItem;
} NES_OBJECTS, *PNES_OBJECTS;

class CNesEditor;

class CNesLevel
{
	CNesSimpleMMC3 &					m_file;
	const CNesPointers &				m_eptr;
	NES_AREA_PTR						m_lvlPtr;
	const CNesObjectData &				m_objData;
	const CNesPlayerEntrance &			m_player;
	std::map<tstring, DWORD>			m_meta;
	CNesLevelHeader						m_header;
	CNesGameLoopCommand &				m_loop;
	CNesGameLinks &						m_gLinks;
	CNesTitleScreen	&					m_title;
	std::vector<NES_LEVEL_ITEM>			m_lvl;
	std::vector<NES_LEVEL_ENEMY_ITEM>	m_enm;

	CNesLevelItemGroundType				m_liGround;
	NES_OBJECTS							m_obj;

	int									m_iLevelStatus;
	LONG								m_uPages;
	CNesMetatiles &						m_mTiles;

	VOID								UpdateLevelStatus();
	VOID								FreeObjects();

	VOID								ValidateLevelPalette( CNesLevelView & view );

	CNesLevelItem *						FetchMasvItemC( const NES_LEVEL_ITEM & item );
	CNesLevelItem *						FetchMasvItemD( const NES_LEVEL_ITEM & item );
	CNesLevelItem *						FetchMasvItemF( const NES_LEVEL_ITEM & item );
	CNesLevelItem *						FetchLongItem( const NES_LEVEL_ITEM & item );
	CNesLevelItem *						FetchSingleItem( const NES_LEVEL_ITEM & item );
	CNesLevelItem *						FetchLevelItem( const NES_LEVEL_ITEM & item );
	CNesEnemyObject *					FetchEnemyItem( const NES_LEVEL_ENEMY_ITEM & item );	
	LONG								ParseLevelData( std::vector<BYTE>::const_iterator areaIt, std::vector<BYTE>::const_iterator areaEnd );
	VOID								ParseGroundData( NES_GROUND_INFO & ngi, const NES_LEVEL_ITEM & item, LONG uPage );
	LONG								ParseEnemyData( std::vector<BYTE>::const_iterator atBegin, std::vector<BYTE>::const_iterator atEnd, std::vector<NES_LINK> & vLinks );
	size_t								SerializeLevelData( std::vector<BYTE> & vLevelData );
	size_t								SerializeEnemyData( std::vector<BYTE> & vEnemyData );
	VOID								SplitItemsToPages( std::map<BYTE, std::vector<NES_LEVEL_ITEM>> & mPage, std::vector<NES_LEVEL_ITEM> & vItem );
	VOID								SplitEnemiesToPages( std::map<BYTE, std::vector<NES_LEVEL_ENEMY_ITEM>> & mPage, std::vector<NES_LEVEL_ENEMY_ITEM> & vItem );
	VOID								AddItemToDump( std::vector<BYTE> & vDump, const NES_LEVEL_ITEM & item );
	VOID								AddEnemyToDump( std::vector<BYTE> & vDump, const NES_LEVEL_ENEMY_ITEM & item );
	VOID								DumpItems( std::vector<BYTE> & vDump, const std::vector<NES_LEVEL_ITEM> & vItem );
	VOID								DumpEnemies( std::vector<BYTE> & vDump, const std::vector<NES_LEVEL_ENEMY_ITEM> & vItem );
	VOID								DumpPointersAsObjects( std::vector<NES_LEVEL_ENEMY_ITEM> & vItem );
	VOID								DumpLoops( std::map<BYTE, std::vector<CNesLoopCommand*>> & mLoop );
	VOID								TakeLoopsSnapshot( std::vector<NES_LOOP_COMMAND> & vLoop );
	VOID								RevertLoopsState( const std::vector<NES_LOOP_COMMAND> & vLoop );
	VOID								GetLoopObjects( std::vector<CNesLoopCommand*> & vItemLoop );

	VOID								ParseLoops();

	VOID								LevelSizeChanged();

	VOID								RemoveDuplicatePointers( const std::vector<NES_LEVEL_ENEMY_ITEM> & v, std::vector<NES_LEVEL_ENEMY_ITEM> & tail );
	// BOOL								IsEqualPointersVectorTail( const std::vector<NES_LEVEL_ENEMY_ITEM> & v, const std::vector<NES_LEVEL_ENEMY_ITEM> & tail );

	BOOL								AddPage( UINT uBeforePage );
	BOOL								DelPage( UINT uPage );
	VOID								CopyLoops( CNesLevel * pCopy );

	VOID								InitDrawQueue( std::vector<NES_DRAW_QUEUE_ITEM> & vDrawQueue );
	VOID								SetupBehindFlag( const std::vector<NES_DRAW_QUEUE_ITEM> & vdqi, BOOL fBehind );
	VOID								SetupGroundItems( std::vector<NES_DRAW_QUEUE_ITEM> & vdqi, BOOL fBehind );

	friend CNesEditor;

public:
	CNesLevel( 
		CNesSimpleMMC3 & file, 
		NES_AREA_PTR uPtr, 
		const CNesPointers & eptr, 
		CNesMetatiles & mtiles, 
		const CNesObjectData & objData,
		CNesGameLoopCommand & loop,
		CNesGameLinks & gLinks,
		CNesTitleScreen & title,
		const CNesPlayerEntrance & plr );

	~CNesLevel();

	CNesSimpleMMC3	&	ROM();
	const CNesPointers	&	Pointers();
	const CNesObjectData &	ObjData();

	int				LoadLevelData(
		const std::vector<BYTE> & vAreaData,
		const std::vector<BYTE> & vEnemyData,
		std::vector<NES_LINK> & vLinks
	);
	
	int				GetLevelStatus();

	NES_AREA_PTR	AreaPtr() const;
	const CNesLevelHeader & Header() const;
	BOOL			UpdateHeader( const CNesLevelHeader & header );

	NES_LEVEL_TYPE	AreaType() const;
	SIZE			LevelSize() const;
	BYTE			PagesCount() const;

	VOID			InitView( CNesLevelView & view );
	VOID			InitObjectsQueue();
	VOID			DrawLevel( CNesLevelView & view, BYTE bWorldId );
	VOID			InitObjects();
	BYTE			GetBgColor();
	BYTE			LevelBgCtl();

	CNesObject *	ObjectFromPoint( int x, int y );
	BOOL			ObjectsFromRect( const RECT & rc, std::vector<CNesObject*> & vpObj );
	VOID			ResortObjects();

	BOOL			MoveGround( int uOldX, int uNewX );
	BOOL			GetGround( int X, std::map<NES_GROUND_TYPE, NES_GROUND_INFO> & gr );
	VOID			SetGround( int x, const std::map<NES_GROUND_TYPE, NES_GROUND_INFO> & gr );
	BOOL			EraseGround( int x );

	int				AssignLink( CNesLink * pLink, int uBeginPoint );
	int				AssignGroundLink( CNesLink * pLink );

	BOOL			GetLevelBinaryData( std::vector<BYTE> & vLevelData, std::vector<BYTE> & vEnemyData );

	// WARNING: Dont use this methods to update area's id or type
	// this will be overwritten in DumpAreas
	// instead of this use CNesEditor::UpdateAreaType !!!
	VOID			UpdateAreaType( NES_LEVEL_TYPE bType );
	VOID			SetAreaId( BYTE bId );

	CNesLevel *		Copy();

	CNesLevelItem *		AddLevelItem( const NES_LEVEL_ITEM & item );
	CNesEnemyObject *	AddEnemyItem( const NES_LEVEL_ENEMY_ITEM & item );
	BOOL				IsValidObject( CNesObject * pObject );
	BOOL				DeleteObject( CNesObject * pObject );
	BOOL				DumpLoops( std::vector<NES_LOOP_COMMAND> & vnlp );
	CNesObject *		ObjectById( size_t idObject, BOOL fSprite );
	BOOL				ObjectId( CNesObject * pObject, size_t & idObject, BOOL & fSprite );
	BOOL				FindLoopObjectId( CNesLoopCommand * pObject, size_t & idObject );

	BOOL				GetAreaPointer( BYTE bWorld, CNesLevel ** pLevel, BYTE & uPage );
	BOOL				SetAreaPointer( BYTE bWorld, CNesLevel * pLevel, BYTE uPage );
	BOOL				ReadAreaLinks( std::map<BYTE, std::pair<UINT, CNesLevel*>>& mLinks );
	BOOL				RemoveAreaLink( BYTE bWorld );

	BOOL				IsObjectPresentOnPageRange( BYTE bPageFrom, BYTE bPageTo, BYTE uItemId, BYTE uSpecial = 0, BOOL fSprite = FALSE );
	const CNesPlayerEntrance PlayerEntrance();
	VOID				Meta( const tstring & strName, DWORD dwValue );
	DWORD				Meta( const tstring & strName ) const;

};