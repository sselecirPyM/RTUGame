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
#include "RTUVKGraphicsFactory.h"
#include "RTUVKGraphicsContext.h"
#include "RTUVKGraphicsDevice.h"
#include "RTUVKCBuffer.h"
#include "RTUVKMesh.h"
#include "RTUVKPipelineState.h"
#include "RTUVKRenderTexture2D.h"
#include "RTUVKRootSignature.h"
#include "RTUVKSBuffer.h"
#include "RTUVKTexture2D.h"

IRTUGraphicsDevice* RTUVKGraphicsFactory::GetGraphicsDevice()
{
    return new RTUVKGraphicsDevice();
}

IRTUGraphicsContext* RTUVKGraphicsFactory::GetGraphicsContext()
{
    return new RTUVKGraphicsContext();
}

IRTURenderTexture2D* RTUVKGraphicsFactory::GetRenderTexture2D()
{
    return new RTUVKRenderTexture2D();
}

IRTUMesh* RTUVKGraphicsFactory::GetMesh()
{
    return new RTUVKMesh();
}

IRTUCBuffer* RTUVKGraphicsFactory::GetCBuffer()
{
    return new RTUVKCBuffer();
}

IRTUSBuffer* RTUVKGraphicsFactory::GetSBuffer()
{
    return new RTUVKSBuffer();
}

IRTUPipelineState* RTUVKGraphicsFactory::GetPipelineState()
{
    return new RTUVKPipelineState();
}

IRTUTexture2D* RTUVKGraphicsFactory::GetTexture2D()
{
    return new RTUVKTexture2D();
}

IRTURootSignature* RTUVKGraphicsFactory::GetRootSignature()
{
    return new RTUVKRootSignature();
}
