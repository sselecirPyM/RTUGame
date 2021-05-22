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
#include "ClientChunk.h"
#include "../Math/FrustumCull.h"
#include "../Math/MathCommon.h"
#include "../Math/Ray.h"
#include "../RenderPipeline/Containers.h"
#include "../Physics/RTUPhysicsInterfaces.h"
#include "ChunkNode.h"
using namespace DirectX;
class ChunkScene
{
public:
	std::map<glm::i32vec3, ChunkNode_1, XMINT3Comparer> m_chunkTree2;
	void Add(ClientChunk* chunk, ClientChunk** popIfExist);
	//void Cull(FrustumCull& frustumCull, XMINT3& relatePosition, std::vector<ClientChunk*>& result);
	void Cull(FrustumCull& frustumCull, glm::i32vec3& relatePosition, std::vector<DrawContainer0>& result);
	bool TryGetChunk(glm::i32vec3 position, ClientChunk** result);
	bool TryGetChunk(glm::i32vec3 position, ChunkNode_Bottom** result);
	bool TryGetBlock(glm::i32vec3 position, std::uint32_t* result);
	bool CastRay(RTU::Ray ray, glm::i32vec3 relatePosition, glm::i32vec3* hitResult, glm::i32vec3* surfaceHit);
};

