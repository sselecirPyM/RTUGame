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
#include "RTU_STATES.h"

const static unsigned int c_maxQueueFrameCount = 2;

struct RTU_INT2
{
	int x;
	int y;
	bool operator==(const RTU_INT2& another)
	{
		return (x == another.x) && (y == another.y);
	}
	bool operator!=(const RTU_INT2& another)
	{
		return (x != another.x) || (y != another.y);
	}
};
struct XMINT3Comparer
{
	bool operator()(const DirectX::XMINT3& Left, const DirectX::XMINT3& Right) const
	{
		return memcmp(&Left, &Right, sizeof(DirectX::XMINT3)) < 0;
	}
};

inline std::string UnicodeToUTF8(const std::wstring& wstr)
{
	std::string ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception& e)
	{

	}
	return ret;
}
inline std::wstring UTF8ToUnicode(const std::string& str)
{
	std::wstring ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception& e)
	{

	}
	return ret;
}