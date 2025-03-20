#include <ecsEntityManagement.h>

void RegisterEcsEntityMgmtSystems(flecs::world& world)
{
	world.system<CanBeDestroyed>()
		.each([&](flecs::entity e, CanBeDestroyed& marker)
	{
		if (!marker.beingDestroyed) return;

		if (!marker.thisTick)
		{
			marker.thisTick = true;
		}
		else
		{
			e.destruct();
		}
	});
}

void ConsciousDestroy(flecs::entity e)
{
	assert(e.has<CanBeDestroyed>());
	e.get_mut<CanBeDestroyed>()->beingDestroyed = true;
}
