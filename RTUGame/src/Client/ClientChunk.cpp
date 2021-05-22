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
#include "ClientChunk.h"
#include "../Math/MathCommon.h"
#include "../Compression/xxhash.h"
using namespace DirectX;

struct PosNormUVTan
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
	XMFLOAT3 Tangent;
};

bool ClientChunk::CastRay(RTU::Ray ray, glm::i32vec3 relatePosition, glm::i32vec3* hitResult, glm::i32vec3* surfaceHit)
{
	if (FLOAT3Equal(ray.StartPoint, ray.EndPoint))return false;
	auto _1 = MathSub(m_chunk.m_position, relatePosition);
	XMVECTOR s1 = XMLoadSInt3((XMINT3*)&_1);

	XMVECTOR p1 = XMLoadFloat3(&ray.StartPoint) - s1;
	XMVECTOR p2 = XMLoadFloat3(&ray.EndPoint) - s1;
	XMVECTOR dir = XMVector3Normalize(p2 - p1);

	if (XMVectorGetX(XMVector3LinePointDistance(p1, p2, XMVECTORF32{ 8,8,8 })) > 13.86 /*8*sqrt3*/)return false;

	XMVECTOR pos1 = XMVECTORF32{ 0, 0, 0 } + XMVECTORF32{ -1e-5,-1e-5,-1e-5 };
	XMVECTOR pos2 = XMVECTORF32{ 16, 16, 16 } + XMVECTORF32{ 1e-5,1e-5,1e-5 };

	XMFLOAT3 sign1;
	XMStoreFloat3(&sign1, dir);
	glm::i32vec3 sign2 = { (sign1.x < 0) ? -1 : 1,(sign1.y < 0) ? -1 : 1,(sign1.z < 0) ? -1 : 1, };

	XMVECTOR closestHit;
	glm::i32vec3 scanPos;
	if (!(XMVector3LessOrEqual(p1, pos2) && XMVector3GreaterOrEqual(p1, pos1)))
	{
		XMVECTOR planes[] =
		{
			XMVECTORF32{-1, 0, 0, 0},
			XMVECTORF32{0, -1, 0, 0},
			XMVECTORF32{0, 0, -1, 0},
			XMVECTORF32{1, 0, 0, -16},
			XMVECTORF32{0, 1, 0, -16},
			XMVECTORF32{0, 0, 1, -16},
		};
		int face = -1;
		for (int i = 0; i < 6; i++)
		{
			if (XMVectorGetX(XMVector3Dot(planes[i], dir)) > 0)continue;
			closestHit = XMPlaneIntersectLine(planes[i], p1, p2);
			if (!XMVector3Equal(closestHit, closestHit))continue;
			if (XMVectorGetX(XMVector3Dot(closestHit - p1, dir)) > 0 && XMVector3LessOrEqual(closestHit, pos2) && XMVector3GreaterOrEqual(closestHit, pos1))
			{
				face = i;
				break;
			}
		}
		if (face == -1)
			return false;

		pos1 = XMVectorClamp(XMVectorFloor(closestHit), XMVECTORF32{}, XMVECTORF32{ 15,15,15 });
		XMStoreSInt3((XMINT3*)&scanPos, pos1);
		if (m_chunk.m_data[scanPos.z][scanPos.y][scanPos.x] & ray.filterMask)
		{
			*hitResult = MathAdd(scanPos, m_chunk.m_position);
			switch (face)
			{
			case 0:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(-1, 0, 0));
				break;
			case 1:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(0, -1, 0));
				break;
			case 2:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(0, 0, -1));
				break;
			case 3:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(1, 0, 0));
				break;
			case 4:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(0, 1, 0));
				break;
			case 5:
				*surfaceHit = MathAdd(*hitResult, glm::i32vec3(0, 0, 1));
				break;
			}

			return true;
		}
	}
	else
	{
		pos1 = XMVectorClamp(XMVectorFloor(p1), XMVECTORF32{}, XMVECTORF32{ 15,15,15 });
		XMStoreSInt3((XMINT3*)&scanPos, pos1);
	}


	XMVECTOR blockTest1 = XMVECTORF32{ (sign1.x > 0) ? 1.0f : 0.0f,(sign1.y > 0) ? 1.0f : 0.0f, (sign1.z > 0) ? 1.0f : 0.0f };
	XMVECTOR vecAdd[] =
	{
		XMVECTORF32{static_cast<float>(sign2.x),0,0},
		XMVECTORF32{0,static_cast<float>(sign2.y),0},
		XMVECTORF32{0,0,static_cast<float>(sign2.z)},
	};
	XMVECTOR scanBlockPos = pos1;
	do
	{
		pos1 = scanBlockPos - XMVECTORF32{ 1e-5,1e-5,1e-5 };
		pos2 = scanBlockPos + XMVECTORF32{ 1 + 1e-5 ,1 + 1e-5 ,1 + 1e-5 };
		for (int i = 0; i < 3; i++)
		{
			closestHit = XMPlaneIntersectLine(XMPlaneFromPointNormal(scanBlockPos + blockTest1, vecAdd[i]), p1, p2);
			if (XMVector3Equal(closestHit, closestHit) && XMVector3LessOrEqual(closestHit, pos2) && XMVector3GreaterOrEqual(closestHit, pos1))
			{
				((int*)&scanPos)[i] += ((int*)&sign2)[i];
				if (((int*)&scanPos)[i] >= 16 || ((int*)&scanPos)[i] < 0)return false;
				if (m_chunk.m_data[scanPos.z][scanPos.y][scanPos.x] & ray.filterMask)
				{
					*hitResult = MathAdd(scanPos, m_chunk.m_position);
					*surfaceHit = *hitResult;
					((int*)surfaceHit)[i] -= ((int*)&sign2)[i];
					return true;
				}
				scanBlockPos += vecAdd[i];
				goto label_t0;
			}
		}
		return false;
	label_t0:
		continue;
	} while (true);


	return false;
}

bool ClientChunk::TryGetBlock(glm::i32vec3 position, std::uint32_t* result)
{
	glm::i32vec3 a = MathSub(position, m_chunk.m_position);
	if (a.x < 0 || a.y < 0 || a.z < 0 || a.x>15 || a.y>15 || a.z>15)
		return false;
	*result = m_chunk.m_data[a.z][a.y][a.x];
	return true;
}
