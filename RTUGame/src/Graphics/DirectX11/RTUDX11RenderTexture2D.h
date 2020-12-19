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
class RTUDX11RenderTexture2D : public IRTURenderTexture2D
{
public:
	void AsDSV(DXGI_FORMAT format);
	void AsRTV(DXGI_FORMAT format);
	void AsRTVUAV(DXGI_FORMAT format);

	UINT m_width;
	UINT m_height;

	DXGI_FORMAT m_format;
	DXGI_FORMAT m_srvformat;
	DXGI_FORMAT m_dsvFormat;
	DXGI_FORMAT m_rtvFormat;
	DXGI_FORMAT m_uavFormat;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_unorderedAccessView;
};

