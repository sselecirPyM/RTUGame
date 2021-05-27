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
#include "DeferredRenderPipeline.h"
using namespace DirectX;

void DeferredRenderPipeline::PrepareRenderData(RenderPipelineContext* context, IRTUGraphicsContext* graphicsContext)
{
	auto& bigBuffer = context->m_bigBuffer;
	auto& dcr = context->m_dynamicContex_r;
	auto& m_graphicsDevice = context->m_graphicsDevice;

	memcpy(bigBuffer, &dcr->m_cameraData, sizeof(dcr->m_cameraData));
	graphicsContext->UpdateBuffer(context->m_mainBuffer.get(), bigBuffer, sizeof(dcr->m_cameraData));

	int ofs1 = 0;
	context->m_renderObjectBufferGroup.RequireSliceCount(m_graphicsDevice.get(), dcr->m_cullResultChunk.size() + dcr->m_anotherRenderList.size() + dcr->m_shadowCullResult.size() + 1);
	for (int i = 0; i < dcr->m_cullResultChunk.size(); i++)
	{
		XMFLOAT4X4 toWorldMatrix = dcr->m_cullResultChunk[i].m_toWorldMatrix;
		memcpy(context->m_renderObjectBufferGroup.GetSlicePtr(graphicsContext,  ofs1), &toWorldMatrix, sizeof(toWorldMatrix));
		ofs1++;
	}
	for (int i = 0; i < dcr->m_anotherRenderList.size(); i++)
	{
		XMFLOAT4X4 toWorldMatrix = dcr->m_anotherRenderList[i].m_toWorldMatrix;
		memcpy(context->m_renderObjectBufferGroup.GetSlicePtr(graphicsContext,  ofs1), &toWorldMatrix, sizeof(toWorldMatrix));
		ofs1++;
	}
	for (int i = 0; i < dcr->m_shadowCullResult.size(); i++)
	{
		XMFLOAT4X4 toWorldMatrix = dcr->m_shadowCullResult[i].m_toWorldMatrix;
		memcpy(context->m_renderObjectBufferGroup.GetSlicePtr(graphicsContext,  ofs1), &toWorldMatrix, sizeof(toWorldMatrix));
		ofs1++;
	}
	context->m_renderObjectBufferGroup.UpdateSliceComplete(graphicsContext);

	context->m_lightBufferGroup.RequireSliceCount(m_graphicsDevice.get(), 1);
	byte* ptr1 = (byte*)context->m_lightBufferGroup.GetSlicePtr(graphicsContext, 0);
	memcpy(ptr1, dcr->m_mainLightMatrice, sizeof(dcr->m_mainLightMatrice));
	ptr1 += sizeof(dcr->m_mainLightMatrice);
	memcpy(ptr1, &dcr->m_mainLightData, sizeof(dcr->m_mainLightData));
	context->m_lightBufferGroup.UpdateSliceComplete(graphicsContext);
}

