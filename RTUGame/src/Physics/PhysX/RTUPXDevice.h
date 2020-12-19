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
#include <glm/glm.hpp>
class RTUPXDevice : public IRTUPhysicsDevice
{
public:
	void Init();
	void InitScene(IRTUPhysicsScene* scene);
	void SceneAddController(IRTUPhysicsScene* scene, IRTUPhysicsController* controller, glm::vec3 position, glm::vec4 rotation);
	void SceneAddStatic(IRTUPhysicsScene* scene,IRTUPhysicsStatic* rigidStatic, glm::vec3 position, glm::vec4 rotation);
	void SceneMoveController(IRTUPhysicsScene* scene, IRTUPhysicsController* controller, glm::vec3 delta, float elapsedTime);
	void SceneRemoveStatic(IRTUPhysicsScene* scene,IRTUPhysicsStatic* rigidStatic);
	void SceneSimulate(IRTUPhysicsScene* scene, double time);
	void ScenePlaceRigidBody(IRTUPhysicsScene* scene, IRTUPhysicsRigidBody* rigidBody, glm::vec3 position, glm::vec4 rotation);
	void SceneRemoveRigidBody(IRTUPhysicsScene* scene, IRTUPhysicsRigidBody* rigidBody);
	void SceneShiftOrigin(IRTUPhysicsScene* scene, glm::vec3 position);
	void GetControllerTransform(IRTUPhysicsController* controller, glm::vec3* position, glm::vec4* rotation);
	void GetRigidBodyTransform(IRTUPhysicsRigidBody* rigidBody, glm::vec3* position, glm::vec4* rotation);

	void GetTriangleMesh(IRTUPhysicsStatic* rigidStatic, void* vert, int vertCount,int vStride, void* index, int indexCount);

	~RTUPXDevice();
	physx::PxFoundation* m_foundation = nullptr;
	physx::PxPhysics* m_physics = nullptr;
	physx::PxCooking* m_cooking = nullptr;
	physx::PxPvd* m_pvd = nullptr;
	physx::PxDefaultCpuDispatcher* m_defaultCpuDispatcher = nullptr;

	physx::PxDefaultAllocator m_allocator;
	physx::PxDefaultErrorCallback m_errorCallback;
	physx::PxTolerancesScale m_tolerancesScale;
};

