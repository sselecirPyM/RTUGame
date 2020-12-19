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
#include "ChunkNode.h"

ChunkNode_1::ChunkNode_1(int level)
{
	m_level = level;
}

void ChunkNodeOP::Add(IChunkNode* chunkNode, ClientChunk* chunk, ClientChunk** popIfExist)
{
	while (true)
	{
		ChunkNode_1* nd1 = dynamic_cast<ChunkNode_1*>(chunkNode);
		ChunkNode_Bottom* ndb = dynamic_cast<ChunkNode_Bottom*>(chunkNode);
		if (nd1 != nullptr)
		{
			XMINT3 aligned = MathAlignedDown(chunk->m_chunk.m_position, Chunk16::c_size * (4 << nd1->m_level));
			XMINT3 position = chunk->m_chunk.m_position;
			int subChunkSize = (Chunk16::c_size * (2 << nd1->m_level));
			int x1 = (position.x - aligned.x) / subChunkSize;
			int y1 = (position.y - aligned.y) / subChunkSize;
			int z1 = (position.z - aligned.z) / subChunkSize;

			if (nd1->m_children[z1][y1][x1] == nullptr)
			{
				nd1->m_childCount += 1;
				if (nd1->m_level > 1)
				{
					ChunkNode_1* nd2 = new ChunkNode_1(nd1->m_level - 1);
					nd1->m_children[z1][y1][x1] = std::unique_ptr<ChunkNode_1>(nd2);
					nd2->m_position = MathAdd(XMINT3(x1 * subChunkSize, y1 * subChunkSize, z1 * subChunkSize), aligned);
					chunkNode = nd2;
				}
				else
				{
					ChunkNode_Bottom* nd2 = new ChunkNode_Bottom();
					nd1->m_children[z1][y1][x1] = std::unique_ptr<ChunkNode_Bottom>(nd2);
					nd2->m_position = MathAdd(XMINT3(x1 * subChunkSize, y1 * subChunkSize, z1 * subChunkSize), aligned);
					chunkNode = nd2;
				}
			}
			else
			{
				chunkNode = nd1->m_children[z1][y1][x1].get();
			}
		}
		else if (ndb != nullptr)
		{
			XMINT3 aligned = MathAlignedDown(chunk->m_chunk.m_position, Chunk16::c_size * 4);
			XMINT3 position = chunk->m_chunk.m_position;
			int x1 = (position.x - aligned.x) / Chunk16::c_size;
			int y1 = (position.y - aligned.y) / Chunk16::c_size;
			int z1 = (position.z - aligned.z) / Chunk16::c_size;


			if (ndb->m_children[z1][y1][x1] == nullptr)
			{
				ndb->m_childCount += 1;
				ndb->m_children[z1][y1][x1] = chunk;
			}
			else
			{
				*popIfExist = ndb->m_children[z1][y1][x1];
				ndb->m_children[z1][y1][x1] = chunk;
			}
			break;
		}
		else
		{
			throw std::exception("chunk node error.a2");
		}
	}
}

void ChunkNodeOP::Cull(IChunkNode* chunkNode, FrustumCull& frustumCull, XMINT3& relatePosition, std::vector<DrawContainer0>& result)
{
	ChunkNode_1* nd1 = dynamic_cast<ChunkNode_1*>(chunkNode);
	ChunkNode_Bottom* ndb = dynamic_cast<ChunkNode_Bottom*>(chunkNode);
	if (nd1 != nullptr)
	{
		auto _1 = MathSub(nd1->m_position, relatePosition);
		int level = nd1->m_level;
		auto pos0 = XMLoadSInt3(&_1) + XMVECTORF32{ (1 << level) * 32.0f,(1 << level) * 32.0f,(1 << level) * 32.0f };
		float t = (1 << level) * 32;
		if (!frustumCull.BoxIntersects(pos0, XMFLOAT3(t, t, t)))
			return;
		if (frustumCull.BoxContains(pos0, XMFLOAT3(t, t, t)))
		{
			AllChild(nd1, relatePosition, result);
			return;
		}
		for (int i = 0; i < 8; i++)
			if (nd1->m_children[0][0][i] != nullptr)
				Cull(nd1->m_children[0][0][i].get(), frustumCull, relatePosition, result);
	}
	else if (ndb != nullptr)
	{
		if (!ndb->m_mergedMesh)return;
		XMINT3 a1 = MathSub(ndb->m_position, relatePosition);
		auto pos0 = XMLoadSInt3(&a1) + XMVECTORF32{ 32, 32, 32 };
		if (!frustumCull.BoxIntersects(pos0, XMFLOAT3(32, 32, 32)))
			return;
		if (frustumCull.BoxContains(pos0, XMFLOAT3(32, 32, 32)))
		{
			AllChild(ndb, relatePosition, result);
			return;
		}
		int startIndex = 0;
		int indexCount = 0;
		auto mesh = DrawContainer0(ndb->m_mergedMesh.get(), XMFLOAT4X4());
		XMStoreFloat4x4(&mesh.m_toWorldMatrix, XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadSInt3(&a1))));
		for (int z = 0; z < 4; z++)
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					auto& child = ndb->m_children[z][y][x];
					if (child != nullptr && ndb->m_startIndexCount[z][y][x] > 0)
					{
						auto _1 = MathAdd(XMINT3(x * 16, y * 16, z * 16), a1);
						auto pos1 = XMLoadSInt3(&_1) + XMVECTORF32{ 8,8,8 };
						if (frustumCull.BoxIntersects(pos1, XMFLOAT3(8, 8, 8)))
						{
							indexCount += ndb->m_startIndexCount[z][y][x];
							mesh.m_startIndex = startIndex;
							mesh.m_indexCount = indexCount;
						}
						else
						{
							if (indexCount)
								result.emplace_back(mesh);
							startIndex += indexCount + ndb->m_startIndexCount[z][y][x];
							indexCount = 0;
						}
					}
				}
		if (indexCount)
			result.emplace_back(mesh);
	}
	else
	{
		throw std::exception("chunk node error.a1");
	}
}

