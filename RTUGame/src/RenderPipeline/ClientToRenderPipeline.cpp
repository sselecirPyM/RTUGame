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
#include "ClientToRenderPipeline.h"
#include "../Math/FrustumCull.h"
#include "../Common/IncreasePerformance.h"
#include <tbb/tbb.h>
using namespace DirectX;
inline bool asyncReady(std::future<void>& _async)
{
	return !_async.valid() || _async.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
void ClientToRenderPipeline::Process1(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext)
{
	std::set<XMINT3, XMINT3Comparer>& posSet = _m_bakeMeshTaskData.posSet;
	std::set<XMINT3, XMINT3Comparer>& posSet64 = _m_bakeMeshTaskData.posSet64;
	auto& preloadChunks = clientContext->m_preloadChunks;
	if (asyncReady(m_bakeMeshTask))
	{
		if (_m_bakeMeshTaskData.posSet.size())
		{
			for (size_t i = 0; i < ba1.count; i++)
			{
				auto& pNode = ba1.nodeList[i];
				renderPipelineContext->m_meshRecycleList_w->emplace_back(std::move(pNode->m_mergedMesh));
				pNode->m_mergedMesh = std::move(pNode->m_mergedMesh1);
				pNode->m_meshIndexCount = pNode->m_meshIndexCount1;
				for (int l = 0; l < 64; l++)
				{
					if (pNode->m_children[0][0][l])
					{
						pNode->m_startIndexCount[0][0][l] = pNode->m_children[0][0][l]->m_indexCount;
					}
					else
					{
						pNode->m_startIndexCount[0][0][l] = 0;
					}
				}
			}
			for (size_t i = 0; i < ba1.count; i++)
			{
				auto& pNode = ba1.nodeList[i];
				auto p = MathSub(pNode->m_position, clientContext->m_clientPlayer.m_position_i);
				pNode->m_collision = std::move(pNode->m_collision1);
				if (pNode->m_collision)
					clientContext->m_physicsDevice->SceneAddStatic(clientContext->m_physicsScene.get(), pNode->m_collision.get(), glm::vec3(p.x, p.y, p.z), glm::vec4(0, 0, 0, 1));
			}

			{
				std::lock_guard<std::mutex> guard(renderPipelineContext->lock_meshLoadList);
				for (size_t i = 0; i < ba1.count; i++)
					if (ba1.bakedMeshList[i].m_mesh)
						renderPipelineContext->L_meshLoadList0_w->emplace_back(ba1.bakedMeshList[i]);
			}
			_m_bakeMeshTaskData.Clear();
		}
		_m_bakeMeshTaskData.Clear();
		int numChunkLoad = std::min(preloadChunks.size(), 32u);
		for (int i = 0; i < numChunkLoad; i++)
		{
			ClientChunk* chunkPop = nullptr;
			auto chunk = preloadChunks.front();
			preloadChunks.pop();
			clientContext->m_chunkScene.Add(chunk, &chunkPop);
			XMINT3 position = chunk->m_chunk.m_position;
			posSet.insert(position);
			posSet.insert(MathAdd(position, XMINT3(-16, 0, 0)));
			posSet.insert(MathAdd(position, XMINT3(16, 0, 0)));
			posSet.insert(MathAdd(position, XMINT3(0, -16, 0)));
			posSet.insert(MathAdd(position, XMINT3(0, 16, 0)));
			posSet.insert(MathAdd(position, XMINT3(0, 0, -16)));
			posSet.insert(MathAdd(position, XMINT3(0, 0, 16)));

			if (chunkPop != nullptr)
			{
				delete chunkPop;
			}
		}
		for (auto it = posSet.begin(); it != posSet.end(); it++)
		{
			posSet64.insert(MathAlignedDown(*it, 64));
		}
		m_bakeMeshTask = std::async(std::launch::async, &ClientToRenderPipeline::Process1_1, this, clientContext, renderPipelineContext);
	}
}

inline void _CullResult(ChunkScene& tree, XMMATRIX& vMatrix, XMMATRIX& pMatrix, XMINT3& cameraPosI, std::vector<DrawContainer0>& cullResult)
{
	FrustumCull frustum1;
	frustum1.Init(vMatrix, pMatrix);
	tree.Cull(frustum1, cameraPosI, cullResult);
}

void ClientToRenderPipeline::Process2(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext)
{
	auto& dcw = renderPipelineContext->m_dynamicContex_w;

	dcw->m_cameraData = clientContext->m_clientPlayer.GetCameraData(renderPipelineContext->m_aspectRatio);
	dcw->m_mainLightData = clientContext->m_clientPlayer.mainLight.Light;
	XMFLOAT4X4 _lightVMat;
	XMFLOAT4X4 _lightPMat;
	clientContext->m_clientPlayer.mainLight.GetLightMatrix(dcw->m_cameraData.camPos_f, clientContext->m_clientPlayer.m_angle, 32, &dcw->m_mainLightMatrice[0], &_lightVMat, &_lightPMat, 1);

	XMMATRIX invertCamMatrix = XMMatrixTranspose(XMLoadFloat4x4(&dcw->m_cameraData.pvMatrix));
	XMMATRIX camvMatrix = XMLoadFloat4x4(&dcw->m_cameraData.vMatrix);
	XMMATRIX campMatrix = XMLoadFloat4x4(&dcw->m_cameraData.pMatrix);
	_CullResult(clientContext->m_chunkScene, camvMatrix, campMatrix, dcw->m_cameraData.camPos_i, dcw->m_cullResultChunk);
	XMMATRIX shadowvMatrix = XMLoadFloat4x4(&_lightVMat);
	XMMATRIX shadowpMatrix = XMLoadFloat4x4(&_lightPMat);
	_CullResult(clientContext->m_chunkScene, shadowvMatrix, shadowpMatrix, dcw->m_cameraData.camPos_i, dcw->m_shadowCullResult);

	XMVECTOR cameraPosition = XMLoadFloat3(&dcw->m_cameraData.camPos_f);
	XMVECTOR testPoint = XMVector3Normalize(XMVector3TransformCoord(XMVECTORF32{}, invertCamMatrix) - cameraPosition);
	RTU::Ray ray;
	ray.filterMask = -1;
	ray.StartPoint = dcw->m_cameraData.camPos_f;
	ray.length = 8;
	XMStoreFloat3(&ray.EndPoint, cameraPosition + (testPoint * ray.length));
	XMINT3 hitResult;
	XMINT3 hitSurface;

	if (clientContext->m_chunkScene.CastRay(ray, dcw->m_cameraData.camPos_i, &hitResult, &hitSurface))
	{
		auto _1 = MathSub(hitSurface, dcw->m_cameraData.camPos_i);
		XMFLOAT4X4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadSInt3(&_1))));
		dcw->m_anotherRenderList.push_back(DrawContainer1(renderPipelineContext->m_cubeModel1.get(), matrix, renderPipelineContext->m_renderPipelineResources.m_deferred_gbuffer.get()));
	}
}

