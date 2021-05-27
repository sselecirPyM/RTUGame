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
#include "RTUCBufferGroup.h"

void RTUCBufferGroup::Init(IRTUGraphicsFactory* graphicsFactory, int sliceSize, int bufferSize)
{
	m_graphicsFactory = graphicsFactory;
	m_sliceSize = sliceSize;
	m_bufferSize = bufferSize;
	m_tempBuffer.resize(bufferSize);
	m_slicePerBuffer = m_bufferSize / m_sliceSize;
	m_prevBufferIndex = 0;
	m_sizeD256 = sliceSize / 256;
#ifdef _DEBUG
	dbg_prevUpdateIndex = -1;
#endif
}

void RTUCBufferGroup::RequireSliceCount(IRTUGraphicsDevice* device, int count)
{
	int requireBufferCount = (count + m_slicePerBuffer - 1) / m_slicePerBuffer;
	while (m_buffers.size() < requireBufferCount)
	{
		m_buffers.push_back(std::unique_ptr< IRTUSBuffer>(m_graphicsFactory->GetSBuffer()));
		device->InitBuffer((m_buffers.end() - 1)->get(), m_bufferSize);
	}
}

void RTUCBufferGroup::UpdateSlice(IRTUGraphicsContext* graphicsContext, int index, void* data, int size)
{
#ifdef _DEBUG
	assert((int)index > dbg_prevUpdateIndex);
	dbg_prevUpdateIndex = index;
#endif
	int index1 = index % m_slicePerBuffer;
	int index2 = index / m_slicePerBuffer;
	if (index2 != m_prevBufferIndex)
	{
		graphicsContext->UpdateBuffer(m_buffers[m_prevBufferIndex].get(), m_tempBuffer.data(), m_bufferSize);
		m_prevBufferIndex = index2;
	}

	memcpy(m_tempBuffer.data() + index1 * m_sliceSize, data, size);
}

void* RTUCBufferGroup::GetSlicePtr(IRTUGraphicsContext* graphicsContext, int index)
{
#ifdef _DEBUG
	assert((int)index > dbg_prevUpdateIndex);
	dbg_prevUpdateIndex = index;
#endif
	int index1 = index % m_slicePerBuffer;
	int index2 = index / m_slicePerBuffer;
	if (index2 != m_prevBufferIndex)
	{
		graphicsContext->UpdateBuffer(m_buffers[m_prevBufferIndex].get(), m_tempBuffer.data(), m_bufferSize);
		m_prevBufferIndex = index2;
	}
	return m_tempBuffer.data() + index1 * m_sliceSize;
}

void RTUCBufferGroup::UpdateSliceComplete(IRTUGraphicsContext* graphicsContext)
{
	if (m_buffers.size() == 0)return;
	graphicsContext->UpdateBuffer(m_buffers[m_prevBufferIndex].get(),  m_tempBuffer.data(), m_bufferSize);
	m_prevBufferIndex = 0;
#ifdef _DEBUG
	dbg_prevUpdateIndex = -1;
#endif
}

void RTUCBufferGroup::SetCBVR(IRTUGraphicsContext* graphicsContext, int index, int slot)
{
	int index1 = index % m_slicePerBuffer;
	int index2 = index / m_slicePerBuffer;
	graphicsContext->SetCBVR(m_buffers[index2].get(), m_sizeD256 * index1, m_sizeD256, slot);
}