void ChunkNodeOP::AllChild(IChunkNode* chunkNode, std::vector<ClientChunk*>& result)
{
	ChunkNode_1* nd1 = dynamic_cast<ChunkNode_1*>(chunkNode);
	ChunkNode_Bottom* ndb = dynamic_cast<ChunkNode_Bottom*>(chunkNode);
	if (nd1 != nullptr)
	{
		for (int i = 0; i < 8; i++)
			if (nd1->m_children[0][0][i] != nullptr)
				AllChild(nd1->m_children[0][0][i].get(), result);
	}
	else if (ndb != nullptr)
	{
		for (int i = 0; i < 64; i++)
			if (ndb->m_children[0][0][i] != nullptr)
				result.emplace_back(ndb->m_children[0][0][i]);
	}
	else
	{
		throw std::exception("chunk node error.a1");
	}
}

void ChunkNodeOP::AllChild(IChunkNode* chunkNode, XMINT3& relatePosition, std::vector<DrawContainer0>& result)
{
	ChunkNode_1* nd1 = dynamic_cast<ChunkNode_1*>(chunkNode);
	ChunkNode_Bottom* ndb = dynamic_cast<ChunkNode_Bottom*>(chunkNode);

	if (nd1 != nullptr)
	{
		for (int i = 0; i < 8; i++)
			if (nd1->m_children[0][0][i] != nullptr)
				AllChild(nd1->m_children[0][0][i].get(), relatePosition, result);
	}
	else if (ndb != nullptr)
	{
		XMINT3 a1 = MathSub(ndb->m_position, relatePosition);
		if (ndb->m_mergedMesh)
		{
			auto mesh = DrawContainer0(ndb->m_mergedMesh.get(), XMFLOAT4X4());
			XMStoreFloat4x4(&mesh.m_toWorldMatrix, XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadSInt3(&a1))));
			mesh.m_startIndex = 0;
			mesh.m_indexCount = ndb->m_meshIndexCount;
			result.emplace_back(mesh);
		}
	}
	else
	{
		throw std::exception("chunk node error.a1");
	}
}

bool ChunkNodeOP::TryGetChunk(IChunkNode* chunkNode, XMINT3 position, ChunkNode_Bottom** result)
{
	while (true)
	{
		ChunkNode_1* nd1 = dynamic_cast<ChunkNode_1*>(chunkNode);
		ChunkNode_Bottom* ndb = dynamic_cast<ChunkNode_Bottom*>(chunkNode);
		if (nd1 != nullptr)
		{
			XMINT3 aligned = MathAlignedDown(position, Chunk16::c_size * (4 << nd1->m_level));
			int subChunkSize = (Chunk16::c_size * (2 << nd1->m_level));
			int x1 = (position.x - aligned.x) / subChunkSize;
			int y1 = (position.y - aligned.y) / subChunkSize;
			int z1 = (position.z - aligned.z) / subChunkSize;

			if (nd1->m_children[z1][y1][x1] == nullptr)
			{
				return false;
			}
			else
			{
				chunkNode = nd1->m_children[z1][y1][x1].get();
			}
		}
		else if (ndb != nullptr)
		{
			*result = ndb;
			return true;
		}
		else
		{
			throw std::exception("chunk node error.a1");
		}
	}
}

bool ChunkNodeOP::TryGetChunk(IChunkNode* chunkNode, XMINT3 position, ClientChunk** result)
{
	ChunkNode_Bottom* chunk;
	if (!TryGetChunk(chunkNode, position, &chunk))return false;

	XMINT3 aligned = MathAlignedDown(position, Chunk16::c_size * 4);
	int x1 = (position.x - aligned.x) / Chunk16::c_size;
	int y1 = (position.y - aligned.y) / Chunk16::c_size;
	int z1 = (position.z - aligned.z) / Chunk16::c_size;


	if (chunk->m_children[z1][y1][x1] != nullptr)
	{
		*result = chunk->m_children[z1][y1][x1];
		return true;
	}
	return false;
}
