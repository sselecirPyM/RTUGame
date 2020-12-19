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
#include "RTUDX12GraphicsDevice.h"
#include "RTUDX12RootSignature.h"
#include "RTUDX12PipelineState.h"
#include "RTUDX12CBuffer.h"
#include "RTUDX12SBuffer.h"
#include "RTUDX12RenderTexture2D.h"
#include "DX12Util.h"
#include "../../DXUtil.h"


void RTUDX12GraphicsDevice::Init()
{
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

	ComPtr<IDXGIAdapter1> adapter;
	DX12::GetAdapter1(m_dxgiFactory.Get(), &adapter, m_desc);

	ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));

	m_isRayTracingSupport = DX12::IsDirectXRaytracingSupported(m_d3dDevice.Get());

	m_d3dDevice->QueryInterface(IID_PPV_ARGS(&m_d3dDevice5));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	NAME_D3D12_OBJECT(m_commandQueue);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = c_rtvHeapMaxCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	NAME_D3D12_OBJECT(m_rtvHeap);
	m_rtvHeapAllocCount = c_maxQueueFrameCount;
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = c_dsvHeapMaxCount;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
	NAME_D3D12_OBJECT(m_dsvHeap);
	m_dsvHeapAllocCount = 0;
	m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = c_cbvsrvuavHeapMaxCount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvsrvuavHeap)));
	NAME_D3D12_OBJECT(m_cbvsrvuavHeap);
	m_cbvsrvuavHeapAllocCount = 0;
	m_cbvsrvuavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT n = 0; n < c_maxQueueFrameCount; n++)
	{
		ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
	}

	ThrowIfFailed(m_d3dDevice->CreateFence(m_currentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_currentFenceValue++;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void RTUDX12GraphicsDevice::CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat)
{
	WaitForGpu();
	for (UINT n = 0; n < c_maxQueueFrameCount; n++)
	{
		m_renderTargets[n] = nullptr;
	}

	m_activatedDpi = m_dpi;

	//// 防止创建大小为零的 DirectX 内容。
	width = std::max<int>(width, 1);
	height = std::max<int>(height, 1);


	if (m_swapChain != nullptr)
	{
		// 如果交换链已存在，请调整其大小。
		HRESULT hr = m_swapChain->ResizeBuffers(c_maxQueueFrameCount, width, height, swapChainFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		ThrowIfFailed(hr);
	}
	else
	{
		// 否则，使用与现有 Direct3D 设备相同的适配器新建一个。
		DXGI_SCALING scaling = DXGI_SCALING_STRETCH;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = swapChainFormat;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = c_maxQueueFrameCount;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.Scaling = scaling;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			m_hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		ThrowIfFailed(swapChain.As(&m_swapChain));
	}

	// 创建交换链后台缓冲区的呈现目标视图。
	{
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < c_maxQueueFrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvDescriptor);
			rtvDescriptor.Offset(m_rtvDescriptorSize);

			WCHAR name[25];
			if (swprintf_s(name, L"m_renderTargets[%u]", n) > 0)
			{
				SetName(m_renderTargets[n].Get(), name);
			}
		}
	}

	// 设置用于确定整个窗口的 3D 渲染视区。
	m_screenViewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
}

void RTUDX12GraphicsDevice::Present(bool vsync)
{
	HRESULT hr;
	if (vsync)
	{
		hr = m_swapChain->Present(1, 0);
	}
	else
	{
		hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		//m_deviceRemoved = true;
		return;
	}
	else
	{
		ThrowIfFailed(hr);
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFenceValue));

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		m_executeIndex = (m_executeIndex < (c_maxQueueFrameCount - 1)) ? (m_executeIndex + 1) : 0;

		// 检查下一帧是否准备好启动。
		if (m_fence->GetCompletedValue() < m_fenceValues[m_executeIndex])
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_executeIndex], m_fenceEvent));
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}

		// 为下一帧设置围栏值。
		m_currentFenceValue++;
		m_fenceValues[m_executeIndex] = m_currentFenceValue;
		return;
	}
}

void RTUDX12GraphicsDevice::BeginCommandAllocator()
{
	GetCommandAllocator()->Reset();
}

void RTUDX12GraphicsDevice::WaitForGpu()
{
	// 在队列中安排信号命令。
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFenceValue));

	// 等待跨越围栏。
	ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFenceValue, m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

	// 对当前帧递增围栏值。
	m_currentFenceValue++;
}

