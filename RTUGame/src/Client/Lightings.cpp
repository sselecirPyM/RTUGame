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
#include "Lightings.h"
using namespace DirectX;

void DirectLight2::GetLightMatrix(XMFLOAT3 position, XMFLOAT3 cameraAngle, float maxDistance,XMFLOAT4X4* matrixOutput, XMFLOAT4X4* vMatrix, XMFLOAT4X4* pMatrix, int Count)
{
	XMVECTOR cameraPosition = XMLoadFloat3(&position);
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);
	XMFLOAT3 forward1 = XMFLOAT3(0, 0, 1);
	XMFLOAT3 up1 = XMFLOAT3(0, 1, 0);
	XMVECTOR forward = XMLoadFloat3(&forward1);
	XMVECTOR up = XMLoadFloat3(&up1);
	auto v = XMMatrixLookAtLH(cameraPosition + XMVector3TransformNormal(forward, rotateMatrix) * 256, cameraPosition, XMVector3TransformNormal(up, rotateMatrix));
	auto p = XMMatrixOrthographicLH(32, 32, 1, 512);
	XMStoreFloat4x4(matrixOutput, XMMatrixTranspose(v * p));
	XMStoreFloat4x4(vMatrix, v);
	XMStoreFloat4x4(pMatrix, p);
}
