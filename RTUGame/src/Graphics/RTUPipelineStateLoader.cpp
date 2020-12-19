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
#include "RTUPipelineStateLoader.h"
#include "../DXUtil.h"

RTUPipelineStateLoader::RTUPipelineStateLoader()
{
}

RTUPipelineStateLoader::RTUPipelineStateLoader(LPCWSTR vs, LPCWSTR gs, LPCWSTR ps)
{
	Load(vs, gs, ps);
}

void RTUPipelineStateLoader::Load(LPCWSTR vs, LPCWSTR gs, LPCWSTR ps)
{
	if (vs != nullptr)
	{
		ThrowIfFailed(ReadDataFromFile(vs, m_vertexShader));
	}
	else
	{
		m_vertexShader.resize(0);
		m_vertexShader.shrink_to_fit();
	}
	if (gs != nullptr)
	{
		ThrowIfFailed(ReadDataFromFile(gs, m_geometryShader));
	}
	else
	{
		m_geometryShader.resize(0);
		m_geometryShader.shrink_to_fit();
	}
	if (ps != nullptr)
	{
		ThrowIfFailed(ReadDataFromFile(ps, m_pixelShader));
	}
	else
	{
		m_pixelShader.resize(0);
		m_pixelShader.shrink_to_fit();
	}
}