void RTUDX12GraphicsDevice::Next()
{
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFenceValue));
	m_executeIndex = (m_executeIndex < (c_maxQueueFrameCount - 1)) ? (m_executeIndex + 1) : 0;
	if (m_fence->GetCompletedValue() < m_fenceValues[m_executeIndex])
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_executeIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	m_currentFenceValue++;
	m_fenceValues[m_executeIndex] = m_currentFenceValue;
}

void RTUDX12GraphicsDevice::SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat)
{
	m_hwnd = hwnd;

	//CreateWindowSizeDependentResources(width, height, swapChainFormat);
	ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES));
}

void RTUDX12GraphicsDevice::InitRootSignaturePass1(IRTURootSignature* rootSignature)
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featherData;
	featherData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featherData, sizeof(featherData))))
	{
		featherData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_ROOT_PARAMETER1 parameter[10] = {};
	CD3DX12_DESCRIPTOR_RANGE1 range[7] = {};
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	range[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	range[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
	range[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	range[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	parameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
	parameter[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
	parameter[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
	parameter[3].InitAsDescriptorTable(1, &range[0]);
	parameter[4].InitAsDescriptorTable(1, &range[1]);
	parameter[5].InitAsDescriptorTable(1, &range[2]);
	parameter[6].InitAsDescriptorTable(1, &range[3]);
	parameter[7].InitAsDescriptorTable(1, &range[4]);
	parameter[8].InitAsDescriptorTable(1, &range[5]);
	parameter[9].InitAsDescriptorTable(1, &range[6]);

	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
	staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplerDesc.MipLODBias = 0;
	staticSamplerDesc.MaxAnisotropy = 0;
	staticSamplerDesc.MinLOD = 0;
	staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	staticSamplerDesc.RegisterSpace = 0;
	D3D12_STATIC_SAMPLER_DESC staticSamplerDescs[4] = { staticSamplerDesc,staticSamplerDesc,staticSamplerDesc,staticSamplerDesc };
	for (int i = 0; i < _countof(staticSamplerDescs); i++)
	{
		staticSamplerDescs[i].ShaderRegister = i;
	}
	staticSamplerDescs[1].MaxAnisotropy = 16;
	staticSamplerDescs[1].Filter = D3D12_FILTER_ANISOTROPIC;
	staticSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
	staticSamplerDescs[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	staticSamplerDescs[3].Filter = D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	staticSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDescs[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(parameter), parameter, _countof(staticSamplerDescs), staticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featherData.HighestVersion, &signature, &error));
	ThrowIfFailed(m_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&static_cast<RTUDX12RootSignature*>(rootSignature)->m_rootSignature)));
}

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

void RTUDX12GraphicsDevice::InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* _pipelineState)
{
	RTUDX12PipelineState* pipelineState = static_cast<RTUDX12PipelineState*>(_pipelineState);
	pipelineState->m_vertexShader = data->m_vertexShader;
	pipelineState->m_geometryShader = data->m_geometryShader;
	pipelineState->m_pixelShader = data->m_pixelShader;

	pipelineState->m_primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void RTUDX12GraphicsDevice::InitBuffer(IRTUCBuffer* _buffer, uint32_t size)
{
	RTUDX12CBuffer* buffer = static_cast<RTUDX12CBuffer*>(_buffer);
	buffer->m_size = (size + 255) & ~255;

	auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(buffer->m_size * c_maxQueueFrameCount);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapUpload,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer->m_buffer)));
	NAME_D3D12_OBJECT(buffer->m_buffer);
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(buffer->m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&buffer->m_mappedBuffer)));
	ZeroMemory(buffer->m_mappedBuffer, buffer->m_size * c_maxQueueFrameCount);
}

void RTUDX12GraphicsDevice::InitBuffer(IRTUSBuffer* _buffer, uint32_t size)
{
	RTUDX12SBuffer* buffer = static_cast<RTUDX12SBuffer*>(_buffer);
	buffer->m_size = (size + 255) & ~255;
	auto bufDesc = CD3DX12_RESOURCE_DESC::Buffer(buffer->m_size);
	auto heapDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapDefault,
		D3D12_HEAP_FLAG_NONE,
		&bufDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer->m_buffer)));
	NAME_D3D12_OBJECT(buffer->m_buffer);

	auto bufDesc1 = CD3DX12_RESOURCE_DESC::Buffer(buffer->m_size);
	for (int i = 0; i < c_maxQueueFrameCount; i++)
	{
		ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
			&heapUpload,
			D3D12_HEAP_FLAG_NONE,
			&bufDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&buffer->m_bufferUpload[i])));

		NAME_D3D12_OBJECT(buffer->m_bufferUpload[i]);
	}
}

