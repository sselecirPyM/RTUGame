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
#include "RTUDX12GraphicsContext.h"
#include "RTUDX12RootSignature.h"
#include "RTUDX12PipelineState.h"
#include "RTUDX12CBuffer.h"
#include "RTUDX12SBuffer.h"
#include "RTUDX12Mesh.h"
#include "RTUDX12Texture2D.h"
#include "RTUDX12RenderTexture2D.h"
#include "../../DXUtil.h"
#include<DirectXTex.h>


static const D3D12_INPUT_ELEMENT_DESC _inputLayoutPosOnly[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
static const D3D12_INPUT_ELEMENT_DESC _inputLayoutPNUVT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
inline D3D12_BLEND_DESC _BlendDescAlpha()
{
	D3D12_BLEND_DESC blendDescAlpha = {};
	blendDescAlpha.RenderTarget[0].BlendEnable = true;
	blendDescAlpha.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blendDescAlpha;
}
inline D3D12_BLEND_DESC _BlendDescAdd()
{
	D3D12_BLEND_DESC blendDescAlpha = {};
	blendDescAlpha.RenderTarget[0].BlendEnable = true;
	blendDescAlpha.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return blendDescAlpha;
}

void _CommonBarrier(D3D12_RESOURCE_STATES& stateMember, D3D12_RESOURCE_STATES stateTest, ID3D12Resource* resource, ID3D12GraphicsCommandList* commandList)
{
	if (stateMember != stateTest)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, stateMember, stateTest);
		commandList->ResourceBarrier(1, &barrier);
		stateMember = stateTest;
	}
}

void RTUDX12GraphicsContext::Init(IRTUGraphicsDevice* _device)
{
	m_device = static_cast<RTUDX12GraphicsDevice*>(_device);
	ThrowIfFailed(m_device->m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_device->GetCommandAllocator(), nullptr, IID_PPV_ARGS(&m_commandList)));
	NAME_D3D12_OBJECT(m_commandList);
	ThrowIfFailed(m_commandList->Close());
}

void RTUDX12GraphicsContext::SetHeapDefault(IRTUGraphicsDevice* _device)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	ID3D12DescriptorHeap* heaps[] = { device->m_cbvsrvuavHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
}

void RTUDX12GraphicsContext::SetGraphicsRootSignature(IRTURootSignature* _signature)
{
	RTUDX12RootSignature* signature = static_cast<RTUDX12RootSignature*> (_signature);
	m_commandList->SetGraphicsRootSignature(signature->m_rootSignature.Get());
}

void RTUDX12GraphicsContext::SetRenderTargetScreen(IRTUGraphicsDevice* _device, IRTURenderTexture2D* _dsv)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D* dsv = static_cast<RTUDX12RenderTexture2D*>(_dsv);
	D3D12_VIEWPORT viewport = device->m_screenViewport;
	D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);
	auto screenRenderTargetView = device->GetRenderTargetView();
	if (dsv != nullptr)
	{
		_CommonBarrier(dsv->m_previousResourceStates, D3D12_RESOURCE_STATE_DEPTH_WRITE, dsv->m_texture.Get(), m_commandList.Get());

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsv->m_dsvRef, device->m_dsvDescriptorSize);
		m_commandList->OMSetRenderTargets(1, &screenRenderTargetView, false, &dsvHandle);
	}
	else
	{
		m_commandList->OMSetRenderTargets(1, &screenRenderTargetView, false, nullptr);
	}
}

void RTUDX12GraphicsContext::SetRenderTargetDSV(IRTUGraphicsDevice* _device, IRTURenderTexture2D* _dsv)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D* dsv = static_cast<RTUDX12RenderTexture2D*>(_dsv);
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		dsv[0].m_width,
		dsv[0].m_height
	);
	D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);
	_CommonBarrier(dsv->m_previousResourceStates, D3D12_RESOURCE_STATE_DEPTH_WRITE, dsv->m_texture.Get(), m_commandList.Get());

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsv->m_dsvRef, device->m_dsvDescriptorSize);

	m_commandList->OMSetRenderTargets(0, nullptr, false, &dsvHandle);
}

