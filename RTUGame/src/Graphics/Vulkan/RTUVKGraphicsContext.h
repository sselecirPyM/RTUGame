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
class RTUVKGraphicsContext : public IRTUGraphicsContext
{
public:
	void Init(IRTUGraphicsDevice* device);
	void SetHeapDefault(IRTUGraphicsDevice* device);
	void SetGraphicsRootSignature(IRTURootSignature* signature);
	void SetRenderTargetScreen(IRTUGraphicsDevice* device, IRTURenderTexture2D* dsv);
	void SetRenderTargetDSV(IRTUGraphicsDevice* device, IRTURenderTexture2D* dsv);
	void SetRenderTargetRTVDSV(IRTUGraphicsDevice* device, IRTURenderTexture2D** rtv, int rtvCount, IRTURenderTexture2D* dsv);
	void SetRenderTargetRTV(IRTUGraphicsDevice* device, IRTURenderTexture2D** rtv, int rtvCount);
	void SetPipelineState(IRTUGraphicsDevice* device, IRTURootSignature* rootSignature, IRTUPipelineState* pipelineState, RTUPipelineStateDesc* desc);
	void SetMesh(IRTUMesh* mesh);
	void SetCBVR(IRTUCBuffer* buffer, int syncIndex, int offset256, int size256, int slot);
	void SetCBVR(IRTUSBuffer* buffer, int syncIndex, int offset256, int size256, int slot);
	void SetSRVT(IRTUTexture2D* texture, int slot);
	void SetSRVT(IRTURenderTexture2D* texture, int slot);
	void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation);
	void DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation);
	void UpdateBuffer(IRTUCBuffer* buffer, int syncIndex, void* data, int dataSize);
	void UpdateBuffer(IRTUSBuffer* buffer, int syncIndex, void* data, int dataSize);
	void UploadMesh(IRTUGraphicsDevice* device, IRTUMesh* mesh, RTUMeshLoader* loader);
	void UploadTexture(IRTUGraphicsDevice* device, IRTUTexture2D* texture, RTUTexture2DLoader* loader);
	void ClearScreen(IRTUGraphicsDevice* device, const float* color);
	void ClearRTV(IRTUGraphicsDevice* device, IRTURenderTexture2D* rtv, const float* color);
	void ClearDSV(IRTUGraphicsDevice* device, IRTURenderTexture2D* dsv);
	void ScreenBeginRender();
	void ScreenEndRender();
	void BeginCommand(IRTUGraphicsDevice* device);
	void EndCommand();
	void ExecuteCommand(IRTUGraphicsDevice* device);
	~RTUVKGraphicsContext();

	VkDevice m_refDevice;
	VkCommandPool m_refCommandPool;
	VkCommandBuffer m_commandBuffer;
};

