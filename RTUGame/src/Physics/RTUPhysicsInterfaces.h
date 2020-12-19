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
class IRTUPhysicsRigidBody
{
public:
	virtual ~IRTUPhysicsRigidBody() {};
};
class IRTUPhysicsStatic
{
public:
	virtual ~IRTUPhysicsStatic() {};
};
class IRTUPhysicsScene
{
public:
	virtual ~IRTUPhysicsScene() {};
};
class IRTUPhysicsController
{
public:
	virtual ~IRTUPhysicsController() {};
};
class IRTUPhysicsDevice
{
public:
	virtual void Init() = 0;
	virtual void InitScene(IRTUPhysicsScene* scene) = 0;
	virtual void SceneAddController(IRTUPhysicsScene* scene, IRTUPhysicsController* controller, glm::vec3 position, glm::vec4 rotation) = 0;
	virtual void SceneAddStatic(IRTUPhysicsScene* scene, IRTUPhysicsStatic* rigidStatic, glm::vec3 position, glm::vec4 rotation) = 0;
	virtual void SceneMoveController(IRTUPhysicsScene* scene, IRTUPhysicsController* controller, glm::vec3 delta, float elapsedTime) = 0;
	virtual void SceneSimulate(IRTUPhysicsScene* scene, double time) = 0;
	virtual void ScenePlaceRigidBody(IRTUPhysicsScene* scene, IRTUPhysicsRigidBody* rigidBody, glm::vec3 position, glm::vec4 rotation) = 0;
	virtual void SceneRemoveRigidBody(IRTUPhysicsScene* scene, IRTUPhysicsRigidBody* rigidBody) = 0;
	virtual void SceneShiftOrigin(IRTUPhysicsScene* scene, glm::vec3 position) = 0;
	virtual void GetControllerTransform(IRTUPhysicsController* controller, glm::vec3* position, glm::vec4* rotation) = 0;
	virtual void GetRigidBodyTransform(IRTUPhysicsRigidBody* rigidBody, glm::vec3* position, glm::vec4* rotation) = 0;
	virtual void GetTriangleMesh(IRTUPhysicsStatic* rigidStatic, void* vert, int vertCount, int vStride, void* index, int indexCount) = 0;
	virtual ~IRTUPhysicsDevice() {};
};
class IRTUPhysicsFactory
{
public:
	virtual IRTUPhysicsDevice* GetDevice() = 0;
	virtual IRTUPhysicsScene* GetScene() = 0;
	virtual IRTUPhysicsRigidBody* GetRigidBody() = 0;
	virtual IRTUPhysicsStatic* GetRigidBodyStatic() = 0;
	virtual IRTUPhysicsController* GetController() = 0;
	virtual ~IRTUPhysicsFactory() {};
};