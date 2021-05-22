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
enum CHUNK_FLAGS : std::uint32_t
{
	CHUNK_FLAGS_NONE = 0,
	CHUNK_FLAGS_OCClUSION_LEFT = 1,
	CHUNK_FLAGS_OCClUSION_RIGHT = 2,
	CHUNK_FLAGS_OCClUSION_UP = 4,
	CHUNK_FLAGS_OCClUSION_DOWN = 8,
	CHUNK_FLAGS_OCClUSION_FRONT = 16,
	CHUNK_FLAGS_OCClUSION_BACK = 32,
	CHUNK_FLAGS_EMPTY = 64,
};
struct Chunk16
{
	glm::i32vec3 m_position;
	CHUNK_FLAGS m_flags;
	std::uint64_t m_serverFrameStamp;
	std::uint64_t m_unknow0;
	std::uint32_t m_data[16][16][16];//z,y,x

	const static int c_size = 16;
	const static int c_headerSize = 64;
	const static int c_minimumSize = 16448;
};
