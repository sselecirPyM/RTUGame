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
#include "stdafx.h"
#include "IAppCore.h"
#include <iostream>



using namespace Microsoft::WRL;

IAppCore::IAppCore(int width, int height, std::wstring name) :
	m_width(width),
	m_height(height),
	m_title(name),
	m_launchGraphicsOption(0)
{
	WCHAR assetsPath[512];
	GetAssetsPath(assetsPath, _countof(assetsPath));
	m_assetsPath = assetsPath;
}

IAppCore::~IAppCore()
{
}

void IAppCore::SetCustomWindowText(LPCWSTR text)
{
	std::wstring windowText = m_title + L": " + text;
	SetWindowText(m_hwnd, windowText.c_str());
}

// Helper function for parsing any supplied command line args.
_Use_decl_annotations_
void IAppCore::ParseCommandLineArgs(WCHAR* argv[], int argc)
{
	m_connectAddress = L"localhost";
	m_connectPort = L"25565";

	std::wstring* subArgValue[4];
	int subArg = 0;
	int numSubArg = 0;
#define IsArg(argx) _wcsnicmp(&argv[i][1], (argx), wcslen(&argv[i][1])) == 0
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == L'-' || argv[i][0] == L'/')
		{
			subArg = 0;
			numSubArg = 0;
			if (IsArg(L"title"))
			{
				numSubArg = 1;
				subArgValue[0] = &m_title;
			}
			else if (IsArg(L"connect"))
			{
				numSubArg = 2;
				subArgValue[0] = &m_connectAddress;
				subArgValue[1] = &m_connectPort;
			}
			else if (IsArg(L"dx12"))
			{
				m_launchGraphicsOption = 1;
			}
			else if (IsArg(L"vulkan"))
			{
				m_launchGraphicsOption = 2;
			}
			else if (IsArg(L"dx11"))
			{
				m_launchGraphicsOption = 3;
			}
		}
		else
		{
			if (subArg < numSubArg)
			{
				*subArgValue[subArg] = std::wstring(argv[i]);
				subArg++;
			}
		}
	}
#undef IsArg;
	return;
}
