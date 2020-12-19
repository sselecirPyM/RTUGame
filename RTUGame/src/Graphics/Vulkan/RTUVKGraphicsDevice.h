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
#include "../RTUGraphicsInterfaces.h"
class RTUVKGraphicsDevice : public IRTUGraphicsDevice
{
public:
	void Init();
	void CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat);
	void Present(bool vsync);
	void BeginCommandAllocator();
	void WaitForGpu();
	void Next();
	void SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat);

	void InitRootSignaturePass1(IRTURootSignature* rootSignature);
	void InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* pipelineState);
	void InitBuffer(IRTUCBuffer* buffer, uint32_t size);
	void InitBuffer(IRTUSBuffer* buffer, uint32_t size);
	void InitRenderTexture2D(IRTURenderTexture2D* texture, int width, int height);
	void Uninit();
	~RTUVKGraphicsDevice();

	int GetExecuteIndex() { return m_executeIndex; }
	uint64_t GetExecutedCount() { return m_executedCount; }

	int m_executeIndex = 0;
	uint64_t m_executedCount = 0;

	VkInstance m_instance;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	VkCommandPool m_commandPool;
	VkSurfaceKHR m_surface;
	uint32_t m_family_count=0;
};

