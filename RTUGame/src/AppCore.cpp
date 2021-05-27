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
#include "AppCore.h"
#include "Client/UserInput.h"
#include "Client/NetToClient.h"
#include "Graphics/DirectX12/RTUDX12GraphicsFactory.h"
#include "Graphics/DirectX11/RTUDX11GraphicsFactory.h"
#include "Graphics/Vulkan/RTUVKGraphicsFactory.h"
#include "imgui.h"
using namespace DirectX;

AppCore::AppCore(int width, int height, std::wstring name) :
	IAppCore(width, height, name)
{
}

void AppCore::OnInit()
{
	switch (m_launchGraphicsOption)
	{
	case 2:
		m_renderPipelineContext.m_graphicsFactory = std::unique_ptr<RTUVKGraphicsFactory>(new RTUVKGraphicsFactory);
		break;
	case 3:
		m_renderPipelineContext.m_graphicsFactory = std::unique_ptr<RTUDX11GraphicsFactory>(new RTUDX11GraphicsFactory);
		break;
	default:
		m_renderPipelineContext.m_graphicsFactory = std::unique_ptr<RTUDX12GraphicsFactory>(new RTUDX12GraphicsFactory);
		break;
	}
	m_renderThread = std::make_shared<std::thread>(&AppCore::RenderLoop, this);
	m_netReceiveThread = std::make_shared<std::thread>(&AppCore::NetLoop, this);
}

void AppCore::OnMouseMove(int x, int y, std::uint32_t flag)
{
	std::uint64_t timeStamp;
	QueryUnbiasedInterruptTime(&timeStamp);
	switch (flag)
	{
	default:
	{
		std::lock_guard<std::mutex> guard(m_messages.lock_sequenceInputs);
		m_messages.L_sequenceInputs_w->push_back(AppSequenceInput(x, y, timeStamp, SEQUENCE_INPUT_FLAG_MOUSE_MOVE, flag));
	}
	break;
	}
}

void AppCore::OnKeyDown(UINT8 key, LONG lParam, std::uint32_t flag)
{
	std::uint64_t timeStamp;
	QueryUnbiasedInterruptTime(&timeStamp);
	if (flag & 1)
		switch (key)
		{
		case VK_RETURN:
		{
			int a = 0;
		}
		break;
		}
	else
	{
		switch (key)
		{
		case VK_ESCAPE:
		{

		}
		break;
		default:
		{
			std::lock_guard<std::mutex> guard(m_messages.lock_sequenceInputs);
			m_messages.L_sequenceInputs_w->push_back(AppSequenceInput(key, timeStamp, SEQUENCE_INPUT_FLAG_KEYBOARD_DOWN));
		}
		break;
		}
	}
}

void AppCore::OnKeyUp(UINT8 key, LONG lParam, std::uint32_t flag)
{
	std::uint64_t lastUpdateTick;
	QueryUnbiasedInterruptTime(&lastUpdateTick);
	if (flag & 1)
	{

	}
	else
	{
		switch (key)
		{
		case VK_ESCAPE:
		{

		}
		break;
		default:
		{
			std::lock_guard<std::mutex>guard(m_messages.lock_sequenceInputs);
			m_messages.L_sequenceInputs_w->push_back(AppSequenceInput(key, lastUpdateTick, SEQUENCE_INPUT_FLAG_KEYBOARD_UP));
		}
		break;
		}
	}
}

void AppCore::OnLeftButtonDown(int x, int y, std::uint32_t flag)
{
	std::uint64_t timeStamp;
	QueryUnbiasedInterruptTime(&timeStamp);
	switch (flag)
	{
	default:
	{
		std::lock_guard<std::mutex>guard(m_messages.lock_sequenceInputs);
		m_messages.L_sequenceInputs_w->push_back(AppSequenceInput(x, y, timeStamp, SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_DOWN, flag));
	}
	break;
	}
}

void AppCore::OnLeftButtonUp(int x, int y, std::uint32_t flag)
{
	std::uint64_t timeStamp;
	QueryUnbiasedInterruptTime(&timeStamp);
	switch (flag)
	{
	default:
	{
		std::lock_guard<std::mutex>guard(m_messages.lock_sequenceInputs);
		m_messages.L_sequenceInputs_w->push_back(AppSequenceInput(x, y, timeStamp, SEQUENCE_INPUT_FLAG_MOUSE_LBUTTON_UP, flag));
	}
	break;
	}
}

void AppCore::OnSizeChanged(int width, int height, bool minimized)
{
	if (!minimized)
	{
		m_messages.ScreenSize.x = width;
		m_messages.ScreenSize.y = height;
	}
}

