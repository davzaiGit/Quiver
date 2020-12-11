#pragma once
#include "PxPhysicsAPI.h"
#include "src/objload.h"

namespace Core
{
	physx::PxRigidDynamic* createDynamic(const physx::PxTransform &t, obj::Model mo, physx::PxPhysics* px, physx::PxCooking* pxcook);
}

