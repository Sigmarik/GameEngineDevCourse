#include <ecsEntityManagement.h>

void RegisterEcsEntityMgmtSystems(flecs::world& world)
{
	world.system<MarkedForDestruction>()
		.each([&](flecs::entity e, MarkedForDestruction& marker)
	{
		if (marker.justPlaced)
		{
			marker.justPlaced = false;
		}
		else
		{
			e.destruct();
		}
	});
}

void ConsciousDestroy(flecs::entity e)
{
	if (!e.has<MarkedForDestruction>()) e.set(MarkedForDestruction{});
}
