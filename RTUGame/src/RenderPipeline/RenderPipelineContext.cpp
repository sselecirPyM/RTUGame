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
#include "RenderPipelineContext.h"
using namespace DirectX;

void RenderPipelineDynamicContext::Clear()
{
	m_cullResultChunk.clear();
	m_cullResultTransparent.clear();
	m_anotherRenderList.clear();
	m_shadowCullResult.clear();
}

void RenderPipelineContext::Init()
{
	m_renderPipelineResources.Init(m_graphicsFactory.get());

	m_graphicsDevice = std::unique_ptr<IRTUGraphicsDevice>(m_graphicsFactory->GetGraphicsDevice());
	m_graphicsContext = std::unique_ptr<IRTUGraphicsContext>(m_graphicsFactory->GetGraphicsContext());
	m_graphicsContextUp1 = std::unique_ptr<IRTUGraphicsContext>(m_graphicsFactory->GetGraphicsContext());
	m_rootSignaturePasses = std::unique_ptr<IRTURootSignature>(m_graphicsFactory->GetRootSignature());

	m_mainBuffer = std::unique_ptr <IRTUCBuffer>(m_graphicsFactory->GetCBuffer());

	m_cubeModel1 = std::unique_ptr <IRTUMesh>(m_graphicsFactory->GetMesh());
	m_cubeModelWire = std::unique_ptr <IRTUMesh>(m_graphicsFactory->GetMesh());
	m_quadModel0 = std::unique_ptr <IRTUMesh>(m_graphicsFactory->GetMesh());
	m_textureLoading = std::unique_ptr <IRTUTexture2D>(m_graphicsFactory->GetTexture2D());
	m_textureError = std::unique_ptr <IRTUTexture2D>(m_graphicsFactory->GetTexture2D());
	m_textureBrdfLut = std::unique_ptr <IRTUTexture2D>(m_graphicsFactory->GetTexture2D());
	m_testTexture = std::unique_ptr <IRTUTexture2D>(m_graphicsFactory->GetTexture2D());

	for (int i = 0; i < _countof(m_screenSizeRTV); i++)
		m_screenSizeRTV[i] = std::unique_ptr<IRTURenderTexture2D>(m_graphicsFactory->GetRenderTexture2D());
	for (int i = 0; i < _countof(m_screenSizeDSV); i++)
		m_screenSizeDSV[i] = std::unique_ptr<IRTURenderTexture2D>(m_graphicsFactory->GetRenderTexture2D());
	for (int i = 0; i < _countof(m_shadowMaps); i++)
		m_shadowMaps[i] = std::unique_ptr<IRTURenderTexture2D>(m_graphicsFactory->GetRenderTexture2D());
	m_outputRTV = std::unique_ptr<IRTURenderTexture2D>(m_graphicsFactory->GetRenderTexture2D());

	m_graphicsDevice->Init();
	m_graphicsContext->Init(m_graphicsDevice.get());
	m_graphicsContextUp1->Init(m_graphicsDevice.get());
	m_graphicsDevice->InitRootSignaturePass1(m_rootSignaturePasses.get());

	std::shared_ptr<RTUMeshLoader> MeshLoader0 = std::make_shared<RTUMeshLoader>();
	MeshLoader0->Cube1();
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(m_cubeModel1.get(), MeshLoader0));

	std::shared_ptr<RTUMeshLoader> MeshLoader1 = std::make_shared<RTUMeshLoader>();
	MeshLoader1->Quad();
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(m_quadModel0.get(), MeshLoader1));

	std::shared_ptr<RTUMeshLoader> MeshLoader2 = std::make_shared<RTUMeshLoader>();
	MeshLoader2->CubeWire();
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(m_cubeModelWire.get(), MeshLoader2));

	std::shared_ptr<RTUTexture2DLoader> texture2DError = std::make_shared<RTUTexture2DLoader>();
	texture2DError->Pure(glm::vec4(1, 0, 1, 1), 32, 32);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(m_textureError.get(), texture2DError));

	std::shared_ptr<RTUTexture2DLoader> texture2DLoading = std::make_shared<RTUTexture2DLoader>();
	texture2DLoading->Pure(glm::vec4(0, 1, 1, 1), 32, 32);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(m_textureLoading.get(), texture2DLoading));

	m_graphicsDevice->InitBuffer(m_mainBuffer.get(), sizeof(m_dynamicContex_r->m_cameraData));
	SetQuality(1);

	m_lightBufferGroup.Init(m_graphicsFactory.get(), 512, 65536);
	m_renderObjectBufferGroup.Init(m_graphicsFactory.get(), 256, 256 * 512);
}

