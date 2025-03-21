// This define is essential for the scripts to expose the ECS components to lua syntax
// There is a task to rework this behavior
#define GAME_FRAMEWORK

#include <Camera.h>
#include <DefaultGeometry.h>
#include <ECS/ecsSystems.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ecsEntityManagement.h>
#include <ecsShooting.h>
#include <flecs.h>

using namespace GameEngine;

void SpawnBullet(
    flecs::world& world,
    const GameEngine::Math::Vector3f& position,
    const GameEngine::Math::Vector3f& direction,
    flecs::entity& owner,
    bool despawnAfterTime = true) {
    GameEngine::Math::Vector3f velocity = direction * 40.0f;
    world.entity()
        .set(Position{ position.x, position.y, position.z })
        .set(Velocity{ velocity.x, velocity.y, velocity.z })
        .set(Gravity{ 0.f, -9.8065f, 0.f })
        .set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
        .set(Bounciness{ 1.f })
        .set(FrictionAmount{ despawnAfterTime ? 0.9f : 0.0f })
        .set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
        .set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })

        .set(Owner{ owner })
        .set(ContactDamage{ 5.0f })
        .set(SphereCollider{ 1.0f })
        .set(TimedDespawn{ 5.0f, despawnAfterTime, 0.0f })
        .set(CanBeDestroyed{ false, false });
}

void SpawnCubeProjectile(
    flecs::world& world,
    const GameEngine::Math::Vector3f& position,
    const GameEngine::Math::Vector3f& direction,
    flecs::entity& owner) {
    SpawnBullet(world, position, direction, owner);
}

void GameFramework::Init()
{
    RegisterComponents(m_World);
    RegisterSystems();
    CreateEntities();
}

void GameFramework::CreateEntities() {
    flecs::entity cubeControl = m_World.entity()
        .set(Position{ -2.f, 0.f, 0.f })
        .set(Velocity{ 0.f, 0.f, 0.f })
        .set(Speed{ 10.f })
        .set(FrictionAmount{ 0.9f })
        .set(JumpSpeed{ 10.f })
        .set(Gravity{ 0.f, -9.8065f, 0.f })
        .set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
        .set(Bounciness{ 0.3f })
        .set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
        .set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
        .set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

    flecs::entity camera = m_World.entity()
        .set(Position{ 0.0f, 12.0f, -10.0f })
        .set(Speed{ 10.f })
        .set(CameraPtr{ Core::g_MainCamera })
        .set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) })
        .set(PrimaryProjectile{ SpawnCubeProjectile })
        .set(Magazine{ .capacity = 3, .count = 3 })
        .set(ReloadDuration{ 3.0f });

    SpawnBullet(
        m_World,
        GameEngine::Math::Vector3f(2.0f, 3.0f, 0.0f),
        GameEngine::Math::Vector3f(0.0f, 0.1f, 0.0f),
        camera,
        false
    );
}

void GameFramework::RegisterComponents(flecs::world& world)
{
    flecs::world actualWorld = world.get_world();

    ECS_META_COMPONENT(actualWorld, Position);
    ECS_META_COMPONENT(actualWorld, Velocity);
    ECS_META_COMPONENT(actualWorld, Gravity);
    ECS_META_COMPONENT(actualWorld, BouncePlane);
    ECS_META_COMPONENT(actualWorld, Bounciness);
    ECS_META_COMPONENT(actualWorld, ShiverAmount);
    ECS_META_COMPONENT(actualWorld, FrictionAmount);
    ECS_META_COMPONENT(actualWorld, Speed);

    ECS_META_COMPONENT(actualWorld, TimedDespawn);
    ECS_META_COMPONENT(actualWorld, CanBeDestroyed);
    ECS_META_COMPONENT(actualWorld, SphereCollider);
}

void GameFramework::RegisterSystems()
{
    RegisterEcsMeshSystems(m_World);
    RegisterEcsControlSystems(m_World);
    RegisterEcsCombatSystems(m_World);
    RegisterEcsEntityMgmtSystems(m_World);
}

void GameFramework::Update(float dt)
{

}