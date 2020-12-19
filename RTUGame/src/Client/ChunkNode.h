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
#include "../RenderPipeline/Containers.h"
#include "../Physics/RTUPhysicsInterfaces.h"
using namespace DirectX;

const int c_chunkTreeLevel = 2;
const int c_chunkTreeRange = 64 * (1 << c_chunkTreeLevel);

class IChunkNode
{
public:
	XMINT3 m_position;
	virtual ~IChunkNode() {}
};

class ChunkNode_1 : public IChunkNode
{
public:
	ChunkNode_1(int level);
	int m_level = 0;
	uint32_t m_childCount = 0;
	std::unique_ptr<IChunkNode> m_children[2][2][2] = {};
};
class ChunkNode_Bottom : public IChunkNode
{
public:
	uint32_t m_childCount = 0;
	std::unique_ptr<IRTUMesh> m_mergedMesh;
	std::unique_ptr<IRTUMesh> m_mergedMesh1;
	int m_meshIndexCount = 0;
	int m_meshIndexCount1 = 0;
	int m_startIndexCount[4][4][4];
	std::unique_ptr<IRTUPhysicsStatic> m_collision;
	std::unique_ptr<IRTUPhysicsStatic> m_collision1;
	ClientChunk* m_children[4][4][4] = {};
};

class ChunkNodeOP
{
public:
	static void Add(IChunkNode* chunkNode, ClientChunk* chunk, ClientChunk** popIfExist);
	static void Cull(IChunkNode* chunkNode, FrustumCull& frustumCull, XMINT3& relatePosition, std::vector<DrawContainer0>& result);
	static void AllChild(IChunkNode* chunkNode, std::vector<ClientChunk*>& result);
	static void AllChild(IChunkNode* chunkNode, XMINT3& relatePosition, std::vector<DrawContainer0>& result);
	static bool TryGetChunk(IChunkNode* chunkNode, XMINT3 position, ChunkNode_Bottom** result);
	static bool TryGetChunk(IChunkNode* chunkNode, XMINT3 position, ClientChunk** result);
};
