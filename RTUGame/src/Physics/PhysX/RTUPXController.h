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
#include "../RTUPhysicsInterfaces.h"
#include <PxPhysicsAPI.h>
class RTUPXController : public IRTUPhysicsController
{
public:
	physx::PxController* m_controller = nullptr;
	physx::PxMaterial* m_material = nullptr;
	physx::PxReal m_height = 1.60f;
	physx::PxReal m_radius = 0.4f;
	~RTUPXController();
};