void RenderPipelineContext::InitResources(std::wstring assetsPath)
{
	m_assetsPath = assetsPath;

	auto& rpr = m_renderPipelineResources;
	auto _loader1 = RTUPipelineStateLoader(AssetPath(L"Deferred_GBuffer_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_GBuffer_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, rpr.m_deferred_gbuffer.get());

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Shadow_VS.cso").c_str(), nullptr, nullptr);
	m_graphicsDevice->InitPipelineState(&_loader1, rpr.m_shadow.get());

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_IBL_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, rpr.m_deferred_IBL.get());

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_DirectLight_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, rpr.m_deferred_directLight.get());

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"PostProcess_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, rpr.m_postprocess.get());

	std::shared_ptr<RTUTexture2DLoader> texture2DLoader = std::make_shared<RTUTexture2DLoader>();
	texture2DLoader->LoadTexture(AssetPath(L"Textures/simpletexture.png").c_str(), (RTU_TEXTURE2D_LOAD_FLAGS)(RTU_TEXTURE2D_LOAD_FLAGS_SRGB | RTU_TEXTURE2D_LOAD_FLAGS_MIPMAP));
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(m_testTexture.get(), texture2DLoader));

	std::shared_ptr<RTUTexture2DLoader> texture2DLoader1 = std::make_shared<RTUTexture2DLoader>();
	texture2DLoader1->LoadTexture(AssetPath(L"Textures/brdflut.png").c_str(), RTU_TEXTURE2D_LOAD_FLAGS_NONE);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(m_textureBrdfLut.get(), texture2DLoader1));
}

void RenderPipelineContext::SwapDynamicContext()
{
	m_dynamicContex_r->Clear();
	std::swap(m_dynamicContex_w, m_dynamicContex_r);

	m_renderCount++;
	m_dynamicContex_w->m_renderCount = m_renderCount;
	m_dynamicContex_w->m_syncIndex = m_renderCount % c_maxQueueFrameCount;
}

void RenderPipelineContext::ProcessSizeChange(int width, int height)
{
	m_graphicsContext->Init(m_graphicsDevice.get());
	m_graphicsDevice->CreateWindowSizeDependentResources(width, height, m_swapChainFormat);
	m_screenSize = XMUINT2(width, height);
	m_aspectRatio = float(width) / float(height);
	for (int i = 0; i < _countof(m_screenSizeRTV); i++)
	{
		m_screenSizeRTV[i]->AsRTVUAV(m_gbufferFormat);
		m_graphicsDevice->InitRenderTexture2D(m_screenSizeRTV[i].get(), width, height);
	}
	for (int i = 0; i < _countof(m_screenSizeDSV); i++)
	{
		m_screenSizeDSV[i]->AsDSV(m_depthFormat);
		m_graphicsDevice->InitRenderTexture2D(m_screenSizeDSV[i].get(), width, height);
	}
	m_outputRTV->AsRTVUAV(m_outputFormat);
	m_graphicsDevice->InitRenderTexture2D(m_outputRTV.get(), width, height);
}

void RenderPipelineContext::SetQuality(std::uint32_t quality)
{
	std::uint32_t shadowMapSize = quality > 1 ? 2048 : 1024;
	for (int i = 0; i < _countof(m_shadowMaps); i++)
	{
		m_shadowMaps[i]->AsDSV(m_depthFormat);
		m_graphicsDevice->InitRenderTexture2D(m_shadowMaps[i].get(), shadowMapSize, shadowMapSize);
	}
}

void RenderPipelineContext::ClearRecycleList(int executeIndex)
{
	m_meshLoaderRecycleList[executeIndex].clear();
	m_textureLoaderRecycleList[executeIndex].clear();
	m_meshRecycleList[executeIndex].clear();
	m_meshRecycleList_w = &m_meshRecycleList[executeIndex];
}
