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
using Microsoft::WRL::ComPtr;
class RTUDX12Mesh : public IRTUMesh
{
public:
	int GetIndexCount();
	RTU_STATES GetState();
	ComPtr<ID3D12Resource> m_vertex;
	ComPtr<ID3D12Resource> m_index;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
	RTU_STATES m_states = RTU_STATES::RTU_STATES_UNLOADED;
	uint64_t m_lastRefFrame;
};
