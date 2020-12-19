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
#include "RTUMeshLoader.h"
#include <glm/glm.hpp>
using namespace DirectX;
void RTUMeshLoader::Quad()
{
	glm::vec3 positions[]
	{
		glm::vec3(-1,-1,0),
		glm::vec3(-1, 1,0),
		glm::vec3(1,-1,0),
		glm::vec3(1, 1,0),
	};
	unsigned int indices[] =
	{
		0, 1, 2,
		2, 1, 3,
	};
	m_vertexStride = 12;
	m_vertexData.resize(sizeof(positions));
	m_indexData.resize(sizeof(indices));
	memcpy(m_vertexData.data(), positions, sizeof(positions));
	memcpy(m_indexData.data(), indices, sizeof(indices));
}

void RTUMeshLoader::Cube()
{
	glm::vec3 positions[]
	{
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,0.5f),
	};
	unsigned int indices[] =
	{
		0, 1, 2,
		1, 3, 2,

		4, 6, 5,
		5, 6, 7,

		0, 5, 1,
		0, 4, 5,

		2, 7, 6,
		2, 3, 7,

		0, 6, 4,
		0, 2, 6,

		1, 7, 3,
		1, 5, 7,
	};
	m_vertexStride = 12;
	m_vertexData.resize(sizeof(positions));
	m_indexData.resize(sizeof(indices));
	memcpy(m_vertexData.data(), positions, sizeof(positions));
	memcpy(m_indexData.data(), indices, sizeof(indices));
}

void RTUMeshLoader::CubeWire()
{
	glm::vec3 positions[]
	{
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,0.5f),
	};
	unsigned int indices[] =
	{
		0, 1,
		2, 3,
		4, 5,
		6, 7,

		0, 2,
		1, 3,
		4, 6,
		5, 7,

		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};
	m_vertexStride = sizeof(glm::vec3);
	m_vertexData.resize(sizeof(positions));
	m_indexData.resize(sizeof(indices));
	memcpy(m_vertexData.data(), positions, sizeof(positions));
	memcpy(m_indexData.data(), indices, sizeof(indices));
}
struct PosNormUVTan
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
	glm::vec3 Tangent;
};
void RTUMeshLoader::Cube1()
{
	PosNormUVTan positions[]
	{
		{glm::vec3(0.0f,0.0f,0.0f),glm::vec3(-1,0,0),glm::vec2(1,1)},
		{glm::vec3(0.0f,0.0f,1.0f),glm::vec3(-1,0,0),glm::vec2(0,1)},
		{glm::vec3(0.0f,1.0f,0.0f),glm::vec3(-1,0,0),glm::vec2(1,0)},
		{glm::vec3(0.0f,1.0f,1.0f),glm::vec3(-1,0,0),glm::vec2(0,0)},
		{glm::vec3(1.0f,0.0f,0.0f),glm::vec3(1,0,0),glm::vec2(0,1)},
		{glm::vec3(1.0f,0.0f,1.0f),glm::vec3(1,0,0),glm::vec2(1,1)},
		{glm::vec3(1.0f,1.0f,0.0f),glm::vec3(1,0,0),glm::vec2(0,0)},
		{glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1,0,0),glm::vec2(1,0)},

		{glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0,-1,0),glm::vec2(0,0)},
		{glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0,-1,0),glm::vec2(0,1)},
		{glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0,1,0),glm::vec2(0,1)},
		{glm::vec3(0.0f,1.0f,1.0f),glm::vec3(0,1,0),glm::vec2(0,0)},
		{glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0,-1,0),glm::vec2(1,0)},
		{glm::vec3(1.0f,0.0f,1.0f),glm::vec3(0,-1,0),glm::vec2(1,1)},
		{glm::vec3(1.0f,1.0f,0.0f),glm::vec3(0,1,0),glm::vec2(1,1)},
		{glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0,1,0),glm::vec2(1,0)},

		{glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0,0,-1),glm::vec2(0,1)},
		{glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0,0,1),glm::vec2(1,1)},
		{glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0,0,-1),glm::vec2(0,0)},
		{glm::vec3(0.0f,1.0f,1.0f),glm::vec3(0,0,1),glm::vec2(1,0)},
		{glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0,0,-1),glm::vec2(1,1)},
		{glm::vec3(1.0f,0.0f,1.0f),glm::vec3(0,0,1),glm::vec2(0,1)},
		{glm::vec3(1.0f,1.0f,0.0f),glm::vec3(0,0,-1),glm::vec2(1,0)},
		{glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0,0,1),glm::vec2(0,0)},
	};
	unsigned int indices[] =
	{
		0, 1, 2,
		1, 3, 2,

		4, 6, 5,
		5, 6, 7,

		8, 13, 9,
		8, 12, 13,

		10, 15, 14,
		10, 11, 15,

		16, 22, 20,
		16, 18, 22,

		17, 23, 19,
		17, 21, 23,
	};
	m_vertexStride = sizeof(PosNormUVTan);
	m_vertexData.resize(sizeof(positions));
	m_indexData.resize(sizeof(indices));
	memcpy(m_vertexData.data(), positions, sizeof(positions));
	memcpy(m_indexData.data(), indices, sizeof(indices));
}
