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
#include "RTUVKGraphicsContext.h"
#include "VkUtil.h"
#include "RTUVKGraphicsDevice.h"
#include "RTUVKMesh.h"

void RTUVKGraphicsContext::Init(IRTUGraphicsDevice* _device)
{
	RTUVKGraphicsDevice* device = static_cast<RTUVKGraphicsDevice*>(_device);
	m_refDevice = device->m_device;
	m_refCommandPool = device->m_commandPool;
	/* Create the command buffer from the command pool */
	VkCommandBufferAllocateInfo cmd = {};
	cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd.pNext = NULL;
	cmd.commandPool = device->m_commandPool;
	cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd.commandBufferCount = 1;

	ThrowIfFailed(vkAllocateCommandBuffers(device->m_device, &cmd, &m_commandBuffer));
}

void RTUVKGraphicsContext::SetHeapDefault()
{
}

void RTUVKGraphicsContext::SetGraphicsRootSignature(IRTURootSignature* _signature)
{
}

void RTUVKGraphicsContext::SetRenderTargetScreen(IRTURenderTexture2D* _dsv)
{
}

void RTUVKGraphicsContext::SetRenderTargetDSV(IRTURenderTexture2D* _dsv)
{
}

void RTUVKGraphicsContext::SetRenderTargetRTVDSV(IRTURenderTexture2D** _rtv, int rtvCount, IRTURenderTexture2D* _dsv)
{
}

void RTUVKGraphicsContext::SetRenderTargetRTV(IRTURenderTexture2D** _rtv, int rtvCount)
{
}

void RTUVKGraphicsContext::SetPipelineState(IRTUPipelineState* _pipelineState, RTUPipelineStateDesc* desc)
{
}

void RTUVKGraphicsContext::SetMesh(IRTUMesh* _mesh)
{
}

void RTUVKGraphicsContext::SetCBVR(IRTUCBuffer* _buffer, int offset256, int size256, int slot)
{
}

void RTUVKGraphicsContext::SetCBVR(IRTUSBuffer* _buffer, int offset256, int size256, int slot)
{
}

void RTUVKGraphicsContext::SetSRVT(IRTUTexture2D* _texture, int slot)
{
}

void RTUVKGraphicsContext::SetSRVT(IRTURenderTexture2D* _texture, int slot)
{
}

void RTUVKGraphicsContext::DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)
{
}

void RTUVKGraphicsContext::DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation)
{
}

void RTUVKGraphicsContext::UpdateBuffer(IRTUCBuffer* _buffer, void* data, int dataSize)
{
}

void RTUVKGraphicsContext::UpdateBuffer(IRTUSBuffer* _buffer, void* data, int dataSize)
{
}

void RTUVKGraphicsContext::UploadMesh(IRTUMesh* _mesh, RTUMeshLoader* loader)
{
	RTUVKMesh* mesh = static_cast<RTUVKMesh*>(_mesh);
	mesh->m_states = RTU_STATES::RTU_STATES_LOADED;
}

void RTUVKGraphicsContext::UploadTexture(IRTUTexture2D* _texture, RTUTexture2DLoader* loader)
{
}

void RTUVKGraphicsContext::ClearScreen(const float* color)
{
}

void RTUVKGraphicsContext::ClearRTV(IRTURenderTexture2D* rtv, const float* color)
{
}

void RTUVKGraphicsContext::ClearDSV(IRTURenderTexture2D* dsv)
{
}

void RTUVKGraphicsContext::ScreenBeginRender()
{
}

void RTUVKGraphicsContext::ScreenEndRender()
{
}

void RTUVKGraphicsContext::BeginCommand(IRTUGraphicsDevice* _device)
{
}

void RTUVKGraphicsContext::EndCommand()
{
}

void RTUVKGraphicsContext::ExecuteCommand(IRTUGraphicsDevice* _device)
{
}

RTUVKGraphicsContext::~RTUVKGraphicsContext()
{
	vkFreeCommandBuffers(m_refDevice, m_refCommandPool, 1, &m_commandBuffer);
}
