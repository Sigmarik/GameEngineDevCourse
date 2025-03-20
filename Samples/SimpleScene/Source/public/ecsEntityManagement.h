#pragma once

#include <flecs.h>

#undef ECS_META_IMPL
#ifndef GAME_FRAMEWORK
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

// There are no words to describe how much I hate this solution to
// the lack of destructors...

ECS_STRUCT(CanBeDestroyed,
{
	bool thisTick;  // = false;
	bool beingDestroyed;  // = false;
});

void RegisterEcsEntityMgmtSystems(flecs::world& world);

void ConsciousDestroy(flecs::entity e);
