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
#include "RTUDX11GraphicsDevice.h"
#include "RTUDX11RenderTexture2D.h"
#include "RTUDX11RootSignature.h"
#include "RTUDX11PipelineState.h"
#include "RTUDX11Mesh.h"
#include "RTUDX11CBuffer.h"
#include "RTUDX11SBuffer.h"
#include "../../DXUtil.h"

#if defined(_DEBUG)
	// ���� SDK ��֧�֡�
inline bool SdkLayersAvailable()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // ���贴��ʵ��Ӳ���豸��
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // ���� SDK �㡣
		nullptr,                    // �κι��ܼ��𶼻�������
		0,
		D3D11_SDK_VERSION,          // ���� Windows Ӧ���̵�Ӧ�ã�ʼ�ս���ֵ����Ϊ D3D11_SDK_VERSION��
		nullptr,                    // ���豣�� D3D �豸���á�
		nullptr,                    // ����֪�����ܼ���
		nullptr                     // ���豣�� D3D �豸���������á�
	);

	return SUCCEEDED(hr);
}
#endif

void RTUDX11GraphicsDevice::Init()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (SdkLayersAvailable())
	{
		// �����Ŀ���ڵ������ɹ����У���ͨ�����д˱�־�� SDK �����õ��ԡ�
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// ָ�� nullptr ��ʹ��Ĭ����������
		D3D_DRIVER_TYPE_HARDWARE,	// ����ʹ��Ӳ��ͼ������������豸��
		0,							// ӦΪ 0���������������� D3D_DRIVER_TYPE_SOFTWARE��
		creationFlags,				// ���õ��Ժ� Direct2D �����Ա�־��
		featureLevels,				// ��Ӧ�ó������֧�ֵĹ��ܼ�����б�
		ARRAYSIZE(featureLevels),	// ������б�Ĵ�С��
		D3D11_SDK_VERSION,			// ���� Windows Ӧ���̵�Ӧ�ã�ʼ�ս���ֵ����Ϊ D3D11_SDK_VERSION��
		&device,					// ���ش����� Direct3D �豸��
		&m_d3dFeatureLevel,			// �����������豸�Ĺ��ܼ���
		&context					// �����豸�ļ�ʱ�����ġ�
	);

	if (FAILED(hr))
	{
		// �����ʼ��ʧ�ܣ�����˵� WARP �豸��
		// �й� WARP ����ϸ��Ϣ�������: 
		// https://go.microsoft.com/fwlink/?LinkId=286690
		ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // ���� WARP �豸������Ӳ���豸��
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&m_d3dFeatureLevel,
				&context
			)
		);
	}
	ThrowIfFailed(device.As(&m_d3dDevice));
	ThrowIfFailed(context.As(&m_d3dContext));


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	if (m_d3dFeatureLevel > D3D_FEATURE_LEVEL_11_0)
		samplerDesc.MaxAnisotropy = 16;
	else if (m_d3dFeatureLevel > D3D_FEATURE_LEVEL_9_1)
		samplerDesc.MaxAnisotropy = 4;
	else
		samplerDesc.MaxAnisotropy = 2;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, &m_defaultSamplers[0]));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, &m_defaultSamplers[1]));

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, &m_defaultSamplers[2]));
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, &m_defaultSamplers[3]));

	m_executedCount++;
}

void RTUDX11GraphicsDevice::CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView = nullptr;
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	width = std::max<int>(width, 1);
	height = std::max<int>(height, 1);
	if (m_swapChain != nullptr)
	{
		// ����������Ѵ��ڣ���������С��
		HRESULT hr = m_swapChain->ResizeBuffers(
			m_bufferCount,
			width,
			height,
			swapChainFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
		);
	}
	else
	{
		DXGI_SCALING scaling = DXGI_SCALING_STRETCH;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = width;		// ƥ�䴰�ڵĴ�С��
		swapChainDesc.Height = height;
		swapChainDesc.Format = swapChainFormat;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;								// �벻Ҫʹ�ö������
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_bufferCount;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.Scaling = scaling;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// �����л�ȡ������������� Direct3D �豸�� DXGI ������
		ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory4> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(m_d3dDevice.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &swapChain));

		ThrowIfFailed(swapChain.As(&m_swapChain));
	}
	// ������������̨����������ȾĿ����ͼ��
	ComPtr<ID3D11Texture2D1> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_d3dRenderTargetView));
	m_screenViewport = CD3D11_VIEWPORT(0.0f, 0.0f, width, height);
}

