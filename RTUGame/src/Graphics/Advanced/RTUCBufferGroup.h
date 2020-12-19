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
#include "../RTUGraphicsInterfaces.h"
class RTUCBufferGroup
{
public:
	void Init(IRTUGraphicsFactory* graphicsFactory, int sliceSize, int bufferSize);
	void RequireSliceCount(IRTUGraphicsDevice* device, int count);
	void UpdateSlice(IRTUGraphicsContext* graphicsContext, int syncIndex, int index, void* data, int size);
	void* GetSlicePtr(IRTUGraphicsContext* graphicsContext, int syncIndex, int index);
	void UpdateSliceComplete(IRTUGraphicsContext* graphicsContext, int syncIndex);
	void SetCBVR(IRTUGraphicsContext* graphicsContext, int syncIndex, int index, int slot);
	int m_sliceSize;
	int m_bufferSize;
	std::vector<std::unique_ptr<IRTUSBuffer>> m_buffers;
	std::vector<char> m_tempBuffer;
	int m_prevBufferIndex;
	int m_slicePerBuffer;
	int m_sizeD256;
	IRTUGraphicsFactory* m_graphicsFactory;
#ifdef _DEBUG
	int dbg_prevUpdateIndex;
#endif
};

