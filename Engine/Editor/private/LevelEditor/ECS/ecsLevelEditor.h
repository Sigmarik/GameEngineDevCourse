#pragma once

#include <Level.h>
#include <flecs.h>

#include <EditorECS/ecsEditor.h>

namespace GameEngine::EntitySystem::LevelEditorECS
{
	struct PositionDesc
	{
		// A pointer that SHOULD behave like a shared pointer without a
		// shared pointer is a non-working pointer.
		// The thing breaks if the vector decides to reallocate itself.
		// 
		// const World::LevelObject::ComponentDesc* value;

		// I replaced a pointer with object/component identifiers.
		World::Id objectId;
		World::Id componentId;
	};

	void RegisterLevelEditorEcsSystems(flecs::world& world, World::Level& level);
}