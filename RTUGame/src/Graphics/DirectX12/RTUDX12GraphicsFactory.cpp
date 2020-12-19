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
#include "RTUDX12GraphicsFactory.h"
#include "RTUDX12RootSignature.h"
#include "RTUDX12PipelineState.h"
#include "RTUDX12CBuffer.h"
#include "RTUDX12SBuffer.h"
#include "RTUDX12Mesh.h"
#include "RTUDX12Texture2D.h"
#include "RTUDX12RenderTexture2D.h"
#include "RTUDX12GraphicsDevice.h"
#include "RTUDX12GraphicsContext.h"

IRTUGraphicsDevice* RTUDX12GraphicsFactory::GetGraphicsDevice()
{
    return new RTUDX12GraphicsDevice();
}

IRTUGraphicsContext* RTUDX12GraphicsFactory::GetGraphicsContext()
{
    return new RTUDX12GraphicsContext();
}

IRTURenderTexture2D* RTUDX12GraphicsFactory::GetRenderTexture2D()
{
    return new RTUDX12RenderTexture2D();
}

IRTUMesh* RTUDX12GraphicsFactory::GetMesh()
{
    return new RTUDX12Mesh();
}

IRTUCBuffer* RTUDX12GraphicsFactory::GetCBuffer()
{
    return new RTUDX12CBuffer();
}

IRTUSBuffer* RTUDX12GraphicsFactory::GetSBuffer()
{
    return new RTUDX12SBuffer();
}

IRTUPipelineState* RTUDX12GraphicsFactory::GetPipelineState()
{
    return new RTUDX12PipelineState();
}

IRTUTexture2D* RTUDX12GraphicsFactory::GetTexture2D()
{
    return new RTUDX12Texture2D();
}

IRTURootSignature* RTUDX12GraphicsFactory::GetRootSignature()
{
    return new RTUDX12RootSignature();
}
