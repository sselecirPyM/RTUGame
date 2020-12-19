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
#include "../Client/ClientContext.h"
#include "RenderPipelineContext.h"
class ClientToRenderPipeline
{
public:
	void Process1(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext);
	void Process2(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext);
private:
	void Process1_1(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext);
	struct _BakeMeshTaskData
	{
		std::set<XMINT3, XMINT3Comparer> posSet;
		std::set<XMINT3, XMINT3Comparer> posSet64;
		void Clear()
		{
			posSet.clear();
			posSet64.clear();
		}
	};
	_BakeMeshTaskData _m_bakeMeshTaskData;
	struct _BakeData
	{
		std::vector<XMINT3> xList;
		std::vector<std::unique_ptr<IRTUMesh>> recycleList;
		std::vector<MeshUploadContainer> bakedMeshList;
		std::vector<ChunkNode_Bottom*> nodeList;
		ClientContext* _clientContext;
		IRTUGraphicsFactory* _graphicsFactory;
		size_t count;
		void Init2(std::set<XMINT3, XMINT3Comparer> set1, ClientContext* clientContext, IRTUGraphicsFactory* graphicsFactory)
		{
			xList.clear();
			recycleList.clear();
			bakedMeshList.clear();
			nodeList.clear();

			_clientContext = clientContext;
			_graphicsFactory = graphicsFactory;
			for (auto it = set1.begin(); it != set1.end(); it++)
			{
				ChunkNode_Bottom* nd = nullptr;
				if (clientContext->m_chunkScene.TryGetChunk(*it, &nd))
				{
					nodeList.push_back(nd);
					xList.push_back(*it);
				}
			}
			count = xList.size();
			recycleList.resize(count);
			bakedMeshList.resize(count);
		}
	};
	_BakeData ba1;

	std::future<void> m_bakeMeshTask;
};

