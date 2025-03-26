#pragma once

#include <flecs.h>

struct Visibility
{
	bool visible;
};

void RegisterEcsMeshSystems(flecs::world& world);
