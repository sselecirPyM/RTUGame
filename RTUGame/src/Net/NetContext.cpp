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
#include "NetContext.h"
#include <codecvt>
#include <iostream>

void NetContext::SwapDynamicContext()
{
	{
		std::lock_guard<std::mutex> guard(lock_smallSizeReceiedData);
		std::swap(m_smallSizeReceiedDatas_r, L_smallSizeReceiedDatas_w);
	}
	{
		std::lock_guard<std::mutex> guard(lock_bigSizeReceiedData);
		std::swap(m_bigSizeReceiedDatas_r, L_bigSizeReceiedDatas_w);
	}
}

bool NetContext::Send(int header, const char* data, int size)
{
	NetBlock17k netBlock;
	memcpy(netBlock.m_data, data, size);
	netBlock.m_header = header;
	netBlock.m_size = size;
	{
		std::lock_guard<std::mutex> guard(lock_smallSizeSendData);
		if (L_smallSizeSendDatas_w->size() < 64)
			L_smallSizeSendDatas_w->emplace_back(netBlock);
	}
	SetEvent(m_sendEvent);
	return true;
}
