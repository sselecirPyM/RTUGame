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
#include "ClientPlayer.h"
#include "ClientChunk.h"
#include "ChunkScene.h"
#include "../Physics/RTUPhysicsInterfaces.h"
class ClientContext
{
public:
	void Init();
	bool BakeMeshCache(glm::i32vec3 chunkPos);
	bool BakeMeshFromCache(glm::i32vec3 chunkPos, RTUMeshLoader* loader);
	std::uint64_t m_currentFrameStamp;
	std::uint64_t m_tickRate;

	ClientPlayer m_clientPlayer;
	ChunkScene m_chunkScene;

	std::unique_ptr<IRTUPhysicsFactory> m_physicsFactory;
	std::unique_ptr<IRTUPhysicsDevice> m_physicsDevice;
	std::unique_ptr<IRTUPhysicsScene> m_physicsScene;
	std::unique_ptr<IRTUPhysicsController> m_physicsController;
	std::queue<ClientChunk*> m_preloadChunks;
	void* ImGuiContext;
	~ClientContext();

	uint64_t TestTotalTime = 0;
};

