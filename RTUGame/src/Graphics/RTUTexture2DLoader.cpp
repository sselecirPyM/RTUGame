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
#include "RTUTexture2DLoader.h"
#include "../DXUtil.h"
#include <wincodec.h>
#include <DirectXTex.h>
using namespace DirectX;

inline uint32_t CountMips(_In_ uint32_t width, _In_ uint32_t height) noexcept
{
	uint32_t mipLevels = 1;

	while (height > 1 || width > 1)
	{
		if (height > 1)
			height >>= 1;

		if (width > 1)
			width >>= 1;

		++mipLevels;
	}

	return mipLevels;
}

inline void _Fun4(RTUTexture2DLoader* tex, DirectX::TexMetadata& metaData, DirectX::ScratchImage& scratchImage, bool srgb, bool generateMips)
{
	DirectX::ScratchImage generatedMips;
	if (srgb)
		tex->m_format = DirectX::MakeSRGB(metaData.format);
	else
		tex->m_format = metaData.format;

	tex->m_width = metaData.width;
	tex->m_height = metaData.height;
	if (generateMips)
		tex->m_mipLevels = std::max<int>(CountMips(tex->m_width, tex->m_height) - 6, 1);
	else
		tex->m_mipLevels = 1;

	if (tex->m_mipLevels > 1)
	{
		ThrowIfFailed(DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_MIRROR | DirectX::TEX_FILTER_SEPARATE_ALPHA, tex->m_mipLevels, generatedMips));

		tex->m_data.resize(generatedMips.GetPixelsSize());
		memcpy(tex->m_data.data(), generatedMips.GetPixels(), generatedMips.GetPixelsSize());
	}
	else
	{
		tex->m_data.resize(scratchImage.GetPixelsSize());
		memcpy(tex->m_data.data(), scratchImage.GetPixels(), scratchImage.GetPixelsSize());
	}
}

void RTUTexture2DLoader::LoadTexture(std::vector<char>& image, RTU_TEXTURE2D_LOAD_FLAGS flags)
{
	bool srgb = flags & RTU_TEXTURE2D_LOAD_FLAGS_SRGB;
	bool mipmap = flags & RTU_TEXTURE2D_LOAD_FLAGS_MIPMAP;
	DirectX::TexMetadata metaData;
	DirectX::ScratchImage scratchImage;

	HRESULT hr1 = DirectX::GetMetadataFromTGAMemory(image.data(), image.size(), metaData);
	if (SUCCEEDED(hr1))
	{
		ThrowIfFailed(DirectX::LoadFromTGAMemory(image.data(), image.size(), &metaData, scratchImage));
		_Fun4(this, metaData, scratchImage, srgb, mipmap);
		return;
	}

	HRESULT hr2 = DirectX::GetMetadataFromHDRMemory(image.data(), image.size(), metaData);
	if (SUCCEEDED(hr2))
	{
		ThrowIfFailed(DirectX::LoadFromHDRMemory(image.data(), image.size(), &metaData, scratchImage));
		_Fun4(this, metaData, scratchImage, srgb, mipmap);
		return;
	}

	HRESULT hr3 = DirectX::GetMetadataFromWICMemory(image.data(), image.size(), DirectX::WIC_FLAGS_NONE, metaData);
	if (SUCCEEDED(hr3))
	{
		ThrowIfFailed(DirectX::LoadFromWICMemory(image.data(), image.size(), DirectX::WIC_FLAGS_NONE, &metaData, scratchImage));
		_Fun4(this, metaData, scratchImage, srgb, mipmap);
		return;
	}
}

void RTUTexture2DLoader::LoadTexture(LPCWSTR path, RTU_TEXTURE2D_LOAD_FLAGS flags)
{
	std::vector<char> data;
	ReadDataFromFile(path, data);
	LoadTexture(data, flags);
}

void RTUTexture2DLoader::Pure(glm::vec4 color, uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;
	m_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_mipLevels = 1;

	int count = width * height;
	m_data.resize(count * DirectX::BitsPerPixel(m_format) / 8);


	void* p = m_data.data();
	glm::vec4* p1 = (glm::vec4*)p;
	for (int i = 0; i < count; i++) {
		p1[i] = color;
	}
}
