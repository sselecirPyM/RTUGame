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
#include <DirectXMath.h>

inline DirectX::XMINT3 MathAligned(DirectX::XMINT3 pos, int alignment)
{
	int v1 = alignment - 1;
	return  DirectX::XMINT3((pos.x + v1) & ~v1, (pos.y + v1) & ~v1, (pos.z + v1) & ~v1);
}
inline DirectX::XMINT3 MathAlignedDown(DirectX::XMINT3 pos, int alignment)
{
	int v1 = alignment - 1;
	return  DirectX::XMINT3((pos.x) & ~v1, (pos.y) & ~v1, (pos.z) & ~v1);
}

inline glm::i32vec3 MathAligned(glm::i32vec3 pos, int alignment)
{
	int v1 = alignment - 1;
	return  glm::i32vec3((pos.x + v1) & ~v1, (pos.y + v1) & ~v1, (pos.z + v1) & ~v1);
}
inline glm::i32vec3 MathAlignedDown(glm::i32vec3 pos, int alignment)
{
	int v1 = alignment - 1;
	return  glm::i32vec3((pos.x) & ~v1, (pos.y) & ~v1, (pos.z) & ~v1);
}
inline DirectX::XMINT3 MathAdd(DirectX::XMINT3 x, DirectX::XMINT3 y)
{
	return  DirectX::XMINT3(x.x + y.x, x.y + y.y, x.z + y.z);
}
inline DirectX::XMINT3 MathSub(DirectX::XMINT3 x, DirectX::XMINT3 y)
{
	return  DirectX::XMINT3(x.x - y.x, x.y - y.y, x.z - y.z);
}
inline glm::i32vec3 MathAdd(glm::i32vec3 x, glm::i32vec3 y)
{
	return x + y;
	//return  glm::i32vec3(x.x + y.x, x.y + y.y, x.z + y.z);
}
inline glm::i32vec3 MathSub(glm::i32vec3 x, glm::i32vec3 y)
{
	return x - y;
	//return  glm::i32vec3(x.x - y.x, x.y - y.y, x.z - y.z);
}
inline bool INT3Equal(DirectX::XMINT3 x, DirectX::XMINT3 y)
{
	return (x.x == y.x && x.y == y.y && x.z == y.z);
}
inline bool FLOAT3Equal(DirectX::XMFLOAT3 x, DirectX::XMFLOAT3 y)
{
	return (x.x == y.x && x.y == y.y && x.z == y.z);
}