void ClientToRenderPipeline::Process1_1(ClientContext* clientContext, RenderPipelineContext* renderPipelineContext)
{
	std::set<XMINT3, XMINT3Comparer>& posSet = _m_bakeMeshTaskData.posSet;
	std::set<XMINT3, XMINT3Comparer>& posSet64 = _m_bakeMeshTaskData.posSet64;
	{
		struct _MeshCacheBakeTask
		{
			std::vector<XMINT3> _xList;
			ClientContext* _clientContext;
			size_t count;
			_MeshCacheBakeTask(std::set<XMINT3, XMINT3Comparer> set1, ClientContext* clientContext)
			{
				_clientContext = clientContext;
				for (auto it = set1.begin(); it != set1.end(); it++)
				{
					ClientChunk* chunk = nullptr;
					if (clientContext->m_chunkScene.TryGetChunk(*it, &chunk))
						_xList.push_back(*it);
				}
				count = _xList.size();
			}

			void operator()(const tbb::blocked_range<size_t>& r) const {
				for (size_t i = r.begin(); i != r.end(); ++i)
				{
					_clientContext->BakeMeshCache(_xList[i]);
				}
			}
		};
		_MeshCacheBakeTask meshBakeTask(posSet, clientContext);

		PerformaceTimer timer1 = {};
		timer1.Start();
		tbb::parallel_for(tbb::blocked_range<size_t>(0, meshBakeTask.count), meshBakeTask);
		timer1.Stop();
		if (timer1.timeStop - timer1.timeStart > 1000)
		{
			clientContext->TestTotalTime += timer1.timeStop - timer1.timeStart;
			PerformaceTimer::print(clientContext->TestTotalTime);
		}
	}
	{
		ba1.Init2(posSet64, clientContext, renderPipelineContext->m_graphicsFactory.get());

		struct _MeshBakeTask
		{
			_BakeData* _bakeData;
			_MeshBakeTask(_BakeData* b) : _bakeData(b) {}
			void operator()(const tbb::blocked_range<size_t>& r) const {
				for (size_t i = r.begin(); i != r.end(); ++i)
				{
					ChunkNode_Bottom* chunk = _bakeData->nodeList[i];
					std::shared_ptr <RTUMeshLoader> loader = std::make_shared<RTUMeshLoader>();
					if (_bakeData->_clientContext->BakeMeshFromCache(_bakeData->xList[i], loader.get()))
					{
						chunk->m_mergedMesh1 = std::unique_ptr<IRTUMesh>(_bakeData->_graphicsFactory->GetMesh());
						_bakeData->bakedMeshList[i] = MeshUploadContainer(chunk->m_mergedMesh1.get(), loader);
					}
				}
			}
		};
		_MeshBakeTask meshBakeTask(&ba1);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, ba1.count), meshBakeTask);
	}
}

