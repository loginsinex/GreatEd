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

class CNesDemoPlay
{
	CNesSimpleMMC3							& m_file;
	CNesPointers							& m_eptr;
	std::vector<std::pair<BYTE, BYTE>>		m_vDemoData;	// timer / action

public:
	CNesDemoPlay( CNesSimpleMMC3 & file, CNesPointers & eptr );

	VOID				LoadData();
	VOID				DumpData();

	VOID				DemoPlayData( std::vector<std::pair<BYTE, BYTE>> & vDemoData );
	VOID				WriteDemoPlayData( const std::vector<std::pair<BYTE, BYTE>> & vDemoData );

};