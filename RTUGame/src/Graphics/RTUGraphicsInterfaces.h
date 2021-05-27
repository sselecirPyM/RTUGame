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
#include "RTUMeshLoader.h"
#include "RTUTexture2DLoader.h"
#include "RTUPipelineStateLoader.h"
#include "RTUPSODesc.h"
class IRTURenderTexture2D
{
public:
	virtual void AsDSV(DXGI_FORMAT format) = 0;
	virtual void AsRTV(DXGI_FORMAT format) = 0;
	virtual void AsRTVUAV(DXGI_FORMAT format) = 0;

	virtual ~IRTURenderTexture2D() {};
};
class IRTUMesh
{
public:
	virtual int GetIndexCount() = 0;
	virtual RTU_STATES GetState() = 0;
	virtual ~IRTUMesh() {};
};
class IRTUCBuffer
{
public:
	virtual ~IRTUCBuffer() {};
};
class IRTUSBuffer
{
public:
	virtual ~IRTUSBuffer() {};
};
class IRTUPipelineState
{
public:
	virtual ~IRTUPipelineState() {};
};
class IRTUTexture2D
{
public:
	virtual ~IRTUTexture2D() {};
};
class IRTURootSignature
{
public:
	virtual ~IRTURootSignature() {};
};
class IRTUGraphicsDevice
{
public:
	virtual void Init() = 0;
	virtual void CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat) = 0;
	virtual void Present(bool vsync) = 0;
	virtual void BeginCommandAllocator() = 0;
	virtual void WaitForGpu() = 0;
	virtual void Next() = 0;
	virtual void SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat) = 0;

	virtual void InitRootSignaturePass1(IRTURootSignature* rootSignature) = 0;
	virtual void InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* pipelineState) = 0;
	virtual void InitBuffer(IRTUCBuffer* buffer, uint32_t size) = 0;
	virtual void InitBuffer(IRTUSBuffer* buffer, uint32_t size) = 0;
	virtual void InitRenderTexture2D(IRTURenderTexture2D* texture, int width, int height) = 0;
	virtual void RemoveMesh(IRTUMesh* mesh) = 0;
	virtual void Uninit() = 0;
	virtual uint64_t GetExecutedCount() = 0;
	virtual ~IRTUGraphicsDevice() {};
};
class IRTUGraphicsContext
{
public:
	virtual void Init(IRTUGraphicsDevice* device) = 0;
	virtual void SetHeapDefault() = 0;
	virtual void SetGraphicsRootSignature(IRTURootSignature* signature) = 0;
	virtual void SetRenderTargetScreen(IRTURenderTexture2D* dsv) = 0;
	virtual void SetRenderTargetDSV(IRTURenderTexture2D* dsv) = 0;
	virtual void SetRenderTargetRTVDSV(IRTURenderTexture2D** rtv, int rtvCount, IRTURenderTexture2D* dsv) = 0;
	virtual void SetRenderTargetRTV(IRTURenderTexture2D** rtv, int rtvCount) = 0;
	virtual void SetPipelineState(IRTUPipelineState* pipelineState, RTUPipelineStateDesc* desc) = 0;
	virtual void SetMesh(IRTUMesh* mesh) = 0;
	virtual void SetCBVR(IRTUCBuffer* buffer, int offset256, int size256, int slot) = 0;
	virtual void SetCBVR(IRTUSBuffer* buffer, int offset256, int size256, int slot) = 0;
	virtual void SetSRVT(IRTUTexture2D* texture, int slot) = 0;
	virtual void SetSRVT(IRTURenderTexture2D* texture, int slot) = 0;
	virtual void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation) = 0;
	virtual void DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation) = 0;
	virtual void UpdateBuffer(IRTUCBuffer* buffer, void* data, int dataSize) = 0;
	virtual void UpdateBuffer(IRTUSBuffer* buffer, void* data, int dataSize) = 0;
	virtual void UploadMesh(IRTUMesh* mesh, RTUMeshLoader* loader) = 0;
	virtual void UploadTexture(IRTUTexture2D* texture, RTUTexture2DLoader* loader) = 0;
	virtual void ClearScreen(const float* color) = 0;
	virtual void ClearRTV(IRTURenderTexture2D* rtv, const float* color) = 0;
	virtual void ClearDSV(IRTURenderTexture2D* dsv) = 0;
	virtual void ScreenBeginRender() = 0;
	virtual void ScreenEndRender() = 0;
	virtual void BeginCommand(IRTUGraphicsDevice* device) = 0;
	virtual void EndCommand() = 0;
	virtual void ExecuteCommand(IRTUGraphicsDevice* device) = 0;

	virtual ~IRTUGraphicsContext() {};
};
class IRTUGraphicsFactory
{
public:
	virtual IRTUGraphicsDevice* GetGraphicsDevice() = 0;
	virtual IRTUGraphicsContext* GetGraphicsContext() = 0;
	virtual IRTURenderTexture2D* GetRenderTexture2D() = 0;
	virtual IRTUMesh* GetMesh() = 0;
	virtual IRTUCBuffer* GetCBuffer() = 0;
	virtual IRTUSBuffer* GetSBuffer() = 0;
	virtual IRTUPipelineState* GetPipelineState() = 0;
	virtual IRTUTexture2D* GetTexture2D() = 0;
	virtual IRTURootSignature* GetRootSignature() = 0;

	virtual ~IRTUGraphicsFactory() {};
};