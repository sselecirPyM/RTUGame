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
#include "ClientContext.h"
#include "../Compression/xxhash.h"
#include "../Physics/PhysX/RTUPXFactory.h"
using namespace DirectX;

void ClientContext::Init()
{
	m_physicsFactory = std::unique_ptr<IRTUPhysicsFactory>(new RTUPXFactory);
	m_physicsDevice = std::unique_ptr<IRTUPhysicsDevice>(m_physicsFactory->GetDevice());
	m_physicsScene = std::unique_ptr<IRTUPhysicsScene>(m_physicsFactory->GetScene());
	m_physicsController = std::unique_ptr<IRTUPhysicsController>(m_physicsFactory->GetController());

	m_physicsDevice->Init();
	m_physicsDevice->InitScene(m_physicsScene.get());
	m_physicsDevice->SceneAddController(m_physicsScene.get(), m_physicsController.get(), glm::vec3(0, 40, 0), glm::vec4(0, 0, 0, 1));
}

struct PosNormUVTan
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
	XMFLOAT3 Tangent;
};

char _mod3Table[256]
{
	0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
	0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
	0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
	0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,
	0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
};
const float uvTest = 1 / 8.0f;
PosNormUVTan verticeLeft[]
{
	{XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(-1,0,0),XMFLOAT2(uvTest,uvTest)},
	{XMFLOAT3(0.0f,0.0f,1.0f),XMFLOAT3(-1,0,0),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT3(-1,0,0),XMFLOAT2(uvTest,0)},
	{XMFLOAT3(0.0f,1.0f,1.0f),XMFLOAT3(-1,0,0),XMFLOAT2(0,0)},
};

PosNormUVTan verticeRight[]
{
	{XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT3(1,0,0),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(1.0f,1.0f,0.0f),XMFLOAT3(1,0,0),XMFLOAT2(0,0)},
	{XMFLOAT3(1.0f,0.0f,1.0f),XMFLOAT3(1,0,0),XMFLOAT2(uvTest,uvTest)},
	{XMFLOAT3(1.0f,1.0f,1.0f),XMFLOAT3(1,0,0),XMFLOAT2(uvTest,0)},
};

PosNormUVTan verticeBottom[]
{
	{XMFLOAT3(0.0f,0.0f,1.0f),XMFLOAT3(0,-1,0),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(0,-1,0),XMFLOAT2(0,0)},
	{XMFLOAT3(1.0f,0.0f,1.0f),XMFLOAT3(0,-1,0),XMFLOAT2(uvTest,uvTest)},
	{XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT3(0,-1,0),XMFLOAT2(uvTest,0)},
};

PosNormUVTan verticeTop[]
{
	{XMFLOAT3(0.0f,1.0f,1.0f),XMFLOAT3(0,1,0),XMFLOAT2(0,0)},
	{XMFLOAT3(1.0f,1.0f,1.0f),XMFLOAT3(0,1,0),XMFLOAT2(uvTest,0)},
	{XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT3(0,1,0),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(1.0f,1.0f,0.0f),XMFLOAT3(0,1,0),XMFLOAT2(uvTest,uvTest)},
};

PosNormUVTan verticeBack[]
{
	{XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(0,0,-1),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT3(0,0,-1),XMFLOAT2(0,0)},
	{XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT3(0,0,-1),XMFLOAT2(uvTest,uvTest)},
	{XMFLOAT3(1.0f,1.0f,0.0f),XMFLOAT3(0,0,-1),XMFLOAT2(uvTest,0)},
};

