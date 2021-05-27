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
#include "RTUDX11RootSignature.h"
#include "RTUDX11GraphicsDevice.h"
class RTUDX11GraphicsContext : public IRTUGraphicsContext
{
public:
	void Init(IRTUGraphicsDevice* device);
	void SetHeapDefault();
	void SetGraphicsRootSignature(IRTURootSignature* signature);
	void SetRenderTargetScreen(IRTURenderTexture2D* dsv);
	void SetRenderTargetDSV(IRTURenderTexture2D* dsv);
	void SetRenderTargetRTVDSV(IRTURenderTexture2D** rtv, int rtvCount, IRTURenderTexture2D* dsv);
	void SetRenderTargetRTV(IRTURenderTexture2D** rtv, int rtvCount);
	void SetPipelineState(IRTUPipelineState* pipelineState, RTUPipelineStateDesc* desc);
	void SetMesh(IRTUMesh* mesh);
	void SetCBVR(IRTUCBuffer* buffer, int offset256, int size256, int slot);
	void SetCBVR(IRTUSBuffer* buffer, int offset256, int size256, int slot);
	void SetSRVT(IRTUTexture2D* texture, int slot);
	void SetSRVT(IRTURenderTexture2D* texture, int slot);
	void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation);
	void DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation);
	void UpdateBuffer(IRTUCBuffer* buffer, void* data, int dataSize);
	void UpdateBuffer(IRTUSBuffer* buffer, void* data, int dataSize);
	void UploadMesh(IRTUMesh* mesh, RTUMeshLoader* loader);
	void UploadTexture(IRTUTexture2D* texture, RTUTexture2DLoader* loader);
	void ClearScreen(const float* color);
	void ClearRTV(IRTURenderTexture2D* rtv, const float* color);
	void ClearDSV(IRTURenderTexture2D* dsv);
	void ScreenBeginRender();
	void ScreenEndRender();
	void BeginCommand(IRTUGraphicsDevice* device);
	void EndCommand();
	void ExecuteCommand(IRTUGraphicsDevice* device);

	Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
	Microsoft::WRL::ComPtr<ID3D11CommandList>	m_commandList;
	RTUDX11GraphicsDevice* m_device;
	RTUDX11RootSignature* m_currentRootSignature = nullptr;
};

