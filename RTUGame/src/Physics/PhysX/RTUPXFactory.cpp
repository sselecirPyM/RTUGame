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
#include "RTUPXFactory.h"
#include "RTUPXDevice.h"
#include "RTUPXScene.h"
#include "RTUPXRigidBody.h"
#include "RTUPXController.h"
#include "RTUPXStatic.h"

IRTUPhysicsDevice* RTUPXFactory::GetDevice()
{
	return new RTUPXDevice();
}

IRTUPhysicsScene* RTUPXFactory::GetScene()
{
	return new RTUPXScene();
}

IRTUPhysicsRigidBody* RTUPXFactory::GetRigidBody()
{
	return new RTUPXRigidBody();
}

IRTUPhysicsStatic* RTUPXFactory::GetRigidBodyStatic()
{
	return new RTUPXStatic();
}

IRTUPhysicsController* RTUPXFactory::GetController()
{
	return new RTUPXController();
}