void RTUDX12GraphicsContext::SetRenderTargetRTVDSV(IRTUGraphicsDevice* _device, IRTURenderTexture2D** _rtv, int rtvCount, IRTURenderTexture2D* _dsv)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D** rtv = (RTUDX12RenderTexture2D**)(_rtv);
	RTUDX12RenderTexture2D* dsv = static_cast<RTUDX12RenderTexture2D*>(_dsv);
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		(*rtv[0]).m_width,
		(*rtv[0]).m_height
	);
	D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);
	for (int i = 0; i < rtvCount; i++)
		_CommonBarrier((*rtv[i]).m_previousResourceStates, D3D12_RESOURCE_STATE_RENDER_TARGET, (*rtv[i]).m_texture.Get(), m_commandList.Get());
	_CommonBarrier(dsv->m_previousResourceStates, D3D12_RESOURCE_STATE_DEPTH_WRITE, dsv->m_texture.Get(), m_commandList.Get());

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsv->m_dsvRef, device->m_dsvDescriptorSize);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvs1[8] = {};
	for (int i = 0; i < rtvCount; i++)
	{
		rtvs1[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), (*rtv[i]).m_rtvRef, device->m_rtvDescriptorSize);
	}
	m_commandList->OMSetRenderTargets(rtvCount, rtvs1, false, &dsvHandle);
}

void RTUDX12GraphicsContext::SetRenderTargetRTV(IRTUGraphicsDevice* _device, IRTURenderTexture2D** _rtv, int rtvCount)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D** rtv = (RTUDX12RenderTexture2D**)(_rtv);
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		(*rtv[0]).m_width,
		(*rtv[0]).m_height
	);
	D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_commandList->RSSetViewports(1, &viewport);
	m_commandList->RSSetScissorRects(1, &scissorRect);
	for (int i = 0; i < rtvCount; i++)
		_CommonBarrier((*rtv[i]).m_previousResourceStates, D3D12_RESOURCE_STATE_RENDER_TARGET, (*rtv[i]).m_texture.Get(), m_commandList.Get());

	D3D12_CPU_DESCRIPTOR_HANDLE rtvs1[8] = {};
	for (int i = 0; i < rtvCount; i++)
	{
		rtvs1[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), (*rtv[i]).m_rtvRef, device->m_rtvDescriptorSize);
	}
	m_commandList->OMSetRenderTargets(rtvCount, rtvs1, false, nullptr);
}

void RTUDX12GraphicsContext::SetPipelineState(IRTUGraphicsDevice* _device, IRTURootSignature* _rootSignature, IRTUPipelineState* _pipelineState, RTUPipelineStateDesc* desc)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RootSignature* rootSignature = static_cast<RTUDX12RootSignature*>(_rootSignature);
	RTUDX12PipelineState* pipelineState = static_cast<RTUDX12PipelineState*>(_pipelineState);

	ComPtr<ID3D12PipelineState> state1 = pipelineState->GetPipelineState(desc);
	if (!state1)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc = {};
		stateDesc.pRootSignature = rootSignature->m_rootSignature.Get();
		if (desc->eInputLayout == RTU_INPUT_LAYOUT_POSITION_ONLY)
			stateDesc.InputLayout = { _inputLayoutPosOnly, _countof(_inputLayoutPosOnly) };
		else if (desc->eInputLayout == RTU_INPUT_LAYOUT_P_N_UV_T)
			stateDesc.InputLayout = { _inputLayoutPNUVT, _countof(_inputLayoutPNUVT) };
		stateDesc.VS = CD3DX12_SHADER_BYTECODE(pipelineState->m_vertexShader.data(), pipelineState->m_vertexShader.size());
		stateDesc.GS = CD3DX12_SHADER_BYTECODE(pipelineState->m_geometryShader.data(), pipelineState->m_geometryShader.size());
		stateDesc.PS = CD3DX12_SHADER_BYTECODE(pipelineState->m_pixelShader.data(), pipelineState->m_pixelShader.size());
		if (desc->blendmode == RTU_BLEND_MODE_ALPHA)
			stateDesc.BlendState = _BlendDescAlpha();
		else if (desc->blendmode == RTU_BLEND_MODE_ADD)
			stateDesc.BlendState = _BlendDescAdd();
		else
			stateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		if (desc->dsvFormat != DXGI_FORMAT_UNKNOWN)
		{
			stateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			stateDesc.DSVFormat = desc->dsvFormat;
		}
		stateDesc.SampleMask = UINT_MAX;
		if (desc->primitiveType == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		else if (desc->primitiveType == D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
			stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		else if (desc->primitiveType == D3D_PRIMITIVE_TOPOLOGY_LINELIST)
			stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		stateDesc.SampleDesc.Count = 1;
		stateDesc.NumRenderTargets = desc->renderTargetCount;
		for (int i = 0; i < desc->renderTargetCount; i++)
		{
			stateDesc.RTVFormats[i] = desc->rtvFormat;
		}
		stateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, false, desc->depthBias, 0.0f, 0.0f, true, false, false, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
		ThrowIfFailed(device->GetD3DDevice()->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(&state1)));

		pipelineState->m_psoDescs.emplace_back(*desc);
		pipelineState->m_pipelineStates.emplace_back(state1);
	}
	else
	{
		int dosomething = 0;
	}
	m_commandList->SetPipelineState(state1.Get());
	m_commandList->IASetPrimitiveTopology(pipelineState->m_primitiveType);
}

