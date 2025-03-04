#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>
#include <ecsEntityManagement.h>

using namespace GameEngine;

void RegisterEcsMeshSystems(flecs::world& world)
{
	static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();

	world.system<const GeometryPtr, RenderObjectPtr>()
		.each([&](flecs::entity e, const GeometryPtr& geometry, RenderObjectPtr& renderObject)
	{
		renderThread->ptr->EnqueueCommand(Render::ERC::CreateRenderObject, geometry.ptr, renderObject.ptr);
		e.remove<GeometryPtr>();
	});

	world.system<RenderObjectPtr, const Position>()
		.each([&](RenderObjectPtr& renderObject, const Position& position)
	{
		if (!renderObject.ptr) return;
		renderObject.ptr->SetPosition(position.value, renderThread->ptr->GetMainFrame());
	});

	world.system<RenderObjectPtr, const MarkedForDestruction>()
		.each([&](flecs::entity e, RenderObjectPtr& renderObject, const MarkedForDestruction& marker)
	{
		if (renderObject.ptr == nullptr)
			return;

		renderThread->ptr->EnqueueCommand(Render::ERC::RemoveRenderObject, GameEngine::RenderCore::Geometry::Ptr(nullptr), renderObject.ptr);
		renderObject.ptr = nullptr;
	});
}


