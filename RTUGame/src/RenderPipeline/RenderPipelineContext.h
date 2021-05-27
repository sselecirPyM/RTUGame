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
#include "../Graphics/RTUGraphicsInterfaces.h"
#include "../Client/ClientPlayer.h"
#include "Containers.h"
#include "../Graphics/Advanced/RTUCBufferGroup.h"

class RenderPipelineDynamicContext
{
public:
	void Clear();

	DirectX::XMFLOAT4X4 m_mainLightMatrice[4];
	DirectLight m_mainLightData;
	GraphicsCameraData m_cameraData;
	std::uint64_t m_renderCount;
	int m_syncIndex;
	std::vector<DrawContainer1> m_anotherRenderList;
	std::vector<DrawContainer0> m_cullResultChunk;
	std::vector<DrawContainer0> m_cullResultTransparent;
	std::vector<DrawContainer0> m_shadowCullResult;
};
class RenderPipelineContext
{
public:
	void Init();
	void InitResources(std::wstring assetsPath);
	void SwapDynamicContext();
	void ProcessSizeChange(int width, int height);
	void SetQuality(std::uint32_t quality);

	glm::u32vec2 m_screenSize;
	float m_aspectRatio;

	DXGI_FORMAT m_swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_gbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_outputFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT m_depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	std::unique_ptr<IRTUGraphicsFactory> m_graphicsFactory;

	std::unique_ptr<IRTUGraphicsDevice> m_graphicsDevice = nullptr;
	std::unique_ptr<IRTUGraphicsContext> m_graphicsContext = nullptr;
	std::unique_ptr<IRTUGraphicsContext> m_graphicsContextUp1 = nullptr;
	std::unique_ptr<IRTURootSignature> m_rootSignaturePasses = nullptr;
	float m_clearColor[4] = { 0.4f, 0.8f, 1.0f, 1.0f };
	std::uint64_t m_renderCount = 0;
	std::uint64_t m_completedRenderCount = 0;

	std::unique_ptr <IRTUCBuffer> m_mainBuffer;
	RTUCBufferGroup m_lightBufferGroup;
	RTUCBufferGroup m_renderObjectBufferGroup;
	RTUCBufferGroup m_imguiTest;

	std::map<std::string, std::unique_ptr<IRTUPipelineState>> m_pipelineStates;
	std::map<std::string, std::unique_ptr <IRTUTexture2D>>m_textures;
	std::map<std::string, std::unique_ptr <IRTURenderTexture2D>>m_renderTextures;
	std::map<std::string, std::unique_ptr <IRTUMesh>> m_meshes;

	std::unique_ptr <IRTURenderTexture2D> m_screenSizeRTV[4];
	std::unique_ptr <IRTURenderTexture2D> m_screenSizeDSV[2];
	std::unique_ptr <IRTURenderTexture2D> m_shadowMaps[2];
	std::unique_ptr <IRTURenderTexture2D> m_outputRTV;

	RenderPipelineDynamicContext* m_dynamicContex_w = &_dynamicContex[0];
	RenderPipelineDynamicContext* m_dynamicContex_r = &_dynamicContex[1];
	RenderPipelineDynamicContext _dynamicContex[2];

	std::vector<MeshUploadContainer>* L_meshLoadList0_w = &_meshLoadList[0];
	std::vector<MeshUploadContainer>* m_meshLoadList0_r = &_meshLoadList[1];
	std::vector<MeshUploadContainer> _meshLoadList[2];
	std::mutex lock_meshLoadList;

	std::vector<Texture2DUploadContainer>* L_texture2DLoadList0_w = &_texture2DLoadList[0];
	std::vector<Texture2DUploadContainer>* m_texture2DLoadList0_r = &_texture2DLoadList[1];
	std::vector<Texture2DUploadContainer> _texture2DLoadList[2];
	std::mutex lock_texture2DLoadList;

	std::vector<IRTUGraphicsContext*> m_executeList;
	char m_bigBuffer[65536];

	std::wstring m_assetsPath;
	IRTUPipelineState* PipelineState(std::string name);
	IRTUTexture2D* Texture2D(std::string name);
	IRTURenderTexture2D* RenderTexture2D(std::string name);
	IRTUMesh* Mesh(std::string name);
private:
	std::wstring AssetPath(LPCWSTR assetName)
	{
		return m_assetsPath + assetName;
	}
};