void RTUDX11GraphicsDevice::Present(bool vsync)
{
	DXGI_PRESENT_PARAMETERS parameters = {};
	HRESULT hr;
	if (vsync)
		hr = m_swapChain->Present1(1, 0, &parameters);
	else
		hr = m_swapChain->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &parameters);
	m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

	m_executeIndex = (m_executeIndex < (c_maxQueueFrameCount - 1)) ? (m_executeIndex + 1) : 0;
	ThrowIfFailed(hr);
	m_executedCount++;
}

void RTUDX11GraphicsDevice::BeginCommandAllocator()
{
}

void RTUDX11GraphicsDevice::WaitForGpu()
{
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);
	m_executedCount++;
}

void RTUDX11GraphicsDevice::Next()
{
	m_executeIndex = (m_executeIndex < (c_maxQueueFrameCount - 1)) ? (m_executeIndex + 1) : 0;
	m_executedCount++;
}

void RTUDX11GraphicsDevice::SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat)
{
	m_hwnd = hwnd;
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView = nullptr;
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);
}

void RTUDX11GraphicsDevice::InitRootSignaturePass1(IRTURootSignature* _rootSignature)
{
	RTUDX11RootSignature* rootSignature = static_cast<RTUDX11RootSignature*>(_rootSignature);
	rootSignature->m_mapToSlot = { 0,1,2,0,1,2,3,4,5,6 };
}

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

void RTUDX11GraphicsDevice::InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* _pipelineState)
{
	RTUDX11PipelineState* pipelineState = static_cast<RTUDX11PipelineState*>(_pipelineState);
	pipelineState->m_vsData = data->m_vertexShader;
	pipelineState->m_psData = data->m_pixelShader;
}

void RTUDX11GraphicsDevice::InitBuffer(IRTUCBuffer* _buffer, uint32_t size)
{
	RTUDX11CBuffer* buffer = static_cast<RTUDX11CBuffer*>(_buffer);
	D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER,D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE);
	ThrowIfFailed(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, &buffer->m_buffer));
}

void RTUDX11GraphicsDevice::InitBuffer(IRTUSBuffer* _buffer, uint32_t size)
{
	RTUDX11SBuffer* buffer = static_cast<RTUDX11SBuffer*>(_buffer);
	D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, &buffer->m_buffer));
}

void RTUDX11GraphicsDevice::InitRenderTexture2D(IRTURenderTexture2D* _texture, int width, int height)
{
	RTUDX11RenderTexture2D* texture = static_cast<RTUDX11RenderTexture2D*>(_texture);
	texture->m_width = width;
	texture->m_height = height;

	D3D11_TEXTURE2D_DESC tex2DDesc = {};
	tex2DDesc.Width = width;
	tex2DDesc.Height = height;
	tex2DDesc.Format = texture->m_format;
	tex2DDesc.ArraySize = 1;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2DDesc.CPUAccessFlags = 0;
	tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (texture->m_dsvFormat)
		tex2DDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	if (texture->m_rtvFormat)
		tex2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if (texture->m_uavFormat)
		tex2DDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	tex2DDesc.MiscFlags = 0;
	tex2DDesc.MipLevels = 1;
	ThrowIfFailed(m_d3dDevice->CreateTexture2D(&tex2DDesc, nullptr, &texture->m_texture2D));


	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(texture->m_texture2D.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, texture->m_srvformat);
	ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(
		texture->m_texture2D.Get(),
		&shaderResourceViewDesc,
		&texture->m_shaderResourceView
	));
	if (texture->m_dsvFormat)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = texture->m_dsvFormat;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(texture->m_texture2D.Get(), &depthStencilViewDesc, &texture->m_depthStencilView));
	}
	if (texture->m_rtvFormat)
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = texture->m_rtvFormat;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(texture->m_texture2D.Get(), &renderTargetViewDesc, &texture->m_renderTargetView));
	}
	if (texture->m_uavFormat)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc = {};
		unorderedAccessViewDesc.Format = texture->m_uavFormat;
		unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		ThrowIfFailed(m_d3dDevice->CreateUnorderedAccessView(texture->m_texture2D.Get(), &unorderedAccessViewDesc, &texture->m_unorderedAccessView));
	}
}

void RTUDX11GraphicsDevice::Uninit()
{
}