void RTUDX12GraphicsContext::SetMesh(IRTUMesh* _mesh)
{
	RTUDX12Mesh* mesh = static_cast<RTUDX12Mesh*> (_mesh);
	m_commandList->IASetVertexBuffers(0, 1, &mesh->m_vertexBufferView);
	m_commandList->IASetIndexBuffer(&mesh->m_indexBufferView);
}

void RTUDX12GraphicsContext::SetCBVR(IRTUCBuffer* _buffer, int syncIndex, int offset256, int size256, int slot)
{
	RTUDX12CBuffer* buffer = static_cast<RTUDX12CBuffer*>(_buffer);
	m_commandList->SetGraphicsRootConstantBufferView(slot, buffer->m_buffer->GetGPUVirtualAddress() + syncIndex * buffer->m_size + offset256 * 256);
}

void RTUDX12GraphicsContext::SetCBVR(IRTUSBuffer* _buffer, int syncIndex, int offset256, int size256, int slot)
{
	RTUDX12SBuffer* buffer = static_cast<RTUDX12SBuffer*>(_buffer);
	m_commandList->SetGraphicsRootConstantBufferView(slot, buffer->m_buffer->GetGPUVirtualAddress() + offset256 * 256);
}

void RTUDX12GraphicsContext::SetSRVT(IRTUTexture2D* _texture, int slot)
{
	RTUDX12GraphicsDevice*& device = m_device;
	RTUDX12Texture2D* texture = static_cast<RTUDX12Texture2D*>(_texture);
	m_commandList->SetGraphicsRootDescriptorTable(slot, CD3DX12_GPU_DESCRIPTOR_HANDLE(device->m_cbvsrvuavHeap->GetGPUDescriptorHandleForHeapStart(), texture->m_srvRef, device->m_cbvsrvuavDescriptorSize));
}

void RTUDX12GraphicsContext::SetSRVT(IRTURenderTexture2D* _texture, int slot)
{
	RTUDX12GraphicsDevice*& device = m_device;
	RTUDX12RenderTexture2D* texture = static_cast<RTUDX12RenderTexture2D*>(_texture);
	_CommonBarrier(texture->m_previousResourceStates, D3D12_RESOURCE_STATE_GENERIC_READ, texture->m_texture.Get(), m_commandList.Get());
	m_commandList->SetGraphicsRootDescriptorTable(slot, CD3DX12_GPU_DESCRIPTOR_HANDLE(device->m_cbvsrvuavHeap->GetGPUDescriptorHandleForHeapStart(), texture->m_srvRef, device->m_cbvsrvuavDescriptorSize));
}

