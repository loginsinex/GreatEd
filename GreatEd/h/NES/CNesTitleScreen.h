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

class CNesLevelView;

typedef enum _tagNES_TITLE_SCREEN_OBJ_TYPE
{
	ntsoTilesLine,
	ntsoAttrLine,
	ntsoPaletteLine
} NES_TITLE_SCREEN_OBJ_TYPE, *PNES_TITLE_SCREEN_OBJ_TYPE;

typedef struct _tagNES_TITLE_SCREEN_OBJ
{
	USHORT			uPPUPtr;
	union
	{
		BYTE			behavior;
		struct
		{
			BYTE			counter : 6;
			BYTE			fRepeat : 1;
			BYTE			fVert : 1;
		};
	};
} NES_TITLE_SCREEN_OBJ, *PNES_TITLE_SCREEN_OBJ;

class CNesTitleScreenObject
{
	POINT								m_pt;
	NES_TITLE_SCREEN_OBJ_TYPE			m_type;
	std::vector<BYTE>					m_data;
	BOOL								m_fRepeat;
	BOOL								m_fVertical;
	BYTE								m_uCounter;

	VOID								ParseObject( const NES_TITLE_SCREEN_OBJ & obj, const std::vector<BYTE> & data );

public:
	CNesTitleScreenObject( const NES_TITLE_SCREEN_OBJ & obj, const std::vector<BYTE> & data );
	BOOL						Split( std::vector<CNesTitleScreenObject> & obj ) const;
	VOID						DrawObject( CNesLevelView & view ) const;
};

class CNesTitleScreen
{
	CNesSimpleMMC3							& m_file;
	CNesPointers							& m_eptr;
	std::vector<CNesTitleScreenObject>		m_obj;

public:
	CNesTitleScreen( CNesSimpleMMC3 & file, CNesPointers & eptr );
	~CNesTitleScreen();

	VOID			LoadData();
	VOID			DumpData();

	VOID			DrawTitleScreen( CNesLevelView & view );
};