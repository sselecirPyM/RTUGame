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
#include "ChunkScene.h"
using namespace DirectX;

void ChunkScene::Add(ClientChunk* chunk, ClientChunk** popIfExist)
{
	auto& position = chunk->m_chunk.m_position;

	XMINT3 alignedMax = MathAlignedDown(position, c_chunkTreeRange);

	auto TopNode1 = m_chunkTree2.find(alignedMax);
	if(TopNode1!=m_chunkTree2.end())
	{
		ChunkNodeOP::Add(&TopNode1->second, chunk, popIfExist);
	}
	else
	{
		m_chunkTree2.insert(std::pair<XMINT3, ChunkNode_1>(alignedMax, ChunkNode_1(2)));
		TopNode1 = m_chunkTree2.find(alignedMax);
		TopNode1->second.m_position = alignedMax;
		ChunkNodeOP::Add(&TopNode1->second, chunk, popIfExist);
	}
}

//void ChunkScene::Cull(FrustumCull& frustumCull, XMINT3& relatePosition, std::vector<ClientChunk*>& result)
//{
//	for (auto it = m_chunkTree.begin(); it != m_chunkTree.end(); it++)
//	{
//		it->second.Cull(frustumCull, relatePosition, result);
//	}
//}

void ChunkScene::Cull(FrustumCull& frustumCull, XMINT3& relatePosition, std::vector<DrawContainer0>& result)
{
	for (auto it = m_chunkTree2.begin(); it != m_chunkTree2.end(); it++)
	{
		ChunkNodeOP::Cull(&it->second, frustumCull, relatePosition, result);
	}
}

bool ChunkScene::TryGetChunk(DirectX::XMINT3 position, ClientChunk** result)
{
	XMINT3 aligned2 = MathAlignedDown(position, c_chunkTreeRange);
	auto p2 = m_chunkTree2.find(aligned2);
	if (p2 != m_chunkTree2.end())
	{
		return ChunkNodeOP::TryGetChunk(&p2->second,position, result);
	}
	else
		return false;
}

bool ChunkScene::TryGetChunk(XMINT3 position, ChunkNode_Bottom** result)
{
	XMINT3 aligned2 = MathAlignedDown(position, c_chunkTreeRange);
	auto p2 = m_chunkTree2.find(aligned2);
	if (p2 != m_chunkTree2.end())
	{
		ChunkNode_Bottom* chunk = nullptr;
		if (!ChunkNodeOP::TryGetChunk(&p2->second, position, &chunk))return false;
		XMINT3 index = MathSub(position, MathAlignedDown(position, 16));
		*result = chunk;
		return result;
	}
	else
		return false;
}

bool ChunkScene::TryGetBlock(DirectX::XMINT3 position, std::uint32_t* result)
{
	XMINT3 aligned2 = MathAlignedDown(position, c_chunkTreeRange);
	auto p2 = m_chunkTree2.find(aligned2);
	if (p2 != m_chunkTree2.end())
	{
		ClientChunk* chunk = nullptr;
		if (!ChunkNodeOP::TryGetChunk(&p2->second,position, &chunk))return false;
		XMINT3 index = MathSub(position, MathAlignedDown(position, 16));
		*result = chunk->m_chunk.m_data[index.z][index.y][index.x];
		return result;
	}
	else
		return false;
}

bool ChunkScene::CastRay(RTU::Ray ray, XMINT3 relatePosition, XMINT3* hitResult, XMINT3* surfaceHit)
{
	if (FLOAT3Equal(ray.StartPoint, ray.EndPoint))return false;
	XMINT3 currentPosition;
	XMStoreSInt3(&currentPosition, XMVectorFloor(XMLoadFloat3(&ray.StartPoint)));
	currentPosition = MathAlignedDown(MathAdd(currentPosition, relatePosition), 16);

	XMVECTOR p1 = XMLoadFloat3(&ray.StartPoint);
	XMVECTOR p2 = XMLoadFloat3(&ray.EndPoint);
	XMVECTOR dir = XMVector3Normalize(p2 - p1);
	XMFLOAT3 _next;
	XMStoreFloat3(&_next, dir);
	XMINT3 next1 =
	{
		(_next.x < 0) ? -1 : 1,
		(_next.y < 0) ? -1 : 1,
		(_next.z < 0) ? -1 : 1,
	};

	XMVECTOR blockTests[] =
	{
		XMVECTORF32{1.0f,0.0f,0.0f,(_next.x > 0) ? -16.0f : 0.0f},
		XMVECTORF32{0.0f,1.0f,0.0f,(_next.y > 0) ? -16.0f : 0.0f},
		XMVECTORF32{0.0f,0.0f,1.0f,(_next.z > 0) ? -16.0f : 0.0f},
	};
	XMINT3 vecAdd[] =
	{
		XMINT3(next1.x * 16,0,0),
		XMINT3(0,next1.y * 16,0),
		XMINT3(0,0,next1.z * 16),
	};

	XMVECTOR pos1 = XMVECTORF32{ -1e-5,-1e-5,-1e-5 };
	XMVECTOR pos2 = XMVECTORF32{ 16 + 1e-4, 16 + 1e-4, 16 + 1e-4 };
	while (true)
	{
		ClientChunk* chunk = nullptr;
		if (TryGetChunk(currentPosition, &chunk) && chunk->CastRay(ray, relatePosition, hitResult, surfaceHit))
		{
			return true;
		}
		else
		{
			auto _1 = MathSub(currentPosition, relatePosition);
			XMVECTOR s1 = XMLoadSInt3(&_1);
			XMVECTOR p1 = XMLoadFloat3(&ray.StartPoint) - s1;
			XMVECTOR p2 = XMLoadFloat3(&ray.EndPoint) - s1;

			XMVECTOR closestHit;

			for (int i = 0; i < 3; i++)
			{
				closestHit = XMPlaneIntersectLine(blockTests[i], p1, p2);
				if (XMVector3Equal(closestHit, closestHit) && XMVector3LessOrEqual(closestHit, pos2) && XMVector3GreaterOrEqual(closestHit, pos1))
				{
					if (XMVectorGetX(XMVector3Length(closestHit - p1)) > ray.length)return false;
					currentPosition = MathAdd(currentPosition, vecAdd[i]);
					break;
				}
			}
		}
	}
}
