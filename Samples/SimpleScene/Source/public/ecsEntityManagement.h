#pragma once

#include <flecs.h>

struct MarkedForDestruction
{
	bool justPlaced = true;
};

void RegisterEcsEntityMgmtSystems(flecs::world& world);

void ConsciousDestroy(flecs::entity e);
