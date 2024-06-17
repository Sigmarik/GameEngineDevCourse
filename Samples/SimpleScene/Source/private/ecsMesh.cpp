#include <ECS/ecsSystems.h>
#include <Geometry.h>
#include <RenderObject.h>
#include <RenderThread.h>
#include <ecsEntityManagement.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <flecs.h>

using namespace GameEngine;

void RegisterEcsMeshSystems(flecs::world& world) {
    static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();

    world.system<EntitySystem::ECS::RenderObjectPtr, const Position>()
        .each([&](EntitySystem::ECS::RenderObjectPtr& renderObject, const Position& position) {
            renderObject.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z), renderThread->ptr->GetMainFrame());
        });

    world.system<RenderObjectPtr, const MarkedForDestruction>()
        .each([&](flecs::entity e, RenderObjectPtr& renderObject, const MarkedForDestruction& marker) {
            if (renderObject.ptr == nullptr)
                return;

            renderThread->ptr->EnqueueCommand(Render::ERC::RemoveRenderObject, GameEngine::RenderCore::Geometry::Ptr(nullptr), renderObject.ptr);
            renderObject.ptr = nullptr;
        });
}
