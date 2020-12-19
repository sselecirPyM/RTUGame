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
#include "ClientPlayer.h"
using namespace DirectX;

void ClientPlayer::Init()
{
	mainLight.Angle = XMFLOAT3(-1, -1, 0);
	auto rotateMatrix = XMMatrixRotationRollPitchYaw(mainLight.Angle.x, mainLight.Angle.y, mainLight.Angle.z);
	XMVECTOR forward = XMVECTORF32{ 0, 0, 1 };
	XMStoreFloat3(&mainLight.Light.LightDirection, XMVector3TransformCoord(forward, rotateMatrix));
	mainLight.Light.Color = { 1,1,1,1 };
}

GraphicsCameraData ClientPlayer::GetCameraData(float aspectRatio)
{
	GraphicsCameraData data;
	data.aspectRatio = aspectRatio;
	data.camPos_f = m_position_f;
	data.camPos_i = m_position_i;
	XMMATRIX RotateMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation));
	auto forawrd1 = XMVECTORF32{ 0, 0, 1 };
	auto up1 = XMVECTORF32{ 0, 1, 0 };
	auto pos1 = XMLoadFloat3(&m_position_f);
	XMVECTOR forward = XMVector3TransformCoord(forawrd1, RotateMatrix) + pos1;
	XMVECTOR up = XMVector3TransformCoord(up1, RotateMatrix);

	XMMATRIX vMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_position_f), forward, up);
	XMMATRIX pMatrix = XMMatrixPerspectiveFovLH(m_FOV, aspectRatio, m_clipNear, m_clipFar);
	XMMATRIX vpMatrix = vMatrix * pMatrix;
	XMStoreFloat4x4(&data.vpMatrix, XMMatrixTranspose(vpMatrix));

	XMStoreFloat4x4(&data.vMatrix, vMatrix);
	XMStoreFloat4x4(&data.pMatrix, pMatrix);

	XMVECTOR Determinant;
	XMMATRIX inv = XMMatrixInverse(&Determinant, vpMatrix);
	XMStoreFloat4x4(&data.pvMatrix, XMMatrixTranspose(inv));
	return data;
}
