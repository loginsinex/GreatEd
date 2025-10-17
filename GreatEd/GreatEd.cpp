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
#include "GreatEd.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	ULONG_PTR token;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartup( &token, &input, 0 );

	INT_PTR retval = 0;
	CMainWindow wnd( hInstance );

	CString s;
	s.LoadString( hInstance, IDS_APP_TITLE, 0 );

	retval = wnd.ShowCustomAccel( IDC_PROJECT, NULL, 0, s );

	Gdiplus::GdiplusShutdown( token );
	::CoUninitialize();

    return (int) retval;
}

int _tmain( int argc, char ** argv, char ** env )
{
	return wWinMain( GetModuleHandle( nullptr ), nullptr, GetCommandLineW(), SW_SHOW );
}