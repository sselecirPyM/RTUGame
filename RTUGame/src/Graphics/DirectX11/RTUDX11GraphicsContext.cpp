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
#include "RTUDX11GraphicsContext.h"
#include "RTUDX11GraphicsDevice.h"
#include "RTUDX11RenderTexture2D.h"
#include "RTUDX11Texture2D.h"
#include "RTUDX11RootSignature.h"
#include "RTUDX11PipelineState.h"
#include "RTUDX11Mesh.h"
#include "RTUDX11CBuffer.h"
#include "RTUDX11SBuffer.h"
#include "../../DXUtil.h"
#include <DirectXTex.h>


static const D3D11_INPUT_ELEMENT_DESC _inputLayoutPosOnly[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static const D3D11_INPUT_ELEMENT_DESC _inputLayoutPNUVT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static const D3D11_INPUT_ELEMENT_DESC _inputLayoutIMGUI[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

inline D3D11_BLEND_DESC _BlendDescAlpha()
{
	D3D11_BLEND_DESC blendDescAlpha = {};
	blendDescAlpha.RenderTarget[0].BlendEnable = true;
	blendDescAlpha.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	return blendDescAlpha;
}
inline D3D11_BLEND_DESC _BlendDescAdd()
{
	D3D11_BLEND_DESC blendDescAlpha = {};
	blendDescAlpha.RenderTarget[0].BlendEnable = true;
	blendDescAlpha.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	return blendDescAlpha;
}

void RTUDX11GraphicsContext::Init(IRTUGraphicsDevice* _device)
{
	RTUDX11GraphicsDevice* device = static_cast<RTUDX11GraphicsDevice*>(_device);
	m_device = device;
#ifdef D3D11_MULTIPLE_THREAD
	device->m_d3dDevice->CreateDeferredContext3(0, &m_d3dContext);
#else
	m_d3dContext = device->m_d3dContext;
#endif
}

void RTUDX11GraphicsContext::SetHeapDefault()
{
}

void RTUDX11GraphicsContext::SetGraphicsRootSignature(IRTURootSignature* _signature)
{
	m_currentRootSignature = static_cast<RTUDX11RootSignature*>(_signature);
}

void RTUDX11GraphicsContext::SetRenderTargetScreen(IRTURenderTexture2D* _dsv)
{
	RTUDX11RenderTexture2D* dsv = static_cast<RTUDX11RenderTexture2D*>(_dsv);
	ID3D11RenderTargetView* const targets[1] = { m_device->GetBackBufferRenderTargetView() };
	D3D11_VIEWPORT viewport = m_device->m_screenViewport;
	D3D11_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_d3dContext->RSSetViewports(1, &viewport);
	m_d3dContext->RSSetScissorRects(1, &scissorRect);
	m_d3dContext->OMSetRenderTargets(1, targets, dsv ? dsv->m_depthStencilView.Get() : nullptr);
}

void RTUDX11GraphicsContext::SetRenderTargetDSV(IRTURenderTexture2D* _dsv)
{
	RTUDX11RenderTexture2D* dsv = static_cast<RTUDX11RenderTexture2D*>(_dsv);
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		dsv->m_width,
		dsv->m_height
	);
	D3D11_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_d3dContext->RSSetViewports(1, &viewport);
	m_d3dContext->RSSetScissorRects(1, &scissorRect);
	m_d3dContext->OMSetRenderTargets(0, nullptr, dsv->m_depthStencilView.Get());
}

void RTUDX11GraphicsContext::SetRenderTargetRTVDSV(IRTURenderTexture2D** _rtv, int rtvCount, IRTURenderTexture2D* _dsv)
{
	RTUDX11RenderTexture2D** rtv = (RTUDX11RenderTexture2D**)(_rtv);
	RTUDX11RenderTexture2D* dsv = static_cast<RTUDX11RenderTexture2D*>(_dsv);
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		(*rtv[0]).m_width,
		(*rtv[0]).m_height
	);
	D3D11_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_d3dContext->RSSetViewports(1, &viewport);
	m_d3dContext->RSSetScissorRects(1, &scissorRect);

	ID3D11RenderTargetView* rtvs1[8] = {};
	for (int i = 0; i < rtvCount; i++)
	{
		rtvs1[i] = (*rtv[i]).m_renderTargetView.Get();
	}
	m_d3dContext->OMSetRenderTargets(rtvCount, rtvs1, dsv->m_depthStencilView.Get());
}

