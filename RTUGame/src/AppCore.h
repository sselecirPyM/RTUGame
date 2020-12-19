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
#include "IAppCore.h"
#include "Graphics/DirectX12/RTUDX12GraphicsContext.h"
#include "RenderPipeline/DeferredRenderPipeline.h"
#include "RenderPipeline/RendererCore.h"
#include "Client/ClientContext.h"
#include "Net/NetDataWrapLayout.h"
#include "Client/AppMessages.h"
#include "RenderPipeline/ClientToRenderPipeline.h"

using Microsoft::WRL::ComPtr;

class AppCore : public IAppCore
{
public:
	AppCore(int width, int height, std::wstring name);

	virtual void OnInit();
	virtual void OnMouseMove(int x, int y, std::uint32_t flag);
	virtual void OnKeyDown(UINT8 key, LONG lParam, std::uint32_t flag);
	virtual void OnKeyUp(UINT8 key, LONG lParam, std::uint32_t flag);
	virtual void OnLeftButtonDown(int x, int y, std::uint32_t flag);
	virtual void OnLeftButtonUp(int x, int y, std::uint32_t flag);
	virtual void OnSizeChanged(int width, int height, bool minimized);
	virtual void OnDestroy();

	RenderPipelineContext m_renderPipelineContext;
	ClientContext m_clientContext;
	ClientToRenderPipeline m_clientToRenderPipeline;
	DeferredRenderPipeline m_deferredRenderPipeline;
	NetContext m_netContext;
	NetDataWrapLayout m_netDataWrapLayout;
	RendererCore m_rendererCore;

	AppMessages m_messages;

	std::shared_ptr <std::thread> m_renderThread;
	std::shared_ptr <std::thread> m_netSendThread;
	std::shared_ptr <std::thread> m_netReceiveThread;


private:
	void NetLoop();
	void RenderLoop();
};
