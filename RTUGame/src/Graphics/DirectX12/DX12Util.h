#pragma once
namespace DX12
{
	inline bool IsDirectXRaytracingSupported(ID3D12Device* testDevice)
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};
		return SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
			&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
	}

	inline void GetAdapter1(IDXGIFactory6* dxgiFactory, IDXGIAdapter1** ppAdapter, DXGI_ADAPTER_DESC1& adapterDesc)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); adapterIndex++)
		{
			adapterDesc = {};
			adapter->GetDesc1(&adapterDesc);

			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
		if (adapter == nullptr)
		{
			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++)
			{
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					adapterDesc = {};
					adapter->GetDesc1(&adapterDesc);

					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}
}