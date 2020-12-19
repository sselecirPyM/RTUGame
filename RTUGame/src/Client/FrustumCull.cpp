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
#include "FrustumCull.h"
using namespace DirectX;

void FrustumCull::Init(CXMMATRIX view, CXMMATRIX projection)
{
	DirectX::BoundingFrustum temp;
	BoundingFrustum::CreateFromMatrix(temp, projection);
	temp.Transform(m_frustum, XMMatrixInverse(nullptr, view));

}

bool FrustumCull::SphereIntersects(DirectX::FXMVECTOR position, float radius)
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, position);
	BoundingSphere sphere = BoundingSphere(pos, radius);
	auto ret = m_frustum.Contains(sphere);
	return ret != ContainmentType::DISJOINT;
}

bool FrustumCull::BoxIntersects(DirectX::FXMVECTOR middle, DirectX::XMFLOAT3 halflwh)
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, middle);
	BoundingBox tbox = BoundingBox(pos, halflwh);
	auto ret = m_frustum.Contains(tbox);
	return ret != ContainmentType::DISJOINT;
}

bool FrustumCull::BoxContains(DirectX::FXMVECTOR middle, DirectX::XMFLOAT3 halflwh)
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, middle);
	BoundingBox tbox = BoundingBox(pos, halflwh);
	auto ret = m_frustum.Contains(tbox);
	return ret == ContainmentType::CONTAINS;
}