PosNormUVTan verticeFront[]
{
	{XMFLOAT3(1.0f,0.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(0,uvTest)},
	{XMFLOAT3(1.0f,1.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(0,0)},
	{XMFLOAT3(0.0f,0.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(uvTest,uvTest)},
	{XMFLOAT3(0.0f,1.0f,1.0f),XMFLOAT3(0,0,1),XMFLOAT2(uvTest,0)},
};

struct _FaceData
{
	_FaceData(char*& _pFillVertex, char*& _pFillIndex, size_t _verticeSize, size_t _vertexCount) :
		pFillVertex(_pFillVertex),
		pFillIndex(_pFillIndex),
		verticeSize(_verticeSize),
		vertexCount(_vertexCount)
	{

	}
	char*& pFillVertex;
	char*& pFillIndex;
	size_t verticeSize;
	size_t vertexCount;
	size_t indexSize = 24;
	size_t indexCount = 6;
	size_t vertexTotal = 0;
	ClientChunk* pChunkNearby[6] = {};
	PosNormUVTan* pVertexFaces[6] = {};
	float dRowColumnCount = 0;
	uint32_t* faceIndex = nullptr;
	int hashSeed = 0;
	int hashSeed2 = 0;
};
inline void _Face1(_FaceData& faceData, PosNormUVTan* faceVertex, uint32_t* faceIndex,
	int x, int y, int z,
	float uvX, float uvY)
{
	memcpy(faceData.pFillVertex, faceVertex, faceData.verticeSize);
	PosNormUVTan* verticeCopyed = (PosNormUVTan*)(faceData.pFillVertex);
	for (size_t i = 0; i < faceData.vertexCount; i++)
	{
		verticeCopyed[i].Position.x += x;
		verticeCopyed[i].Position.y += y;
		verticeCopyed[i].Position.z += z;
		verticeCopyed[i].UV.x += uvX;
		verticeCopyed[i].UV.y += uvY;
	}
	memcpy(faceData.pFillIndex, faceIndex, faceData.indexSize);
	for (size_t i = 0; i < faceData.indexCount; i++)
	{
		faceIndex[i] += faceData.vertexCount;
	}
	faceData.pFillIndex += faceData.indexSize;
	faceData.pFillVertex += faceData.verticeSize;
}
inline void _unknowFun(ClientContext* _context, _FaceData faceData, ClientChunk* pChunk, int x, int y, int z)
{
	assert(x >= 0 && x < 16);
	assert(y >= 0 && y < 16);
	assert(z >= 0 && z < 16);
	auto& pChunkNearby = faceData.pChunkNearby;
	auto& pVertexFaces = faceData.pVertexFaces;
	auto& dRowColumnCount = faceData.dRowColumnCount;
	auto& faceIndex = faceData.faceIndex;

	Chunk16& m_chunk = pChunk->m_chunk;
	XMINT3 posWorld(m_chunk.m_position.x + x, m_chunk.m_position.y + y, m_chunk.m_position.z + z);
	uint32_t hash1 = XXH32(&posWorld, sizeof(posWorld), faceData.hashSeed);
	uint32_t hash2 = XXH32(&posWorld, sizeof(posWorld), faceData.hashSeed2);
	const size_t c_m1 = Chunk16::c_size - 1;
	if ((x > 0 && m_chunk.m_data[z][y][x - 1] == 0) ||
		(x == 0 && !pChunkNearby[0]) ||
		(x == 0 && pChunkNearby[0]->m_chunk.m_data[z][y][Chunk16::c_size - 1] == 0))
	{
		_Face1(faceData, pVertexFaces[0], faceIndex, x, y, z, (_mod3Table[(hash1 & 0xff)] + 3) * dRowColumnCount, 0);
	}
	if ((x < c_m1 && m_chunk.m_data[z][y][x + 1] == 0) ||
		(x == c_m1 && !pChunkNearby[1]) ||
		(x == c_m1 && pChunkNearby[1]->m_chunk.m_data[z][y][0] == 0))
	{
		_Face1(faceData, pVertexFaces[1], faceIndex, x, y, z, (_mod3Table[(hash1 & 0xff00) >> 8] + 3) * dRowColumnCount, 0);
	}
	if ((y > 0 && m_chunk.m_data[z][y - 1][x] == 0) ||
		(y == 0 && !pChunkNearby[2]) ||
		(y == 0 && pChunkNearby[2]->m_chunk.m_data[z][Chunk16::c_size - 1][x] == 0))
	{
		_Face1(faceData, pVertexFaces[2], faceIndex, x, y, z, (((hash2 & 0xff) & 1) + 6) * dRowColumnCount, 0);
	}
	if ((y < c_m1 && m_chunk.m_data[z][y + 1][x] == 0) ||
		(y == c_m1 && !pChunkNearby[3]) ||
		(y == c_m1 && pChunkNearby[3]->m_chunk.m_data[z][0][x] == 0))
	{
		_Face1(faceData, pVertexFaces[3], faceIndex, x, y, z, (_mod3Table[(hash2 & 0xff00) >> 8]) * dRowColumnCount, 0);
	}
	if ((z > 0 && m_chunk.m_data[z - 1][y][x] == 0) ||
		(z == 0 && !pChunkNearby[4]) ||
		(z == 0 && pChunkNearby[4]->m_chunk.m_data[Chunk16::c_size - 1][y][x] == 0))
	{
		_Face1(faceData, pVertexFaces[4], faceIndex, x, y, z, (_mod3Table[(hash1 & 0xff0000) >> 16] + 3) * dRowColumnCount, 0);
	}
	if ((z < c_m1 && m_chunk.m_data[z + 1][y][x] == 0) ||
		(z == c_m1 && !pChunkNearby[5]) ||
		(z == c_m1 && pChunkNearby[5]->m_chunk.m_data[0][y][x] == 0))
	{
		_Face1(faceData, pVertexFaces[5], faceIndex, x, y, z, (_mod3Table[(hash1 & 0xff000000) >> 24] + 3) * dRowColumnCount, 0);
	}
}

bool ClientContext::BakeMeshCache(XMINT3 chunkPos)
{
	ClientChunk* pChunk = nullptr;
	if (!m_chunkScene.TryGetChunk(chunkPos, &pChunk))return false;

	auto& _vert = pChunk->m_vertexCache;
	auto& _index = pChunk->m_indexCache;

	const int verticeSize = sizeof(verticeLeft);
	const int vertexCount = _countof(verticeLeft);
	uint32_t faceIndex[]
	{
		0, 1, 2,
		1, 3, 2,
	};
	int indexSize = sizeof(faceIndex);
	int indexCount = _countof(faceIndex);

	int c_maxFaceCountLow = 1024;//best performance 1024
	_vert.resize(sizeof(verticeLeft) * c_maxFaceCountLow);
	_index.resize(sizeof(faceIndex) * c_maxFaceCountLow);

	char* pFillVertex = _vert.data();
	char* pFillIndex = _index.data();
	char* _pFillVertexEnd = _vert.data() + sizeof(verticeLeft) * c_maxFaceCountLow;

	//--------just a spliter---------
	_FaceData faceData(pFillVertex, pFillIndex, verticeSize, vertexCount);

	auto& pChunkNearby = faceData.pChunkNearby;

	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(-16, 0, 0)), &pChunkNearby[0]);
	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(16, 0, 0)), &pChunkNearby[1]);
	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(0, -16, 0)), &pChunkNearby[2]);
	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(0, 16, 0)), &pChunkNearby[3]);
	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(0, 0, -16)), &pChunkNearby[4]);
	m_chunkScene.TryGetChunk(MathAdd(chunkPos, XMINT3(0, 0, 16)), &pChunkNearby[5]);

	Chunk16& m_chunk = pChunk->m_chunk;

	const float rowColumnCount = 8;
	const float dRowColumnCount = 1.0f / rowColumnCount;
	auto& pVertexFaces = faceData.pVertexFaces;
	pVertexFaces[0] = verticeLeft;
	pVertexFaces[1] = verticeRight;
	pVertexFaces[2] = verticeBottom;
	pVertexFaces[3] = verticeTop;
	pVertexFaces[4] = verticeBack;
	pVertexFaces[5] = verticeFront;

	faceData.faceIndex = faceIndex;
	faceData.dRowColumnCount = dRowColumnCount;
	faceData.hashSeed = 0;
	faceData.hashSeed2 = 1;

	size_t itc = 1;
	for (int z = 0; z < Chunk16::c_size; z++)
		for (int y = 0; y < Chunk16::c_size; y++)
			for (int x = 0; x < Chunk16::c_size; x++)
				if (m_chunk.m_data[z][y][x] != 0)
				{
					if (pFillVertex < _pFillVertexEnd - 6 * sizeof(verticeLeft))
					{
						_unknowFun(this, faceData, pChunk, x, y, z);
					}
					else
					{
						itc++;
						size_t deltaVert = pFillVertex - _vert.data();
						size_t deltaIndex = pFillIndex - _index.data();
						_vert.resize(sizeof(verticeLeft) * c_maxFaceCountLow * itc);
						_index.resize(sizeof(faceIndex) * c_maxFaceCountLow * itc);

						pFillVertex = _vert.data() + deltaVert;
						pFillIndex = _index.data() + deltaIndex;
						_pFillVertexEnd = _vert.data() + sizeof(verticeLeft) * c_maxFaceCountLow * itc;
						_unknowFun(this, faceData, pChunk, x, y, z);
					}
				}

	_index.resize(pFillIndex - _index.data());
	_vert.resize(pFillVertex - _vert.data());
	_index.shrink_to_fit();
	_vert.shrink_to_fit();
	if (_vert.size())
	{
		return true;
	}
	else
		return false;
}

