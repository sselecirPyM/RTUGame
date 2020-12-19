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
#include "../../Common/RTU_STATES.h"
using Microsoft::WRL::ComPtr;
class RTUDX12RenderTexture2D : public IRTURenderTexture2D
{
public:
	void AsDSV(DXGI_FORMAT format);
	void AsRTV(DXGI_FORMAT format);
	void AsRTVUAV(DXGI_FORMAT format);
	ComPtr<ID3D12Resource> m_texture;
	UINT m_srvRef;
	UINT m_rtvRef;
	UINT m_dsvRef;
	UINT m_uavRef;
	DXGI_FORMAT m_format;
	DXGI_FORMAT m_dsvFormat;
	DXGI_FORMAT m_rtvFormat;
	DXGI_FORMAT m_uavFormat;

	UINT m_width;
	UINT m_height;
	D3D12_RESOURCE_STATES m_previousResourceStates;
	D3D12_RESOURCE_FLAGS m_resourceFlags;
	RTU_STATES m_states;
};