void RTUDX12GraphicsDevice::InitRenderTexture2D(IRTURenderTexture2D* _texture, int width, int height)
{
	RTUDX12RenderTexture2D* texture = static_cast<RTUDX12RenderTexture2D*>(_texture);
	texture->m_width = width;
	texture->m_height = height;
	auto d3dDevice = m_d3dDevice;
	if (texture->m_texture == nullptr)
	{
		texture->m_srvRef = _InterlockedIncrement(&m_cbvsrvuavHeapAllocCount) - 1;
		if (texture->m_dsvFormat != DXGI_FORMAT_UNKNOWN)
		{
			texture->m_dsvRef = _InterlockedIncrement(&m_dsvHeapAllocCount) - 1;
		}
		if (texture->m_rtvFormat != DXGI_FORMAT_UNKNOWN)
		{
			texture->m_rtvRef = _InterlockedIncrement(&m_rtvHeapAllocCount) - 1;
		}
		if (texture->m_uavFormat != DXGI_FORMAT_UNKNOWN)
		{
			texture->m_uavRef = _InterlockedIncrement(&m_cbvsrvuavHeapAllocCount) - 1;
		}
	}

	{
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		if (texture->m_dsvFormat != DXGI_FORMAT_UNKNOWN)
			textureDesc.Format = texture->m_dsvFormat;
		else
			textureDesc.Format = texture->m_format;
		textureDesc.Width = texture->m_width;
		textureDesc.Height = texture->m_height;
		textureDesc.Flags = texture->m_resourceFlags;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		auto heapDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		if (texture->m_dsvFormat != DXGI_FORMAT_UNKNOWN)
		{
			CD3DX12_CLEAR_VALUE clearValue(texture->m_dsvFormat, 1.0f, 0);
			ThrowIfFailed(d3dDevice->CreateCommittedResource(
				&heapDefault,
				D3D12_HEAP_FLAG_NONE,
				&textureDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				&clearValue,
				IID_PPV_ARGS(&texture->m_texture)));
		}
		else
		{
			float color[] = { 0.0f,0.0f,0.0f,0.0f };
			CD3DX12_CLEAR_VALUE clearValue(texture->m_format, color);
			ThrowIfFailed(d3dDevice->CreateCommittedResource(
				&heapDefault,
				D3D12_HEAP_FLAG_NONE,
				&textureDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				&clearValue,
				IID_PPV_ARGS(&texture->m_texture)));
		}
		texture->m_previousResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
		NAME_D3D12_OBJECT(texture->m_texture);
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = texture->m_format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	UINT incrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvsrvuavHeap->GetCPUDescriptorHandleForHeapStart(), texture->m_srvRef, incrementSize);
	d3dDevice->CreateShaderResourceView(texture->m_texture.Get(), &srvDesc, handle);
	if (texture->m_dsvFormat != DXGI_FORMAT_UNKNOWN)
	{
		incrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), texture->m_dsvRef, incrementSize);
		d3dDevice->CreateDepthStencilView(texture->m_texture.Get(), nullptr, handle);
	}
	if (texture->m_rtvFormat != DXGI_FORMAT_UNKNOWN)
	{
		incrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), texture->m_rtvRef, incrementSize);
		d3dDevice->CreateRenderTargetView(texture->m_texture.Get(), nullptr, handle);
	}
	if (texture->m_uavFormat != DXGI_FORMAT_UNKNOWN)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Format = texture->m_uavFormat;

		incrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvsrvuavHeap->GetCPUDescriptorHandleForHeapStart(), texture->m_uavRef, incrementSize);
		d3dDevice->CreateUnorderedAccessView(texture->m_texture.Get(), nullptr, &uavDesc, handle);
	}
}

void RTUDX12GraphicsDevice::Uninit()
{
	CloseHandle(m_fenceEvent);
}
