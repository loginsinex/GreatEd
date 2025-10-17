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

#pragma pack(push,1)
typedef enum _tagPARAM_TYPE : BYTE
{
	ptWorld,
	ptLevel,
	ptArea
} PARAM_TYPE, *PPARAM_TYPE;

typedef union _tagWORLD_PARAM
{
	DWORD		lParam;
	struct
	{
		BYTE			bWorld:3;
		BYTE			bLevel:5;
		BYTE			bArea;
		PARAM_TYPE		type;
		NES_AREA_PTR	lType;
	};
} WORLD_PARAM, *PWORLD_PARAM;
#pragma pack(pop)

class CSelectLevelDlg : public CDialog
{
	CTreeView				m_tv;
	NES_AREA_PTR			m_aptr;
	BYTE					m_world;
	BYTE					m_level;
	NES_GAME_AREA_LIST		m_nAreaList;
	WORLD_PARAM				m_lParam;

	BOOL					OnInit( LPARAM lParam ) override;
	VOID					OnOK() override;
	INT_PTR					OnNotify( LPNMHDR lpnm );

public:
	CSelectLevelDlg( HINSTANCE hInstance );

	VOID					SetParam( const NES_GAME_AREA_LIST & nAreaList, BYTE bWorld, BYTE bLevel, CNesLevel * pLevel );
	VOID					GetParam( CNesLevel ** pLevel, BYTE & bWorld, BYTE & bLevel );
};