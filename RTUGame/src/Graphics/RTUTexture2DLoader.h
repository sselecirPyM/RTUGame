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
#include <glm/glm.hpp>
using Microsoft::WRL::ComPtr;
enum RTU_TEXTURE2D_LOAD_FLAGS : uint32_t
{
	RTU_TEXTURE2D_LOAD_FLAGS_NONE = 0,
	RTU_TEXTURE2D_LOAD_FLAGS_SRGB = 1,
	RTU_TEXTURE2D_LOAD_FLAGS_MIPMAP = 2,
};
class RTUTexture2DLoader
{
public:
	void LoadTexture(std::vector<char>& image, RTU_TEXTURE2D_LOAD_FLAGS flags);
	void LoadTexture(LPCWSTR path, RTU_TEXTURE2D_LOAD_FLAGS flags);
	void Pure(glm::vec4 color, uint32_t width, uint32_t height);
	ComPtr<ID3D12Resource> m_texture;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_mipLevels;
	DXGI_FORMAT m_format;

	std::vector<byte> m_data;
};

