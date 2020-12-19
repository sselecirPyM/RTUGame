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
#include "../RTUPipelineStateLoader.h"
#include "../../Common/RTUCommon.h"
#include "../RTUGraphicsInterfaces.h"
class RTUDX12GraphicsDevice : public IRTUGraphicsDevice
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

	int GetExecuteIndex() { return m_executeIndex; }
	uint64_t GetExecutedCount() { return m_currentFenceValue; }

	ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() const { return m_commandAllocators[m_executeIndex].Get(); }
	ID3D12Resource* GetRenderTarget() const { return m_renderTargets[m_frameIndex].Get(); }
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize); }
	ID3D12Device* GetD3DDevice() const { return m_d3dDevice.Get(); }

	bool m_isRayTracingSupport = false;
#pragma region Descriptions
	DXGI_ADAPTER_DESC1 m_desc = {};
#pragma endregion

	const static UINT c_rtvHeapMaxCount = 2048;
	const static UINT c_dsvHeapMaxCount = 2048;
	const static UINT c_cbvsrvuavHeapMaxCount = 65536;

	HWND											m_hwnd;

	float											m_dpi;
	float											m_activatedDpi;


	Microsoft::WRL::ComPtr<ID3D12Device>			m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Device5>			m_d3dDevice5;

	Microsoft::WRL::ComPtr<IDXGIFactory6>			m_dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	m_cbvsrvuavHeap;
	UINT											m_cbvsrvuavHeapAllocCount = 0;
	UINT											m_cbvsrvuavDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	UINT											m_rtvHeapAllocCount = 0;
	UINT											m_rtvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	m_dsvHeap;
	UINT											m_dsvHeapAllocCount = 0;
	UINT											m_dsvDescriptorSize = 0;


	Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	m_commandAllocators[c_maxQueueFrameCount];


	Microsoft::WRL::ComPtr<ID3D12Fence>				m_fence;
	std::uint64_t									m_fenceValues[c_maxQueueFrameCount] = {};
	HANDLE											m_fenceEvent = 0;
	UINT											m_frameIndex = 0;
	int												m_executeIndex = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource>			m_renderTargets[c_maxQueueFrameCount];

	D3D12_VIEWPORT									m_screenViewport;

	std::uint64_t									m_currentFenceValue = 0;
};

