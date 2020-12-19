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
struct DirectLight
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 LightDirection;
	float unknow1;
};
struct DirectLight2
{
	DirectX::XMFLOAT3 Angle;
	float unknow1;
	DirectLight Light;
	void GetLightMatrix(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 CameraAngle, float maxDistance, DirectX::XMFLOAT4X4* matrixOutput, DirectX::XMFLOAT4X4* vMatrix, DirectX::XMFLOAT4X4* pMatrix, int Count);
};
struct PointLight
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 LightPosition;
	float Range;
};