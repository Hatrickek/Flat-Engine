#include "physics.h"

#include "physx/physx/include/PxPhysicsAPI.h"

#define PX_RELEASE(x)	if(x)	{ (x)->release(); (x) = NULL;	}

namespace FlatEngine {
	/* static physx::PxDefaultAllocator		gAllocator; */
	/* static physx::PxDefaultErrorCallback	gErrorCallback; */
	/* static physx::PxFoundation*			gFoundation = NULL; */
	/* static physx::PxPhysics*				gPhysics	= NULL; */
	/* static physx::PxDefaultCpuDispatcher*	gDispatcher = NULL; */
	/* static physx::PxScene*					gScene		= NULL; */
	/* static physx::PxMaterial*				gMaterial	= NULL; */
	/* static physx::PxPvd*                  	gPvd        = NULL; */
	/* static physx::PxReal 					stackZ = 10.0f; */

	//void Physics::InitPhysics() {
	//
	//
	//	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	//	gPvd = PxCreatePvd(*gFoundation);
	//	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	//	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	//
	//	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(),true,gPvd);
	//
	//	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	//	if(pvdClient)
	//	{
	//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//	}
	//	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	//
	//	physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0,1,0,0), *gMaterial);
	//	gScene->addActor(*groundPlane);
	//
	//	for(physx::PxU32 i=0;i<5;i++)
	//		createStack(physx::PxTransform(physx::PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);
	//
	//	//if(!interactive)
	//		createDynamic(physx::PxTransform(physx::PxVec3(0,40,100)), physx::PxSphereGeometry(10), physx::PxVec3(0,-50,-100));
	//}
	//
	//void Physics::CleanupPhysics()
	//{
	//	PX_RELEASE(gScene)
	//	PX_RELEASE(gDispatcher)
	//	PX_RELEASE(gPhysics)
	//	if(gPvd)
	//	{
	//		physx::PxPvdTransport* transport = gPvd->getTransport();
	//		gPvd->release();	gPvd = NULL;
	//		PX_RELEASE(transport)
	//	}
	//	PX_RELEASE(gFoundation)
	//
	//	printf("SnippetHelloWorld done.\n");
	//}
}