void RTUDX11GraphicsContext::SetRenderTargetRTV(IRTURenderTexture2D** _rtv, int rtvCount)
{
	RTUDX11RenderTexture2D** rtv = (RTUDX11RenderTexture2D**)(_rtv);
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		(*rtv[0]).m_width,
		(*rtv[0]).m_height
	);
	D3D11_RECT scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
	m_d3dContext->RSSetViewports(1, &viewport);
	m_d3dContext->RSSetScissorRects(1, &scissorRect);

	ID3D11RenderTargetView* rtvs1[8] = {};
	for (int i = 0; i < rtvCount; i++)
	{
		rtvs1[i] = (*rtv[i]).m_renderTargetView.Get();
	}
	m_d3dContext->OMSetRenderTargets(rtvCount, rtvs1, nullptr);
}

void RTUDX11GraphicsContext::SetPipelineState(IRTUPipelineState* _pipelineState, RTUPipelineStateDesc* desc)
{
	RTUDX11PipelineState* pipelineState = static_cast<RTUDX11PipelineState*>(_pipelineState);

	if (!pipelineState->m_vertexShader)
	{
		if (pipelineState->m_vsData.size())
			ThrowIfFailed(m_device->m_d3dDevice->CreateVertexShader(pipelineState->m_vsData.data(), pipelineState->m_vsData.size(), nullptr, &pipelineState->m_vertexShader));
		if (pipelineState->m_psData.size())
			ThrowIfFailed(m_device->m_d3dDevice->CreatePixelShader(pipelineState->m_psData.data(), pipelineState->m_psData.size(), nullptr, &pipelineState->m_pixelShader));

		if (desc->eInputLayout == RTU_INPUT_LAYOUT::POSITION_ONLY)
			ThrowIfFailed(m_device->m_d3dDevice->CreateInputLayout(_inputLayoutPosOnly, _countof(_inputLayoutPosOnly), pipelineState->m_vsData.data(), pipelineState->m_vsData.size(), &pipelineState->m_inputLayout));
		else if (desc->eInputLayout == RTU_INPUT_LAYOUT::P_N_UV_T)
			ThrowIfFailed(m_device->m_d3dDevice->CreateInputLayout(_inputLayoutPNUVT, _countof(_inputLayoutPNUVT), pipelineState->m_vsData.data(), pipelineState->m_vsData.size(), &pipelineState->m_inputLayout));
		else if (desc->eInputLayout == RTU_INPUT_LAYOUT::IMGUI)
			ThrowIfFailed(m_device->m_d3dDevice->CreateInputLayout(_inputLayoutIMGUI, _countof(_inputLayoutIMGUI), pipelineState->m_vsData.data(), pipelineState->m_vsData.size(), &pipelineState->m_inputLayout));
	}

	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	rasterizerDesc.DepthBias = desc->depthBias;
	m_device->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &pipelineState->m_rasterizerState);

	D3D11_BLEND_DESC blendDesc = {};
	if (desc->blendmode == RTU_BLEND_MODE_ALPHA)
		blendDesc = _BlendDescAlpha();
	else if (desc->blendmode == RTU_BLEND_MODE_ADD)
		blendDesc = _BlendDescAdd();
	else
		blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	m_device->m_d3dDevice->CreateBlendState(&blendDesc, &pipelineState->m_blendState);

	m_d3dContext->IASetInputLayout(pipelineState->m_inputLayout.Get());
	m_d3dContext->IASetPrimitiveTopology(desc->primitiveType);
	m_d3dContext->VSSetShader(pipelineState->m_vertexShader.Get(), nullptr, 0);
	m_d3dContext->PSSetShader(pipelineState->m_pixelShader.Get(), nullptr, 0);
	m_d3dContext->RSSetState(pipelineState->m_rasterizerState.Get());
	float factory[4] = { 0.0f,0.0f,0.0f,0.0f };
	m_d3dContext->OMSetBlendState(pipelineState->m_blendState.Get(), factory, 0xffffffff);
}

