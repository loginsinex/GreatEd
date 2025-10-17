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

CMenu::CMenu(HMENU hMenu)
    : m_hMenu(hMenu)
{
}

CMenu::CMenu(HINSTANCE hInstance, INT_PTR idMenu)
    : m_hMenu(LoadMenu(hInstance, MAKEINTRESOURCE(idMenu)))
{
}

CMenu::operator HMENU() const
{
    return m_hMenu;
}

HMENU CMenu::FindMenuWithId(HMENU hMenu, UINT uId)
{
    auto menuItemsCount = GetMenuItemCount(hMenu);
    for (decltype(menuItemsCount)i = 0;i < menuItemsCount;++i)
    {
        if (auto hSubMenu = GetSubMenu(hMenu, i);hSubMenu)
        {
            if (auto hItemMenu = FindMenuWithId(hSubMenu, uId); hItemMenu)
                return hItemMenu;
        }
        else
        {
            if (uId == GetMenuItemID(hMenu, i))
                return hMenu;
        }
    }

    return nullptr;
}

HMENU CMenu::FindMenuWithId(UINT uId) const
{
    return FindMenuWithId(m_hMenu, uId);
}

void CMenu::SetItem(UINT id, SetItem_t fn)
{
    if (auto hMenu = FindMenuWithId(id); hMenu)
        fn(hMenu, id);
}

bool CMenu::IsItemChecked(UINT uId) const
{
    auto hMenu = FindMenuWithId(uId);
    if (!hMenu)
        return false;

    return (MF_CHECKED == (MF_CHECKED & GetMenuState(hMenu, uId, MF_BYCOMMAND)));
}