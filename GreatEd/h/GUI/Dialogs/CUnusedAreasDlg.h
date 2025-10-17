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

class CUnusedAreasDlg : public CDialog
{
	CPaletteBitmap					m_bmView;
	CNesLevelsKeeper			*	m_pKeeper;
	std::vector<CNesLevel*>			m_vpUnusedLevels;
	std::vector<CNesLevel*>			m_vpCheckedLevels;

	CListView						m_lv;
	CCanvasCtl						m_cv;
	CButtonControl					m_btnRemove;
	CButtonControl					m_btnLeave;

	BOOL							OnInit( LPARAM lParam ) override;
	VOID							OnOK() override;
	VOID							OnDestroy() override;
	INT_PTR							OnNotify( LPNMHDR lpnm ) override;

	VOID							RenderSelected();
	VOID							FreeView();

public:
	CUnusedAreasDlg( HINSTANCE hInstance );

	VOID							Set_Param( CNesLevelsKeeper * pKeeper, const std::vector<CNesLevel*> & vpUnusedLevels );
	VOID							Get_Param( std::vector<CNesLevel*> & vpUnusedLevels );
};