void RTUDX12GraphicsContext::DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)
{
	m_commandList->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

void RTUDX12GraphicsContext::DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation)
{
	m_commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void RTUDX12GraphicsContext::UpdateBuffer(IRTUCBuffer* _buffer, int syncIndex, void* data, int dataSize)
{
	RTUDX12CBuffer* buffer = static_cast<RTUDX12CBuffer*>(_buffer);
	memcpy(buffer->m_mappedBuffer + buffer->m_size * syncIndex, data, dataSize);
}

void RTUDX12GraphicsContext::UpdateBuffer(IRTUSBuffer* _buffer, int syncIndex, void* data, int dataSize)
{
	RTUDX12SBuffer* buffer = static_cast<RTUDX12SBuffer*>(_buffer);
	assert(dataSize <= buffer->m_size);
	D3D12_RANGE readRange = CD3DX12_RANGE(0, 0);
	char* mapped = nullptr;

	buffer->m_bufferUpload[syncIndex]->Map(0, &readRange, reinterpret_cast<void**>(&mapped));
	memcpy(mapped, data, dataSize);
	buffer->m_bufferUpload[syncIndex]->Unmap(0, nullptr);

	auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
	m_commandList->ResourceBarrier(1, &barrier1);
	m_commandList->CopyBufferRegion(buffer->m_buffer.Get(), 0, buffer->m_bufferUpload[syncIndex].Get(), 0, dataSize);
	barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(buffer->m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	m_commandList->ResourceBarrier(1, &barrier1);
}

void RTUDX12GraphicsContext::UploadMesh(IRTUGraphicsDevice* _device, IRTUMesh* _mesh, RTUMeshLoader* loader)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12Mesh* mesh = static_cast<RTUDX12Mesh*> (_mesh);
	auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
#define _TEST_SINGLE_MESH_BUFFER
#ifndef _TEST_SINGLE_MESH_BUFFER
	auto bufferDescVertexBuffer = CD3DX12_RESOURCE_DESC::Buffer(loader->m_vertexData.size());
	auto bufferDescIndexBuffer = CD3DX12_RESOURCE_DESC::Buffer(loader->m_indexData.size());
	auto bufferDescUploaderBuffer = CD3DX12_RESOURCE_DESC::Buffer(loader->m_vertexData.size() + loader->m_indexData.size());
	ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDescVertexBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mesh->m_vertex)
	));
	NAME_D3D12_OBJECT(mesh->m_vertex);
	ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDescIndexBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mesh->m_index)
	));
	NAME_D3D12_OBJECT(mesh->m_index);
	ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDescUploaderBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&loader->m_vertex)
	));
	NAME_D3D12_OBJECT(loader->m_vertex);

	char* mapped = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(loader->m_vertex->Map(0, &readRange, reinterpret_cast<void**>(&mapped)));
	size_t offset = 0;

	memcpy(mapped, loader->m_vertexData.data(), loader->m_vertexData.size());
	m_commandList->CopyBufferRegion(mesh->m_vertex.Get(), 0, loader->m_vertex.Get(), offset, loader->m_vertexData.size());
	offset += loader->m_vertexData.size();

	memcpy(mapped + offset, loader->m_indexData.data(), loader->m_indexData.size());
	m_commandList->CopyBufferRegion(mesh->m_index.Get(), 0, loader->m_vertex.Get(), offset, loader->m_indexData.size());
	offset += loader->m_indexData.size();

	loader->m_vertex->Unmap(0, nullptr);
	mapped = nullptr;


	D3D12_RESOURCE_BARRIER barriers[] = { CD3DX12_RESOURCE_BARRIER::Transition(mesh->m_vertex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ),
	 CD3DX12_RESOURCE_BARRIER::Transition(mesh->m_index.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ) };
	m_commandList->ResourceBarrier(2, barriers);

	mesh->m_vertexBufferView.BufferLocation = mesh->m_vertex->GetGPUVirtualAddress();
	mesh->m_vertexBufferView.StrideInBytes = loader->m_vertexStride;
	mesh->m_vertexBufferView.SizeInBytes = loader->m_vertexData.size();

	mesh->m_indexBufferView.BufferLocation = mesh->m_index->GetGPUVirtualAddress();
	mesh->m_indexBufferView.SizeInBytes = loader->m_indexData.size();
	mesh->m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
#else
	size_t vertexSizeA = (loader->m_vertexData.size() + 255) & ~255;
	size_t indexSizeA = (loader->m_indexData.size() + 255) & ~255;
	auto bufferDescVertexBuffer = CD3DX12_RESOURCE_DESC::Buffer(vertexSizeA + indexSizeA);
	auto bufferDescUploaderBuffer = CD3DX12_RESOURCE_DESC::Buffer(vertexSizeA + indexSizeA);

	ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDescVertexBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mesh->m_vertex)
	));
	NAME_D3D12_OBJECT(mesh->m_vertex);
	ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDescUploaderBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&loader->m_vertex)
	));
	NAME_D3D12_OBJECT(loader->m_vertex);

	char* mapped = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(loader->m_vertex->Map(0, &readRange, reinterpret_cast<void**>(&mapped)));
	size_t offset = 0;

	memcpy(mapped, loader->m_vertexData.data(), loader->m_vertexData.size());
	offset += vertexSizeA;
	memcpy(mapped + offset, loader->m_indexData.data(), loader->m_indexData.size());
	offset += indexSizeA;
	m_commandList->CopyBufferRegion(mesh->m_vertex.Get(), 0, loader->m_vertex.Get(), 0, vertexSizeA + indexSizeA);

	loader->m_vertex->Unmap(0, nullptr);
	mapped = nullptr;


	D3D12_RESOURCE_BARRIER barriers[] = { CD3DX12_RESOURCE_BARRIER::Transition(mesh->m_vertex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ) };
	m_commandList->ResourceBarrier(1, barriers);

	mesh->m_vertexBufferView.BufferLocation = mesh->m_vertex->GetGPUVirtualAddress();
	mesh->m_vertexBufferView.StrideInBytes = loader->m_vertexStride;
	mesh->m_vertexBufferView.SizeInBytes = loader->m_vertexData.size();

	mesh->m_indexBufferView.BufferLocation = mesh->m_vertex->GetGPUVirtualAddress() + vertexSizeA;
	mesh->m_indexBufferView.SizeInBytes = loader->m_indexData.size();
	mesh->m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
