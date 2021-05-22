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
#include "Lightings.h"
struct GraphicsCameraData
{
	DirectX::XMFLOAT4X4 vpMatrix;
	DirectX::XMFLOAT4X4 pvMatrix;
	DirectX::XMFLOAT3 camPos_f;
	float aspectRatio;
	glm::i32vec3 camPos_i;
	UINT unknow1;
	DirectX::XMFLOAT4X4 vMatrix;
	DirectX::XMFLOAT4X4 pMatrix;
};
struct InputDesc1
{
	std::uint64_t timeStamp;
	bool pressed;
};
class ClientPlayer
{
public:
	void Init();
	glm::i32vec3 m_position_i = {};
	float m_FOV = 1.047197551196f;//60 degree
	DirectX::XMFLOAT3 m_position_f = { 0.5f,60,-1 };
	float m_clipFar = 512.0f;
	DirectX::XMFLOAT3 m_angle = { 1.047197551196f ,0,0 };
	float m_clipNear = 0.3f;
	DirectX::XMFLOAT3 m_currentSpeed;
	DirectX::XMFLOAT4 m_rotation = { 0, 0, 0, 1 };
	GraphicsCameraData GetCameraData(float aspectRatio);

	DirectLight2 mainLight;

	InputDesc1 left;
	InputDesc1 up;
	InputDesc1 right;
	InputDesc1 down;
	InputDesc1 space;

	bool mouseLPressed = false;
	bool mouseRPressed = false;
	DirectX::XMUINT2 mousePosition;
};

