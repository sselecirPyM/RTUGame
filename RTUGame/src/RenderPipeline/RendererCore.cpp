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
#include "RendererCore.h"
#include "imgui.h"

void RendererCore::Render1()
{
	m_renderPipelineContext->SwapDynamicContext();
	auto& m_graphicsDevice = m_renderPipelineContext->m_graphicsDevice;
	auto& m_meshLoadList0_r = m_renderPipelineContext->m_meshLoadList0_r;
	auto& m_texture2DLoadList0_r = m_renderPipelineContext->m_texture2DLoadList0_r;
	{
		std::lock_guard<std::mutex> guard(m_renderPipelineContext->lock_meshLoadList);
		std::swap(m_renderPipelineContext->L_meshLoadList0_w, m_meshLoadList0_r);
	}
	{
		std::lock_guard<std::mutex> guard(m_renderPipelineContext->lock_texture2DLoadList);
		std::swap(m_renderPipelineContext->L_texture2DLoadList0_w, m_texture2DLoadList0_r);
	}

	m_graphicsDevice->BeginCommandAllocator();
	auto& _graphicsContext = m_renderPipelineContext->m_graphicsContextUp1;
	while (!m_meshLoadList0_r->empty() || !m_texture2DLoadList0_r->empty())
	{
		_graphicsContext->BeginCommand(m_graphicsDevice.get());

		int startIndex = std::max(0, static_cast<int>(m_meshLoadList0_r->size()) - 512);
		for (int i = startIndex; i < m_meshLoadList0_r->size(); i++)
		{
			_graphicsContext->UploadMesh((*m_meshLoadList0_r)[i].m_mesh, (*m_meshLoadList0_r)[i].m_meshLoader.get());
		}
		for (int i = 0; i < m_texture2DLoadList0_r->size(); i++)
		{
			_graphicsContext->UploadTexture((*m_texture2DLoadList0_r)[i].m_texture, (*m_texture2DLoadList0_r)[i].m_textureLoader.get());
		}

		while (startIndex < m_meshLoadList0_r->size())
		{
			m_meshLoadList0_r->pop_back();
		}
		m_texture2DLoadList0_r->clear();

		_graphicsContext->EndCommand();
		_graphicsContext->ExecuteCommand(m_graphicsDevice.get());
		if (!m_meshLoadList0_r->empty() || !m_texture2DLoadList0_r->empty())
		{
			m_graphicsDevice->Next();
			m_graphicsDevice->BeginCommandAllocator();
		}

	}

	auto& m_graphicsContext = m_renderPipelineContext->m_graphicsContext;
	auto& io = ImGui::GetIO();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	auto data = ImGui::GetDrawData();

	float L = data->DisplayPos.x;
	float R = data->DisplayPos.x + data->DisplaySize.x;
	float T = data->DisplayPos.y;
	float B = data->DisplayPos.y + data->DisplaySize.y;
	float mvp[] =
	{
			2.0f / (R - L),   0.0f,           0.0f,       0.0f,
			0.0f,         2.0f / (T - B),     0.0f,       0.0f,
			0.0f,         0.0f,           0.5f,       0.0f,
			(R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f,
	};

	m_graphicsContext->BeginCommand(m_graphicsDevice.get());
	m_graphicsContext->SetHeapDefault();
	m_currentRenderPipeline->PrepareRenderData(m_renderPipelineContext, m_renderPipelineContext->m_graphicsContext.get());

	m_graphicsContext->ScreenBeginRender();
	m_currentRenderPipeline->Render(m_renderPipelineContext, m_renderPipelineContext->m_graphicsContext.get());


	m_renderPipelineContext->m_imguiTest.RequireSliceCount(m_graphicsDevice.get(), 1);
	byte* ptr1 = (byte*)m_renderPipelineContext->m_imguiTest.GetSlicePtr(m_graphicsContext.get(), 0);
	memcpy(ptr1, mvp, sizeof(mvp));

	m_renderPipelineContext->m_imguiTest.UpdateSliceComplete(m_graphicsContext.get());

	RTUMeshLoader meshLoader;
	for (int i = 0; i < data->CmdListsCount; i++)
	{
		auto cmdList = data->CmdLists[i];
		auto* mesh = m_renderPipelineContext->Mesh("imgui_mesh");
		meshLoader.m_indexFormat = DXGI_FORMAT_R16_UINT;
		meshLoader.m_vertexStride = sizeof(ImDrawVert);
		meshLoader.m_vertexData.resize(cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		meshLoader.m_indexData.resize(cmdList->IdxBuffer.Size * sizeof(unsigned int));
		memcpy(meshLoader.m_vertexData.data(), cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(meshLoader.m_indexData.data(), cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(unsigned int));

		m_graphicsContext->UploadMesh(mesh, &meshLoader);

		RTUPipelineStateDesc desc1 = {};
		desc1.rtvFormat = m_renderPipelineContext->m_swapChainFormat;
		desc1.primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		desc1.eInputLayout = RTU_INPUT_LAYOUT::IMGUI;
		desc1.renderTargetCount = 1;
		desc1.blendmode = RTU_BLEND_MODE_ALPHA;

		for (int j = 0; j < cmdList->CmdBuffer.size(); j++)
		{
			auto& cmd = cmdList->CmdBuffer[j];
			m_graphicsContext->SetMesh(mesh);
			m_graphicsContext->SetPipelineState(m_renderPipelineContext->PipelineState("imgui"),&desc1);
			m_graphicsContext->SetSRVT(m_renderPipelineContext->Texture2D("imgui_font"), 0);
			m_renderPipelineContext->m_imguiTest.SetCBVR(m_graphicsContext.get(), 0, 0);
			m_graphicsContext->DrawIndexed(cmd.ElemCount, cmd.IdxOffset, cmd.VtxOffset);
		}
	}

	m_graphicsContext->ScreenEndRender();
	m_graphicsContext->EndCommand();
	m_graphicsContext->ExecuteCommand(m_graphicsDevice.get());
	m_graphicsDevice->Present(true);
}