bool ClientContext::BakeMeshFromCache(DirectX::XMINT3 chunkPos, RTUMeshLoader* loader)
{
	ChunkNode_Bottom* pNode = nullptr;
	if (!m_chunkScene.TryGetChunk(chunkPos, &pNode))return false;
	uint32_t vertCount = 0;
	uint32_t indexCount = 0;
	for (int i = 0; i < 64; i++)
	{
		if (pNode->m_children[0][0][i])
		{
			vertCount += pNode->m_children[0][0][i]->m_vertexCache.size() / sizeof(PosNormUVTan);
			indexCount += pNode->m_children[0][0][i]->m_indexCache.size() / sizeof(uint32_t);
		}
	}
	if (vertCount == 0)return false;

	auto& _vert = loader->m_vertexData;
	auto& _index = loader->m_indexData;
	loader->m_vertexStride = sizeof(PosNormUVTan);
	_vert.resize(vertCount * sizeof(PosNormUVTan));
	_index.resize(indexCount * sizeof(uint32_t));
	vertCount = 0;
	indexCount = 0;
	for (int z = 0; z < 4; z++)
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				if (pNode->m_children[z][y][x])
				{
					auto& _v1 = pNode->m_children[z][y][x]->m_vertexCache;
					auto& _i1 = pNode->m_children[z][y][x]->m_indexCache;
					XMVECTOR posDelta = XMVECTORF32{ x * 16.0f,y * 16.0f,z * 16.0f };
					memcpy(_vert.data() + vertCount * sizeof(PosNormUVTan), _v1.data(), _v1.size());
					{
						PosNormUVTan* pSource = reinterpret_cast<PosNormUVTan*>(_v1.data());
						PosNormUVTan* pSourceEnd = reinterpret_cast<PosNormUVTan*>(_v1.data() + _v1.size());
						PosNormUVTan* pData = reinterpret_cast<PosNormUVTan*>(_vert.data() + vertCount * sizeof(PosNormUVTan));
						while (pSource < pSourceEnd)
						{
							XMStoreFloat3(&pData->Position, XMLoadFloat3(&pData->Position) + posDelta);
							pSource++;
							pData++;
						}
					}
					{
						uint32_t* pSource = reinterpret_cast<uint32_t*>(_i1.data());
						uint32_t* pSourceEnd = reinterpret_cast<uint32_t*>(_i1.data() + _i1.size());
						uint32_t* pData = reinterpret_cast<uint32_t*>(_index.data() + indexCount * sizeof(uint32_t));
						while (pSource < pSourceEnd)
						{
							*pData = vertCount + *pSource;
							pSource++;
							pData++;
						}
					}
					pNode->m_children[z][y][x]->m_indexCount = _i1.size() / sizeof(uint32_t);
					//memcpy(_index.data() + indexCount * sizeof(uint32_t), _i1.data(), _i1.size());
					vertCount += _v1.size() / sizeof(PosNormUVTan);
					indexCount += _i1.size() / sizeof(uint32_t);
				}
	pNode->m_meshIndexCount1 = indexCount;
	pNode->m_collision1 = std::unique_ptr<IRTUPhysicsStatic>(m_physicsFactory->GetRigidBodyStatic());
	m_physicsDevice->GetTriangleMesh(pNode->m_collision1.get(), loader->m_vertexData.data(), vertCount, sizeof(PosNormUVTan), loader->m_indexData.data(), indexCount);
	return true;
}

ClientContext::~ClientContext()
{
	std::vector<ClientChunk*>chunks;
	for (auto it = m_chunkScene.m_chunkTree2.begin(); it != m_chunkScene.m_chunkTree2.end(); it++)
	{
		ChunkNodeOP::AllChild(&it->second, chunks);
	}
	for (int i = 0; i < chunks.size(); i++)
	{
		delete chunks[i];
	}
	m_chunkScene.m_chunkTree2.clear();
	chunks.clear();

	while (m_preloadChunks.size())
	{
		delete m_preloadChunks.front();
		m_preloadChunks.pop();
	}
}