void DeferredRenderPipeline::Render(RenderPipelineContext* context, IRTUGraphicsContext* graphicsContext)
{
	RTUPipelineStateDesc desc1 = {};
	desc1.dsvFormat = context->m_depthFormat;
	desc1.rtvFormat = context->m_swapChainFormat;
	desc1.primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	desc1.eInputLayout = RTU_INPUT_LAYOUT::POSITION_ONLY;
	desc1.renderTargetCount = 1;

	RTUPipelineStateDesc desc_gbuffer = desc1;
	desc_gbuffer.renderTargetCount = 3;
	desc_gbuffer.rtvFormat = context->m_gbufferFormat;
	desc_gbuffer.eInputLayout = RTU_INPUT_LAYOUT::P_N_UV_T;

	RTUPipelineStateDesc desc_shadow = desc1;
	desc_shadow.renderTargetCount = 0;
	desc_shadow.depthBias = 4000;
	desc_shadow.rtvFormat = DXGI_FORMAT_UNKNOWN;

	RTUPipelineStateDesc desc_pass = desc1;
	desc_pass.dsvFormat = DXGI_FORMAT_UNKNOWN;
	desc_pass.rtvFormat = context->m_outputFormat;
	desc_pass.blendmode = RTU_BLEND_MODE_ADD;

	RTUPipelineStateDesc desc_postprocess = desc1;
	desc_postprocess.dsvFormat = DXGI_FORMAT_UNKNOWN;
	desc_postprocess.rtvFormat = context->m_swapChainFormat;

	auto& graphicsDevice = context->m_graphicsDevice;
	auto& dcr = context->m_dynamicContex_r;
	auto RS1 = context->m_rootSignaturePasses.get();
	float clearColor[4] = {};

	//generate gbuffer
	IRTURenderTexture2D* pRT4[4] = { context->m_screenSizeRTV[0].get() ,context->m_screenSizeRTV[1].get() ,context->m_screenSizeRTV[2].get() ,context->m_screenSizeRTV[3].get() };
	graphicsContext->SetRenderTargetRTVDSV(pRT4, 4, context->m_screenSizeDSV[0].get());
	graphicsContext->ClearDSV(context->m_screenSizeDSV[0].get());
	for (int i = 0; i < 4; i++)
	{
		graphicsContext->ClearRTV(context->m_screenSizeRTV[i].get(), clearColor);
	}
	graphicsContext->SetGraphicsRootSignature(RS1);
	graphicsContext->SetCBVR(context->m_mainBuffer.get(),0, 0, 0);
	graphicsContext->SetSRVT(context->m_textures["test"].get(), 0);

	graphicsContext->SetPipelineState(context->m_pipelineStates["deferred_gbuffer"].get(), &desc_gbuffer);
	int ofs1 = 0;
	for (int i = 0; i < dcr->m_cullResultChunk.size(); i++)
	{
		assert(dcr->m_cullResultChunk[i].m_mesh->GetState() == RTU_STATES::RTU_STATES_LOADED);
		graphicsContext->SetMesh(dcr->m_cullResultChunk[i].m_mesh);
		context->m_renderObjectBufferGroup.SetCBVR(graphicsContext,  ofs1, 1);
		graphicsContext->DrawIndexed(dcr->m_cullResultChunk[i].m_indexCount, dcr->m_cullResultChunk[i].m_startIndex, 0);
		ofs1++;
	}
	for (int i = 0; i < dcr->m_anotherRenderList.size(); i++)
	{
		assert(dcr->m_anotherRenderList[i].m_mesh->GetState() == RTU_STATES::RTU_STATES_LOADED);
		graphicsContext->SetMesh(dcr->m_anotherRenderList[i].m_mesh);
		context->m_renderObjectBufferGroup.SetCBVR(graphicsContext, ofs1, 1);
		graphicsContext->DrawIndexed(dcr->m_anotherRenderList[i].m_mesh->GetIndexCount(), 0, 0);
		ofs1++;
	}
	//passes
	IRTURenderTexture2D* pOutputRT = context->m_outputRTV.get();
	//IBL
	graphicsContext->SetRenderTargetRTV(&pOutputRT, 1);
	graphicsContext->ClearRTV(context->m_outputRTV.get(), clearColor);
	graphicsContext->SetSRVT(context->m_screenSizeRTV[0].get(), 0);
	graphicsContext->SetSRVT(context->m_screenSizeRTV[1].get(), 1);
	graphicsContext->SetSRVT(context->m_screenSizeDSV[0].get(), 2);
	graphicsContext->SetPipelineState(context->m_pipelineStates["deferred_ibl"].get(), &desc_pass);
	graphicsContext->SetMesh(context->m_meshes["quad"].get());
	graphicsContext->DrawIndexed(context->m_meshes["quad"]->GetIndexCount(), 0, 0);

	//lightings
	graphicsContext->SetRenderTargetDSV(context->m_shadowMaps[0].get());
	graphicsContext->ClearDSV(context->m_shadowMaps[0].get());
	context->m_lightBufferGroup.SetCBVR(graphicsContext, 0, 0);
	graphicsContext->SetPipelineState(context->m_pipelineStates["shadow"].get(), &desc_shadow);
	for (int i = 0; i < dcr->m_shadowCullResult.size(); i++)
	{
		assert(dcr->m_shadowCullResult[i].m_mesh->GetState() == RTU_STATES::RTU_STATES_LOADED);
		context->m_renderObjectBufferGroup.SetCBVR(graphicsContext, ofs1, 1);
		graphicsContext->SetMesh(dcr->m_shadowCullResult[i].m_mesh);
		graphicsContext->DrawIndexed(dcr->m_shadowCullResult[i].m_indexCount, dcr->m_shadowCullResult[i].m_startIndex, 0);
		ofs1++;
	}
	graphicsContext->SetRenderTargetRTV(&pOutputRT, 1);
	graphicsContext->SetCBVR(context->m_mainBuffer.get(), 0, 0, 0);
	context->m_lightBufferGroup.SetCBVR(graphicsContext, 0, 1);
	graphicsContext->SetSRVT(context->m_screenSizeRTV[0].get(), 0);
	graphicsContext->SetSRVT(context->m_screenSizeRTV[1].get(), 1);
	graphicsContext->SetSRVT(context->m_screenSizeDSV[0].get(), 2);
	graphicsContext->SetSRVT(context->m_shadowMaps[0].get(), 3);
	graphicsContext->SetPipelineState(context->m_pipelineStates["deferred_directLight"].get(), &desc_pass);
	graphicsContext->SetMesh(context->m_meshes["quad"].get());
	graphicsContext->DrawIndexed(context->m_meshes["quad"]->GetIndexCount(), 0, 0);


	//post process
	graphicsContext->SetRenderTargetScreen(nullptr);
	graphicsContext->ClearScreen(context->m_clearColor);
	graphicsContext->SetSRVT(context->m_outputRTV.get(), 0);
	graphicsContext->SetPipelineState(context->m_pipelineStates["postprocess"].get(), &desc_postprocess);
	graphicsContext->SetMesh(context->m_meshes["quad"].get());
	graphicsContext->DrawIndexed(context->m_meshes["quad"]->GetIndexCount(), 0, 0);
}
