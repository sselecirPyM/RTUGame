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
#include "RTUDX11GraphicsFactory.h"
#include "RTUDX11CBuffer.h"
#include "RTUDX11GraphicsDevice.h"
#include "RTUDX11GraphicsContext.h"
#include "RTUDX11Mesh.h"
#include "RTUDX11PipelineState.h"
#include "RTUDX11RenderTexture2D.h"
#include "RTUDX11RootSignature.h"
#include "RTUDX11SBuffer.h"
#include "RTUDX11Texture2D.h"

IRTUGraphicsDevice* RTUDX11GraphicsFactory::GetGraphicsDevice()
{
	return new RTUDX11GraphicsDevice();
}

IRTUGraphicsContext* RTUDX11GraphicsFactory::GetGraphicsContext()
{
	return new RTUDX11GraphicsContext();
}

IRTURenderTexture2D* RTUDX11GraphicsFactory::GetRenderTexture2D()
{
	return new RTUDX11RenderTexture2D();
}

IRTUMesh* RTUDX11GraphicsFactory::GetMesh()
{
	return new RTUDX11Mesh();
}

IRTUCBuffer* RTUDX11GraphicsFactory::GetCBuffer()
{
	return new RTUDX11CBuffer();
}

IRTUSBuffer* RTUDX11GraphicsFactory::GetSBuffer()
{
	return new RTUDX11SBuffer();
}

IRTUPipelineState* RTUDX11GraphicsFactory::GetPipelineState()
{
	return new RTUDX11PipelineState();
}

IRTUTexture2D* RTUDX11GraphicsFactory::GetTexture2D()
{
	return new RTUDX11Texture2D();
}

IRTURootSignature* RTUDX11GraphicsFactory::GetRootSignature()
{
	return new RTUDX11RootSignature();
}
