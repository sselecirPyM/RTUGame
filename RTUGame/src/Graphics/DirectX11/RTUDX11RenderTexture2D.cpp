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
#include "RTUDX11RenderTexture2D.h"

void RTUDX11RenderTexture2D::AsDSV(DXGI_FORMAT format)
{
	if (format == DXGI_FORMAT_D24_UNORM_S8_UINT)
	{
		m_format = DXGI_FORMAT_R24G8_TYPELESS;
		m_srvformat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	}
	else if (format == DXGI_FORMAT_D32_FLOAT)
	{
		m_format = DXGI_FORMAT_R32_FLOAT;
		m_srvformat = format;
	}
	m_dsvFormat = format;
	m_rtvFormat = DXGI_FORMAT_UNKNOWN;
	m_uavFormat = DXGI_FORMAT_UNKNOWN;
}

void RTUDX11RenderTexture2D::AsRTV(DXGI_FORMAT format)
{
	m_format = format;
	m_srvformat = format;
	m_dsvFormat = DXGI_FORMAT_UNKNOWN;
	m_rtvFormat = format;
	m_uavFormat = DXGI_FORMAT_UNKNOWN;
}

void RTUDX11RenderTexture2D::AsRTVUAV(DXGI_FORMAT format)
{
	m_format = format;
	m_srvformat = format;
	m_dsvFormat = DXGI_FORMAT_UNKNOWN;
	m_rtvFormat = format;
	m_uavFormat = format;
}
