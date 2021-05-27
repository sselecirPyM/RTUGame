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
#include "../Common/RTUCommon.h"
enum SEQUENCE_INPUT_FLAG : std::uint32_t
{
	SEQUENCE_INPUT_FLAG_KEYBOARD_DOWN = 0,
	SEQUENCE_INPUT_FLAG_KEYBOARD_UP = 1,
	SEQUENCE_INPUT_FLAG_MOUSE_MOVE = 2,
	SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_DOWN = 3,
	SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_UP = 4,
};
struct AppSequenceInput
{
	AppSequenceInput(UINT8 key, std::uint64_t timeStamp, SEQUENCE_INPUT_FLAG flag)
	{
		Key = key;
		Flag = flag;
		TimeStamp = timeStamp;
	}
	AppSequenceInput(int x,int y, std::uint64_t timeStamp, SEQUENCE_INPUT_FLAG flag, std::uint32_t flag1)
	{
		X = x;
		Y = y;
		TimeStamp = timeStamp;
		Flag = flag;
		Flag1 = flag1;
	}
	UINT8 Key;
	int X;
	int Y;
	SEQUENCE_INPUT_FLAG Flag;
	std::uint32_t Flag1;
	std::uint64_t TimeStamp;

};
class AppMessages
{
public:
	RTU_INT2 ScreenSize;
	RTU_INT2 ScreenSizePrevious;

	std::uint64_t m_previousTick;
	std::uint64_t m_currentTick;
	double m_deltaTime;

	std::mutex lock_sequenceInputs;
	std::vector<AppSequenceInput>* L_sequenceInputs_w = &_sequenceInputs[0];
	std::vector<AppSequenceInput>* m_sequenceInputs_r = &_sequenceInputs[1];
	std::vector<AppSequenceInput> _sequenceInputs[2];

	std::atomic<bool> low_requireDestory;
};