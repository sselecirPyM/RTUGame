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
	int executeIndex = m_graphicsDevice->GetExecuteIndex();
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
			int executeIndex2 = m_graphicsDevice->GetExecuteIndex();
			m_graphicsDevice->BeginCommandAllocator();
		}

	}

	auto& m_graphicsContext = m_renderPipelineContext->m_graphicsContext;
	m_graphicsContext->BeginCommand(m_graphicsDevice.get());
	m_graphicsContext->SetHeapDefault();
	m_currentRenderPipeline->PrepareRenderData(m_renderPipelineContext,m_renderPipelineContext->m_graphicsContext.get());

	m_graphicsContext->ScreenBeginRender();
	m_currentRenderPipeline->Render(m_renderPipelineContext, m_renderPipelineContext->m_graphicsContext.get());
	m_graphicsContext->ScreenEndRender();
	m_graphicsContext->EndCommand();
	m_graphicsContext->ExecuteCommand(m_graphicsDevice.get());
	m_graphicsDevice->Present(true);
}
