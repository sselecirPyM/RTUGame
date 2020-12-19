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
#include "../Graphics/RTUGraphicsInterfaces.h"
class RenderPipelineResources
{
public:
	void Init(IRTUGraphicsFactory* factory);
	std::unique_ptr<IRTUPipelineState> m_deferred_gbuffer;
	std::unique_ptr<IRTUPipelineState> m_deferred_IBL;
	std::unique_ptr<IRTUPipelineState> m_deferred_directLight;
	std::unique_ptr<IRTUPipelineState> m_shadow;
	std::unique_ptr<IRTUPipelineState> m_postprocess;
};

