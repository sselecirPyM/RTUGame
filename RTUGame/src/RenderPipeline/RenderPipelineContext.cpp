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
#include "imgui.h"
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
	m_graphicsDevice = std::unique_ptr<IRTUGraphicsDevice>(m_graphicsFactory->GetGraphicsDevice());
	m_graphicsContext = std::unique_ptr<IRTUGraphicsContext>(m_graphicsFactory->GetGraphicsContext());
	m_graphicsContextUp1 = std::unique_ptr<IRTUGraphicsContext>(m_graphicsFactory->GetGraphicsContext());
	m_rootSignaturePasses = std::unique_ptr<IRTURootSignature>(m_graphicsFactory->GetRootSignature());

	m_mainBuffer = std::unique_ptr <IRTUCBuffer>(m_graphicsFactory->GetCBuffer());


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
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(Mesh("cube"), MeshLoader0));

	std::shared_ptr<RTUMeshLoader> MeshLoader1 = std::make_shared<RTUMeshLoader>();
	MeshLoader1->Quad();
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(Mesh("quad"), MeshLoader1));

	std::shared_ptr<RTUMeshLoader> MeshLoader2 = std::make_shared<RTUMeshLoader>();
	MeshLoader2->CubeWire();
	L_meshLoadList0_w->emplace_back(MeshUploadContainer(Mesh("cube_wire"), MeshLoader2));

	std::shared_ptr<RTUTexture2DLoader> texture2DError = std::make_shared<RTUTexture2DLoader>();
	texture2DError->Pure(glm::vec4(1, 0, 1, 1), 32, 32);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(Texture2D("error"), texture2DError));

	std::shared_ptr<RTUTexture2DLoader> texture2DLoading = std::make_shared<RTUTexture2DLoader>();
	texture2DLoading->Pure(glm::vec4(0, 1, 1, 1), 32, 32);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(Texture2D("loading"), texture2DLoading));

	m_graphicsDevice->InitBuffer(m_mainBuffer.get(), sizeof(m_dynamicContex_r->m_cameraData));
	SetQuality(1);

	m_lightBufferGroup.Init(m_graphicsFactory.get(), 512, 65536);
	m_renderObjectBufferGroup.Init(m_graphicsFactory.get(), 512, 512 * 512);
	m_imguiTest.Init(m_graphicsFactory.get(), 512, 65536);
}

void RenderPipelineContext::InitResources(std::wstring assetsPath)
{
	m_assetsPath = assetsPath;

	auto _loader1 = RTUPipelineStateLoader(AssetPath(L"Deferred_GBuffer_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_GBuffer_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("deferred_gbuffer"));

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Shadow_VS.cso").c_str(), nullptr, nullptr);
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("shadow"));

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_IBL_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("deferred_ibl"));

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"Deferred_DirectLight_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("deferred_directLight"));

	_loader1 = RTUPipelineStateLoader(AssetPath(L"Screen_VS.cso").c_str(), nullptr, AssetPath(L"PostProcess_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("postprocess"));

	_loader1 = RTUPipelineStateLoader(AssetPath(L"ImGui_VS.cso").c_str(), nullptr, AssetPath(L"ImGui_PS.cso").c_str());
	m_graphicsDevice->InitPipelineState(&_loader1, PipelineState("imgui"));

	std::shared_ptr<RTUTexture2DLoader> texture2DLoader = std::make_shared<RTUTexture2DLoader>();
	texture2DLoader->LoadTexture(AssetPath(L"Textures/simpletexture.png").c_str(), (RTU_TEXTURE2D_LOAD_FLAGS)(RTU_TEXTURE2D_LOAD_FLAGS_SRGB | RTU_TEXTURE2D_LOAD_FLAGS_MIPMAP));
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(Texture2D("test"), texture2DLoader));

	std::shared_ptr<RTUTexture2DLoader> texture2DLoader1 = std::make_shared<RTUTexture2DLoader>();
	texture2DLoader1->LoadTexture(AssetPath(L"Textures/brdflut.png").c_str(), RTU_TEXTURE2D_LOAD_FLAGS_NONE);
	L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(Texture2D("_BRDFLUT"), texture2DLoader1));
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
	ImGui::GetIO().DisplaySize = ImVec2(width, height);
	m_screenSize = glm::u32vec2(width, height);
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

IRTUPipelineState* RenderPipelineContext::PipelineState(std::string name)
{
	auto* _pipelineState = m_pipelineStates[name].get();
	if (_pipelineState == nullptr)
	{
		_pipelineState = m_graphicsFactory->GetPipelineState();
		m_pipelineStates[name] = std::unique_ptr<IRTUPipelineState>(_pipelineState);
	}
	return _pipelineState;
}
IRTUTexture2D* RenderPipelineContext::Texture2D(std::string name)
{
	auto* tex = m_textures[name].get();
	if (tex == nullptr)
	{
		tex = m_graphicsFactory->GetTexture2D();
		m_textures[name] = std::unique_ptr <IRTUTexture2D>(tex);
	}
	return tex;
}
IRTURenderTexture2D* RenderPipelineContext::RenderTexture2D(std::string name)
{
	auto* tex = m_graphicsFactory->GetRenderTexture2D();
	m_renderTextures[name] = std::unique_ptr <IRTURenderTexture2D>(tex);
	return tex;
}
IRTUMesh* RenderPipelineContext::Mesh(std::string name)
{
	auto* mesh = m_meshes[name].get();
	if (mesh == nullptr)
	{
		mesh = m_graphicsFactory->GetMesh();
		m_meshes[name] = std::unique_ptr <IRTUMesh>(mesh);
	}
	return mesh;
}