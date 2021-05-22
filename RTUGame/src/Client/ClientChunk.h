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
#include "../Graphics/RTUGraphicsInterfaces.h"
#include "../Graphics/RTUMeshLoader.h"
#include "../Math/Ray.h"
class ClientChunk
{
public:
	std::vector<char> m_vertexCache;
	std::vector<char> m_indexCache;
	int m_indexCount = 0;
	Chunk16 m_chunk;
	bool CastRay(RTU::Ray ray, glm::i32vec3 relatePosition, glm::i32vec3* hitResult, glm::i32vec3* surfaceHit);
	bool TryGetBlock(glm::i32vec3 position, std::uint32_t* result);
};