#endif
	mesh->m_states = RTU_STATES::RTU_STATES_LOADED;
}

void RTUDX12GraphicsContext::UploadTexture(IRTUGraphicsDevice* _device, IRTUTexture2D* _texture, RTUTexture2DLoader* loader)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12Texture2D* texture = static_cast<RTUDX12Texture2D*>(_texture);
#ifdef _DEBUG
	texture->dbg_width = loader->m_width;
	texture->dbg_height = loader->m_height;
	texture->dbg_mipLevels = loader->m_mipLevels;
#endif
	auto d3dDevice = device->GetD3DDevice();

	auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = loader->m_mipLevels;
	textureDesc.Format = loader->m_format;
	textureDesc.Width = loader->m_width;
	textureDesc.Height = loader->m_height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texture->m_texture)));
	NAME_D3D12_OBJECT(texture->m_texture);

	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(loader->m_data.size());
	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&loader->m_texture)));

	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	subresources.reserve(textureDesc.MipLevels);

	byte* pdata = loader->m_data.data();
	int bitsPerPixel = DirectX::BitsPerPixel(textureDesc.Format);
	UINT width = textureDesc.Width;
	UINT height = textureDesc.Height;
	for (int i = 0; i < textureDesc.MipLevels; i++)
	{
		D3D12_SUBRESOURCE_DATA subresourcedata = {};
		subresourcedata.pData = pdata;
		subresourcedata.RowPitch = width * bitsPerPixel / 8;
		subresourcedata.SlicePitch = width * height * bitsPerPixel / 8;
		pdata += width * height * bitsPerPixel / 8;

		subresources.push_back(subresourcedata);
		width /= 2;
		height /= 2;
	}

	UpdateSubresources(m_commandList.Get(), texture->m_texture.Get(), loader->m_texture.Get(), 0, 0, textureDesc.MipLevels, subresources.data());

	auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(texture->m_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	m_commandList->ResourceBarrier(1, &barrier1);
	texture->m_srvRef = _InterlockedIncrement(&device->m_cbvsrvuavHeapAllocCount) - 1;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	d3dDevice->CreateShaderResourceView(texture->m_texture.Get(), &srvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_cbvsrvuavHeap->GetCPUDescriptorHandleForHeapStart(), device->m_cbvsrvuavDescriptorSize * texture->m_srvRef));
	texture->m_states = RTU_STATES::RTU_STATES_LOADED;
}

void RTUDX12GraphicsContext::ClearScreen(IRTUGraphicsDevice* _device, const float* color)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	m_commandList->ClearRenderTargetView(device->GetRenderTargetView(), color, 0, nullptr);
}

void RTUDX12GraphicsContext::ClearRTV(IRTUGraphicsDevice* _device, IRTURenderTexture2D* _rtv, const float* color)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D* rtv = static_cast<RTUDX12RenderTexture2D*>(_rtv);
	_CommonBarrier(rtv->m_previousResourceStates, D3D12_RESOURCE_STATE_RENDER_TARGET, rtv->m_texture.Get(), m_commandList.Get());
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), rtv->m_rtvRef, device->m_rtvDescriptorSize);
	m_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
}

void RTUDX12GraphicsContext::ClearDSV(IRTUGraphicsDevice* _device, IRTURenderTexture2D* _dsv)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	RTUDX12RenderTexture2D* dsv = static_cast<RTUDX12RenderTexture2D*>(_dsv);
	_CommonBarrier(dsv->m_previousResourceStates, D3D12_RESOURCE_STATE_DEPTH_WRITE, dsv->m_texture.Get(), m_commandList.Get());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsv->m_dsvRef, device->m_dsvDescriptorSize);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RTUDX12GraphicsContext::ScreenBeginRender()
{
	RTUDX12GraphicsDevice*& device = m_device;
	auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(device->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier1);
}

void RTUDX12GraphicsContext::ScreenEndRender()
{
	RTUDX12GraphicsDevice*& device = m_device;
	auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(device->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrier1);
}

void RTUDX12GraphicsContext::BeginCommand(IRTUGraphicsDevice* _device)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	ThrowIfFailed(m_commandList->Reset(device->GetCommandAllocator(), nullptr));
}

void RTUDX12GraphicsContext::EndCommand()
{
	ThrowIfFailed(m_commandList->Close());
}

void RTUDX12GraphicsContext::ExecuteCommand(IRTUGraphicsDevice* _device)
{
	RTUDX12GraphicsDevice* device = static_cast<RTUDX12GraphicsDevice*>(_device);
	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	device->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
}