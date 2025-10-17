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

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#pragma warning(disable:4838)
#pragma warning(disable:4091)	// shlobj.h for winXP toolset

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#include "h\NES\NES.h"

#include <Resource.h>

#include "h\GUI\Dialogs\CAboutDlg.h"
#include "h\GUI\Dialogs\CPaletteEditorDlg.h"
#include "h\GUI\Dialogs\EngineHack\CEngineHackSheet.h"
#include "h\GUI\Dialogs\CDemoPlayEditorDlg.h"
#include "h\GUI\Dialogs\CUnusedAreasDlg.h"
#include "h\GUI\\Dialogs\CLinkManagerDlg.h"
#include "h\GUI\Dialogs\CPageReferencesDlg.h"
#include "h\GUI\Dialogs\CWorldEditorDlg.h"
#include "h\GUI\Dialogs\CCreateNewAreaDlg.h"
#include "h\GUI\Dialogs\CChangeAreaTypeDlg.h"
#include "h\GUI\Dialogs\CAreaPropertiesDlgr.h"
#include "h\GUI\Dialogs\CLoopSettingsDlg.h"
#include "h\GUI\Dialogs\CGroundDlg.h"
#include "h\GUI\Dialogs\CNewObjectDlg.h"
#include "h\GUI\\Dialogs\CAreasDlg.h"
#include "h\GUI\Dialogs\CSelectLevelDlg.h"
#include "h\GUI\Dialogs\MainWindow\CCanvasTabDlg.h"
#include "h\GUI\Dialogs\MainWindow\CMainWindow.h"
