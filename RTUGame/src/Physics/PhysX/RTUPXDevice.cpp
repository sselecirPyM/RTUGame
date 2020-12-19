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
#include "RTUPXDevice.h"
#include "RTUPXScene.h"
#include "RTUPXRigidBody.h"
#include "RTUPXController.h"
#include "RTUPXStatic.h"
using namespace physx;

void RTUPXDevice::Init()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
#ifdef _DEBUG
#define PVD_HOST "127.0.0.1"
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pvd = PxCreatePvd(*m_foundation);
	m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
	m_tolerancesScale = PxTolerancesScale();

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_tolerancesScale, true, m_pvd);
	PxInitExtensions(*m_physics, m_pvd);
	m_defaultCpuDispatcher = PxDefaultCpuDispatcherCreate(4);
	PxCookingParams cookingParams(m_tolerancesScale);
	cookingParams.meshPreprocessParams|= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	cookingParams.meshPreprocessParams|= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, cookingParams);
}

void RTUPXDevice::InitScene(IRTUPhysicsScene* _scene)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -9.81, 0);
	sceneDesc.cpuDispatcher = m_defaultCpuDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene->m_scene = m_physics->createScene(sceneDesc);
	//PxPvdSceneClient* pvdClient = scene->m_scene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	scene->m_controllerManager = PxCreateControllerManager(*scene->m_scene);
	scene->m_controllerManager->setOverlapRecoveryModule(false);

}

void RTUPXDevice::SceneAddController(IRTUPhysicsScene* _scene, IRTUPhysicsController* _controller, glm::vec3 position, glm::vec4 rotation)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXController* controller = static_cast<RTUPXController*>(_controller);
	controller->m_material = m_physics->createMaterial(0.5f, 0.5f, 0.2f);

	PxCapsuleControllerDesc cdesc;
	cdesc.radius = controller->m_radius;
	cdesc.height = controller->m_height;
	cdesc.climbingMode = PxCapsuleClimbingMode::eEASY;
	cdesc.position = PxExtendedVec3(position.x, position.y, position.z);
	cdesc.material = controller->m_material;
	controller->m_controller = scene->m_controllerManager->createController(cdesc);
}

void RTUPXDevice::SceneAddStatic(IRTUPhysicsScene* _scene, IRTUPhysicsStatic* _rigidStatic, glm::vec3 position, glm::vec4 rotation)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXStatic* rigidStatic = static_cast<RTUPXStatic*>(_rigidStatic);
	auto& p = position;
	auto& q = rotation;
	rigidStatic->m_rigidStatic->setGlobalPose(PxTransform(p.x, p.y, p.z, PxQuat(q.x, q.y, q.z, q.w)));
	scene->m_scene->addActor(*rigidStatic->m_rigidStatic);
}

void RTUPXDevice::SceneMoveController(IRTUPhysicsScene* _scene, IRTUPhysicsController* _controller, glm::vec3 delta, float elapsedTime)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXController* controller = static_cast<RTUPXController*>(_controller);
	PxControllerFilters filters;
	controller->m_controller->move(PxVec3(delta.x, delta.y, delta.z), 0.01, elapsedTime, filters);
}

void RTUPXDevice::SceneRemoveStatic(IRTUPhysicsScene* _scene, IRTUPhysicsStatic* _rigidStatic)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXStatic* rigidStatic = static_cast<RTUPXStatic*>(_rigidStatic);
	rigidStatic->~RTUPXStatic();
}

void RTUPXDevice::SceneSimulate(IRTUPhysicsScene* _scene, double time)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	scene->m_scene->simulate(static_cast<PxReal>(time));
	scene->m_scene->fetchResults(true);
}

void RTUPXDevice::ScenePlaceRigidBody(IRTUPhysicsScene* _scene, IRTUPhysicsRigidBody* _rigidBody, glm::vec3 position, glm::vec4 rotation)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXRigidBody* rigidBody = static_cast<RTUPXRigidBody*>(_rigidBody);
	auto& p = position;
	auto& q = rotation;
	rigidBody->m_rigidDynamic->setGlobalPose(PxTransform(p.x, p.y, p.z, PxQuat(q.x, q.y, q.z, q.w)));
}

void RTUPXDevice::SceneRemoveRigidBody(IRTUPhysicsScene* _scene, IRTUPhysicsRigidBody* _rigidBody)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	RTUPXRigidBody* rigidBody = static_cast<RTUPXRigidBody*>(_rigidBody);
	assert(rigidBody->m_rigidDynamic);
	rigidBody->m_rigidDynamic->release();
	rigidBody->m_physicsMaterial->release();
	rigidBody->m_rigidDynamic = nullptr;
	rigidBody->m_physicsMaterial = nullptr;
}

void RTUPXDevice::SceneShiftOrigin(IRTUPhysicsScene* _scene, glm::vec3 position)
{
	RTUPXScene* scene = static_cast<RTUPXScene*>(_scene);
	auto& p = position;
	scene->m_scene->shiftOrigin(PxVec3(p.x, p.y, p.z));
	scene->m_controllerManager->shiftOrigin(PxVec3(p.x, p.y, p.z));
}

void RTUPXDevice::GetControllerTransform(IRTUPhysicsController* _controller, glm::vec3* position, glm::vec4* rotation)
{
	RTUPXController* controller = static_cast<RTUPXController*>(_controller);
	auto _pos=controller->m_controller->getPosition();
	position->x = _pos.x;
	position->y = _pos.y;
	position->z = _pos.z;
}

void RTUPXDevice::GetRigidBodyTransform(IRTUPhysicsRigidBody* _rigidBody, glm::vec3* position, glm::vec4* rotation)
{
	RTUPXRigidBody* rigidBody = static_cast<RTUPXRigidBody*>(_rigidBody);
	auto pose = rigidBody->m_rigidDynamic->getGlobalPose();
	position->x = pose.p.x;
	position->y = pose.p.y;
	position->z = pose.p.z;
	rotation->x = pose.q.x;
	rotation->y = pose.q.y;
	rotation->z = pose.q.z;
	rotation->w = pose.q.w;
}
void RTUPXDevice::GetTriangleMesh(IRTUPhysicsStatic* _rigidStatic, void* vert, int vertCount, int vStride, void* index, int indexCount)
{
	RTUPXStatic* rigidStatic = static_cast<RTUPXStatic*>(_rigidStatic);
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertCount;
	meshDesc.points.stride = vStride;
	meshDesc.points.data = vert;

	meshDesc.triangles.count = indexCount/3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = index;
	rigidStatic->m_triangleMesh = m_cooking->createTriangleMesh(meshDesc, m_physics->getPhysicsInsertionCallback());
	rigidStatic->m_physicsMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.1f);

	PxTriangleMeshGeometry geometry;
	geometry.triangleMesh = rigidStatic->m_triangleMesh;
	rigidStatic->m_rigidStatic = PxCreateStatic(*m_physics,PxTransform(0,0,0,PxQuat(0,0,0,1)) , geometry, *rigidStatic->m_physicsMaterial);
}
template<class _classA>
inline void _FreePx(_classA*& v)
{
	if (v)
	{
		v->release();
		v = nullptr;
	}
}
RTUPXDevice::~RTUPXDevice()
{
	_FreePx(m_foundation);
	_FreePx(m_physics);
	_FreePx(m_cooking);
	_FreePx(m_pvd);
	_FreePx(m_defaultCpuDispatcher);
}
