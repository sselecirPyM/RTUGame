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
#include "../Voxel/Chunk16.h"
#include "NetBlock.h"
class NetContext
{
public:
	void SwapDynamicContext();
	bool Send(int header, const char* data, int size);
	std::vector<NetBlock17k>* L_smallSizeReceiedDatas_w = &m_smallSizeReceiedDatas[0];
	std::vector<NetBlock17k>* m_smallSizeReceiedDatas_r = &m_smallSizeReceiedDatas[1];
	std::vector<NetBlock17k> m_smallSizeReceiedDatas[2];
	std::mutex lock_smallSizeReceiedData;

	std::vector<NetBlock64k>* L_bigSizeReceiedDatas_w = &m_bigSizeReceiedDatas[0];
	std::vector<NetBlock64k>* m_bigSizeReceiedDatas_r = &m_bigSizeReceiedDatas[1];
	std::vector<NetBlock64k> m_bigSizeReceiedDatas[2];
	std::mutex lock_bigSizeReceiedData;

	std::vector<NetBlock17k>* L_smallSizeSendDatas_w = &m_smallSizeSendDatas[0];
	std::vector<NetBlock17k>* m_smallSizeSendDatas_r = &m_smallSizeSendDatas[1];
	std::vector<NetBlock17k> m_smallSizeSendDatas[2];
	std::mutex lock_smallSizeSendData;

	uint64_t m_lastSendTimeStamp;
	HANDLE m_sendEvent;
};

