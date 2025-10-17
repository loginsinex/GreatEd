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


typedef struct _tagNES_LINK
{
	BYTE			uSourceWorld;
	BYTE			uTargetPage;
	NES_AREA_PTR	aptrTarget;
	POINT			ptLink;
} NES_LINK, *PNES_LINK;

class CNesLevel;
class CNesObject;

class CNesLink
{
	const BYTE					m_bWorld;
	CNesLevel *					m_pSourceLevel;
	CNesLevel *					m_pDestLevel;
	BYTE						m_uDestPage;
	int							m_ref;

public:
	CNesLink( BYTE uSourceWorld, CNesLevel * pSourceLevel );
	VOID				SetTarget( CNesLevel * pDestLevel, BYTE uDestPage );

	operator CNesLevel *( );

	CNesLevel *			SourceLevel();
	CNesLevel *			TargetLevel();
	BYTE				TargetPage();
	BYTE				SourceWorld();

	VOID				AddRef();
	BOOL				Release();
	BOOL				IsReferenced();
};

class CNesGameLinks
{
	std::map<std::pair<BYTE, CNesLevel*>, std::vector<CNesLink*>>			m_mLink;

public:
	CNesGameLinks();
	~CNesGameLinks();

	CNesLink			* AddLink( BYTE bWorld, CNesLevel * pLevel );
	VOID				RemoveLink( CNesLink * pLink );
	VOID				FreeStaleLinks();

	BOOL				GetSourceList( CNesLevel * pLevel, UINT uPage, std::vector<std::pair<BYTE, CNesLevel*>> & vLink );
	BOOL				SetSourcePage( CNesLevel * pLevel, UINT uPage, UINT uNewPage );

	BOOL				IsPageReferenced( CNesLevel * pLevel, UINT uPage );
	BOOL				GetReferencesForTarget( std::vector<CNesLevel*> & vpSource, CNesLevel * pLevel, BOOL fIncludeSelfReference = TRUE ) const;
};