void RTUDX11GraphicsContext::SetMesh(IRTUMesh* _mesh)
{
	RTUDX11Mesh* mesh = static_cast<RTUDX11Mesh*>(_mesh);
	UINT ofs = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(), &mesh->m_vertexStride, &ofs);
	m_d3dContext->IASetIndexBuffer(mesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void RTUDX11GraphicsContext::SetCBVR(IRTUCBuffer* _buffer, int offset256, int size256, int slot)
{
	UINT ofs = 16 * offset256;
	UINT numConst = 16 * size256;
	RTUDX11CBuffer* buffer = static_cast<RTUDX11CBuffer*>(_buffer);
	m_d3dContext->VSSetConstantBuffers1(slot, 1, buffer->m_buffer.GetAddressOf(), &ofs, &numConst);
	m_d3dContext->PSSetConstantBuffers1(slot, 1, buffer->m_buffer.GetAddressOf(), &ofs, &numConst);
}

void RTUDX11GraphicsContext::SetCBVR(IRTUSBuffer* _buffer, int offset256, int size256, int slot)
{
	UINT ofs = 16 * offset256;
	UINT numConst = 16 * size256;
	RTUDX11SBuffer* buffer = static_cast<RTUDX11SBuffer*>(_buffer);
	m_d3dContext->VSSetConstantBuffers1(slot, 1, buffer->m_buffer.GetAddressOf(), &ofs, &numConst);
	m_d3dContext->PSSetConstantBuffers1(slot, 1, buffer->m_buffer.GetAddressOf(), &ofs, &numConst);
}

void RTUDX11GraphicsContext::SetSRVT(IRTUTexture2D* _texture, int slot)
{
	RTUDX11Texture2D* texture = static_cast<RTUDX11Texture2D*>(_texture);
	m_d3dContext->PSSetShaderResources(slot, 1, texture->m_shaderResourceView.GetAddressOf());
}

void RTUDX11GraphicsContext::SetSRVT(IRTURenderTexture2D* _texture, int slot)
{
	RTUDX11RenderTexture2D* texture = static_cast<RTUDX11RenderTexture2D*>(_texture);
	m_d3dContext->PSSetShaderResources(slot, 1, texture->m_shaderResourceView.GetAddressOf());
}

void RTUDX11GraphicsContext::DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)
{
	m_d3dContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void RTUDX11GraphicsContext::DrawIndexedInstanced(int indexCount, int startIndexLocation, int baseVertexLocation, int instanceCount, int startInstanceLocation)
{
	m_d3dContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void RTUDX11GraphicsContext::UpdateBuffer(IRTUCBuffer* _buffer, void* data, int dataSize)
{
	RTUDX11CBuffer* buffer = static_cast<RTUDX11CBuffer*>(_buffer);
	D3D11_MAPPED_SUBRESOURCE mappedSubResource = {};
	m_d3dContext->Map(buffer->m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, data, dataSize);
	m_d3dContext->Unmap(buffer->m_buffer.Get(), 0);
}

void RTUDX11GraphicsContext::UpdateBuffer(IRTUSBuffer* _buffer, void* data, int dataSize)
{
	RTUDX11SBuffer* buffer = static_cast<RTUDX11SBuffer*>(_buffer);
	m_d3dContext->UpdateSubresource(buffer->m_buffer.Get(), 0, nullptr, data, 0, 0);
}

void RTUDX11GraphicsContext::UploadMesh(IRTUMesh* _mesh, RTUMeshLoader* loader)
{
	RTUDX11Mesh* mesh = static_cast<RTUDX11Mesh*>(_mesh);
	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = loader->m_vertexData.data();
	CD3D11_BUFFER_DESC vertexBufferDesc(loader->m_vertexData.size(), D3D11_BIND_VERTEX_BUFFER);
	ThrowIfFailed(m_device->m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &mesh->m_vertexBuffer));


	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = loader->m_indexData.data();
	CD3D11_BUFFER_DESC indexBufferDesc(loader->m_indexData.size(), D3D11_BIND_INDEX_BUFFER);
	ThrowIfFailed(m_device->m_d3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &mesh->m_indexBuffer));

	mesh->m_indexCount = loader->m_indexData.size() / 4;
	mesh->m_vertexStride = loader->m_vertexStride;
}

