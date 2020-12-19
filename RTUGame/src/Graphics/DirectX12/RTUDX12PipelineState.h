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
#include "../RTUPSODesc.h"
class RTUDX12PipelineState : public IRTUPipelineState
{
public:
	ID3D12PipelineState* GetPipelineState(RTUPipelineStateDesc* desc);
	std::vector<char> m_vertexShader;
	std::vector<char> m_geometryShader;
	std::vector<char> m_pixelShader;
	std::vector<ComPtr<ID3D12PipelineState>> m_pipelineStates;
	std::vector<RTUPipelineStateDesc> m_psoDescs;
	D3D12_PRIMITIVE_TOPOLOGY m_primitiveType;
};
