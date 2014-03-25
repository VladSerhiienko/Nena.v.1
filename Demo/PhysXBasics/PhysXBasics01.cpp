
#include <PxPhysicsAPI.h>

int main(int argc, char *argv [])
{
	physx::PxDefaultErrorCallback errorCallback;
	physx::PxDefaultAllocator allocCallback;

	physx::PxFoundation *foundation = ::PxCreateFoundation(PX_PHYSICS_VERSION, allocCallback, errorCallback);
	if (!foundation) return 1;

	physx::PxProfileZoneManager *zoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(foundation);
	if (!zoneManager) return 1;

	physx::PxPhysics *physics = ::PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, zoneManager);
	if (!physics) return 1;

	physx::PxCooking *cooking = ::PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!physics) return 1;

	cooking->release();
	physics->release();
	zoneManager->release();
	foundation->release();

	return 0;
}
