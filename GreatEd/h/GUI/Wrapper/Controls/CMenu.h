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

class CMenu
{
    using SetItem_t = std::function<void(HMENU, UINT)>;

    HMENU       m_hMenu = nullptr;

    static HMENU FindMenuWithId(HMENU hMenu, UINT uId);
    HMENU       FindMenuWithId(UINT uId) const;

    void        SetItem(UINT id, SetItem_t fn);

public:
    CMenu(HMENU hMenu);
    CMenu(HINSTANCE hInstance, INT_PTR idMenu);

    operator HMENU() const;
    bool IsItemChecked(UINT uId) const;

    template <typename... Args>
    VOID RadioCheckItems(UINT uChecked, Args... id)
    {
        auto fn = [&](HMENU hMenu, UINT uId)
            {
                MENUITEMINFO mii{};
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_FTYPE | MIIM_STATE;
                if (!GetMenuItemInfo(hMenu, uId, false, &mii))
                    return;

                mii.fType |= MFT_RADIOCHECK;
                if (uId == uChecked)
                    mii.fState |= MF_CHECKED;
                else
                    mii.fState &= ~MF_CHECKED;

                mii.fMask = MIIM_FTYPE | MIIM_STATE;
                SetMenuItemInfo(hMenu, uId, false, &mii);
            };

        (SetItem(id, fn), ...);
    };

    template <typename... Args>
    VOID EnableItems(BOOL fEnable, Args... id)
    {
        UINT uEnable = (MF_BYCOMMAND | (fEnable ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED)));
        auto fn = [&](HMENU hMenu, UINT uId)
            {
                EnableMenuItem(hMenu, uId, uEnable);
            };

        (SetItem(id, fn), ...);
    };

    template <typename... Args>
    VOID CheckItems(BOOL fCheck, Args... id)
    {
        UINT uChecked = (MF_BYCOMMAND | (fCheck ? (MF_CHECKED) : (MF_UNCHECKED)));
        auto fn = [&](HMENU hMenu, UINT uId)
            {
                CheckMenuItem(hMenu, uId, uChecked);
            };

        (SetItem(id, fn), ...);
    };


};