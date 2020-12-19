//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
#pragma once

#include "DXUtil.h"

class IAppCore
{
public:
    IAppCore(int width, int height, std::wstring name);
    virtual ~IAppCore();

    virtual void OnInit() = 0;
    virtual void OnMouseMove(int x, int y, std::uint32_t flag) {}
    virtual void OnKeyDown(UINT8 key, LONG lParam, std::uint32_t flag)   {}
    virtual void OnKeyUp(UINT8 key, LONG lParam, std::uint32_t flag)     {}
    virtual void OnLeftButtonDown(int x, int y, std::uint32_t flag) {}
    virtual void OnLeftButtonUp(int x, int y, std::uint32_t flag) {}
    virtual void OnSizeChanged(int width, int height, bool minimized) = 0;
    virtual void OnDestroy() = 0;

    UINT GetWidth() const           { return m_width; }
    UINT GetHeight() const          { return m_height; }
    const WCHAR* GetTitle() const   { return m_title.c_str(); }

    void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

    HWND m_hwnd;
    HINSTANCE m_hinstance;
protected:

    void SetCustomWindowText(LPCWSTR text);

    // Viewport dimensions.
    int m_width;
    int m_height;

    std::wstring m_connectAddress;
    std::wstring m_connectPort;

    // Root assets path.
    std::wstring m_assetsPath;

    // Window title.
    std::wstring m_title;

    std::int32_t m_launchGraphicsOption;
};