void AppCore::OnDestroy()
{
	m_messages.low_requireDestory = true;
	m_netDataWrapLayout.Close();
	m_renderThread->join();
	m_netReceiveThread->join();
	if (m_netSendThread != nullptr)
	{
		m_netSendThread->join();
	}
	CloseHandle(m_netContext.m_sendEvent);
}

void AppCore::NetLoop()
{
	m_netDataWrapLayout.Init(&m_netContext);
	if (m_netDataWrapLayout.Connect(UnicodeToUTF8(m_connectAddress).c_str(), UnicodeToUTF8(m_connectPort).c_str()))
	{
		m_netSendThread = std::make_shared<std::thread>(&NetDataWrapLayout::SendService, &m_netDataWrapLayout);
		m_netDataWrapLayout.ReceiveService();
	}
}

void AppCore::RenderLoop()
{
	ThrowIfFailed(CoInitialize(NULL));
	QueryUnbiasedInterruptTime(&m_messages.m_currentTick);

	m_renderPipelineContext.Init();
	m_rendererCore.m_currentRenderPipeline = &m_deferredRenderPipeline;
	m_rendererCore.m_renderPipelineContext = &m_renderPipelineContext;


	m_messages.ScreenSize.x = m_width;
	m_messages.ScreenSize.y = m_height;
	m_renderPipelineContext.m_screenSize = glm::u32vec2(m_width, m_height);
	m_renderPipelineContext.m_aspectRatio = float(m_width) / float(m_height);

	auto& m_graphicsDevice = m_renderPipelineContext.m_graphicsDevice;
	auto& m_graphicsContext = m_renderPipelineContext.m_graphicsContext;


	m_graphicsDevice->SetupSwapChain(m_hwnd, m_hinstance, m_width, m_height, m_renderPipelineContext.m_swapChainFormat);

	m_clientContext.Init();
	m_clientContext.m_clientPlayer.Init();
	m_renderPipelineContext.InitResources(m_assetsPath);

	m_clientContext.ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext((ImGuiContext*)m_clientContext.ImGuiContext);
	unsigned char* pixels;
	int width;
	int height;
	int bytePerPixel;
	ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\SIMHEI.ttf", 13, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
	ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytePerPixel);

	std::shared_ptr<RTUTexture2DLoader> texture2DLoading = std::make_shared<RTUTexture2DLoader>();
	texture2DLoading->m_data.resize(width * height * bytePerPixel);
	memcpy(texture2DLoading->m_data.data(), pixels, width * height * bytePerPixel);
	texture2DLoading->m_width = width;
	texture2DLoading->m_height = height;
	texture2DLoading->m_mipLevels = 1;
	texture2DLoading->m_format = DXGI_FORMAT_R8G8B8A8_UNORM;


	m_renderPipelineContext.L_texture2DLoadList0_w->emplace_back(Texture2DUploadContainer(m_renderPipelineContext.Texture2D("imgui_font"), texture2DLoading));

	while (true)
	{
		auto& m_graphicsDevice = m_renderPipelineContext.m_graphicsDevice;

		{
			auto capture = m_messages.ScreenSize;
			auto& current = m_messages.ScreenSizePrevious;

			if (current != capture)
			{
				current = capture;
				m_renderPipelineContext.ProcessSizeChange(capture.x, capture.y);
			}
			if (m_messages.low_requireDestory)
			{
				m_graphicsDevice->WaitForGpu();
				m_graphicsDevice->Uninit();
				break;
			}
		}
		NetToClient::Process(&m_netContext, &m_clientContext);
		m_clientToRenderPipeline.Process1(&m_clientContext, &m_renderPipelineContext);//for low lag

		m_messages.m_previousTick = m_messages.m_currentTick;
		QueryUnbiasedInterruptTime(&m_messages.m_currentTick);
		m_messages.m_deltaTime = std::min(std::max(double(m_messages.m_currentTick - m_messages.m_previousTick) / 1e7, 0.0), 0.5);
		m_clientContext.m_currentFrameStamp += m_messages.m_currentTick - m_messages.m_previousTick;

		UserInput::Process(&m_clientContext, &m_messages);
		m_clientContext.m_physicsDevice->SceneSimulate(m_clientContext.m_physicsScene.get(), m_messages.m_deltaTime);
		m_clientToRenderPipeline.Process2(&m_clientContext, &m_renderPipelineContext);

		m_rendererCore.Render1();
		std::this_thread::yield();
	}
	CoUninitialize();
}