void RTUDX11GraphicsContext::UploadTexture(IRTUTexture2D* _texture, RTUTexture2DLoader* loader)
{
	RTUDX11Texture2D* texture = static_cast<RTUDX11Texture2D*>(_texture);
	D3D11_TEXTURE2D_DESC tex2DDesc = {};
	tex2DDesc.Width = loader->m_width;
	tex2DDesc.Height = loader->m_height;
	tex2DDesc.Format = loader->m_format;
	tex2DDesc.ArraySize = 1;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tex2DDesc.MiscFlags = 0;
	tex2DDesc.MipLevels = 1;
	UINT bytesPerPixel = DirectX::BitsPerPixel(tex2DDesc.Format) / 8;

	D3D11_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = loader->m_data.data();
	subresourceData.SysMemPitch = loader->m_width * bytesPerPixel;
	subresourceData.SysMemSlicePitch = loader->m_width * loader->m_height * bytesPerPixel;

	ThrowIfFailed(m_device->m_d3dDevice->CreateTexture2D(&tex2DDesc, &subresourceData, &texture->m_texture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(texture->m_texture2D.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, loader->m_format);
	ThrowIfFailed(m_device->m_d3dDevice->CreateShaderResourceView(
		texture->m_texture2D.Get(),
		&shaderResourceViewDesc,
		&texture->m_shaderResourceView
	));
}

void RTUDX11GraphicsContext::ClearScreen(const float* color)
{
	m_d3dContext->ClearRenderTargetView(m_device->GetBackBufferRenderTargetView(), color);
}

void RTUDX11GraphicsContext::ClearRTV(IRTURenderTexture2D* _rtv, const float* color)
{
	RTUDX11RenderTexture2D* rtv = static_cast<RTUDX11RenderTexture2D*>(_rtv);
	m_d3dContext->ClearRenderTargetView(rtv->m_renderTargetView.Get(), color);
}

void RTUDX11GraphicsContext::ClearDSV(IRTURenderTexture2D* _dsv)
{
	RTUDX11RenderTexture2D* dsv = static_cast<RTUDX11RenderTexture2D*>(_dsv);
	m_d3dContext->ClearDepthStencilView(dsv->m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RTUDX11GraphicsContext::ScreenBeginRender()
{
}

void RTUDX11GraphicsContext::ScreenEndRender()
{
}

void RTUDX11GraphicsContext::BeginCommand(IRTUGraphicsDevice* _device)
{
	RTUDX11GraphicsDevice* device = static_cast<RTUDX11GraphicsDevice*>(_device);
	m_currentRootSignature = nullptr;
	ID3D11SamplerState* samplers[4] = { device->m_defaultSamplers[0].Get(),device->m_defaultSamplers[1].Get(),device->m_defaultSamplers[2].Get(),device->m_defaultSamplers[3].Get() };
	m_d3dContext->PSSetSamplers(0, 4, samplers);
}

void RTUDX11GraphicsContext::EndCommand()
{
	m_d3dContext->ClearState();
#ifdef D3D11_MULTIPLE_THREAD
	ThrowIfFailed(m_d3dContext->FinishCommandList(false, &m_commandList));
#endif
}

void RTUDX11GraphicsContext::ExecuteCommand(IRTUGraphicsDevice* _device)
{
	RTUDX11GraphicsDevice* device = static_cast<RTUDX11GraphicsDevice*>(_device);
#ifdef D3D11_MULTIPLE_THREAD
	device->m_d3dContext->ExecuteCommandList(m_commandList.Get(), false);
#endif
}
