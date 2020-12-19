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
class RTUDX11GraphicsDevice : public IRTUGraphicsDevice
{
public:
	void Init();
	void CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat);
	void Present(bool vsync);
	void BeginCommandAllocator();
	void WaitForGpu();
	void Next();
	void SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat);

	void InitRootSignaturePass1(IRTURootSignature* rootSignature);
	void InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* pipelineState);
	void InitBuffer(IRTUCBuffer* buffer, uint32_t size);
	void InitBuffer(IRTUSBuffer* buffer, uint32_t size);
	void InitRenderTexture2D(IRTURenderTexture2D* texture, int width, int height);
	void Uninit();

	int GetExecuteIndex() { return m_executeIndex; };
	uint64_t GetExecutedCount() { return m_executedCount; }

	int m_executeIndex = 0;
	uint64_t m_executedCount = 0;

	ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }

	HWND											m_hwnd;

	D3D_FEATURE_LEVEL								m_d3dFeatureLevel;

	Microsoft::WRL::ComPtr<ID3D11Device3>			m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;


	D3D11_VIEWPORT									m_screenViewport;
	int												m_bufferCount = 2;

	ComPtr<ID3D11SamplerState>						m_defaultSamplers[4];
};

