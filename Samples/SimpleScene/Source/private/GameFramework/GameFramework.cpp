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
#include <flecs.h>

using namespace GameEngine;

void SpawnCubeProjectile(
    flecs::world& world,
    const GameEngine::Math::Vector3f& position,
    const GameEngine::Math::Vector3f& direction,
    flecs::entity& owner) {
    world.entity()
        .set(Position{position})
        .set(Velocity{direction * 40.0f})
        .set(Gravity{Math::Vector3f(0.f, -9.8065f, 0.f)})
        .set(BouncePlane{Math::Vector4f(0.f, 1.f, 0.f, 5.f)})
        .set(Bounciness{1.f})
        .set(FrictionAmount{0.9f})
        .set(GeometryPtr{RenderCore::DefaultGeometry::Cube()})
        .set(RenderObjectPtr{new Render::RenderObject()})

        .set(Owner{owner})
        .set(ContactDamage{5.0f})
        .set(SphereCollider{0.5f})
        .set(TimedDespawn{5.0f});
}

void GameFramework::Init() {
    RegisterComponents();
    RegisterSystems();

    flecs::entity cubeControl = m_World.entity()
                                    .set(Position{-2.f, 0.f, 0.f})
                                    .set(Velocity{0.f, 0.f, 0.f})
                                    .set(Speed{10.f})
                                    .set(FrictionAmount{0.9f})
                                    .set(JumpSpeed{10.f})
                                    .set(Gravity{0.f, -9.8065f, 0.f})
                                    .set(BouncePlane{0.f, 1.f, 0.f, 5.f})
                                    .set(Bounciness{0.3f})
                                    .set(EntitySystem::ECS::GeometryPtr{RenderCore::DefaultGeometry::Cube()})
                                    .set(EntitySystem::ECS::RenderObjectPtr{new Render::RenderObject()})
                                    .set(ControllerPtr{new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini"))});

    flecs::entity cubeMoving = m_World.entity()
                                   .set(Position{2.f, 0.f, 0.f})
                                   .set(Velocity{0.f, 3.f, 0.f})
                                   .set(Gravity{0.f, -9.8065f, 0.f})
                                   .set(BouncePlane{0.f, 1.f, 0.f, 5.f})
                                   .set(Bounciness{1.f})
                                   .set(EntitySystem::ECS::GeometryPtr{RenderCore::DefaultGeometry::Cube()})
                                   .set(EntitySystem::ECS::RenderObjectPtr{new Render::RenderObject()})
                                   .set(SphereCollider{1.0f})
                                   .set(AmoRefill{3})
                                   .set(CanDie{});

    flecs::entity camera = m_World.entity()
                               .set(Position{0.0f, 12.0f, -10.0f})
                               .set(Speed{10.f})
                               .set(CameraPtr{Core::g_MainCamera})
                               .set(ControllerPtr{new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini"))})
                               .set(PrimaryProjectile{SpawnCubeProjectile})
                               .set(Magazine{.capacity = 3, .count = 3})
                               .set(ReloadDuration{3.0f});
}

void GameFramework::RegisterComponents() {
    // Exposing these components for the lua system
    ECS_META_COMPONENT(m_World, Position);
    ECS_META_COMPONENT(m_World, Velocity);
    ECS_META_COMPONENT(m_World, Gravity);
    ECS_META_COMPONENT(m_World, BouncePlane);
    ECS_META_COMPONENT(m_World, Bounciness);
    ECS_META_COMPONENT(m_World, ShiverAmount);
    ECS_META_COMPONENT(m_World, FrictionAmount);
    ECS_META_COMPONENT(m_World, Speed);
}

void GameFramework::RegisterSystems() {
    RegisterEcsMeshSystems(m_World);
    RegisterEcsControlSystems(m_World);
}

void GameFramework::RegisterComponents() {
    // Exposing these components for the lua system
    ECS_META_COMPONENT(m_World, Position);
    ECS_META_COMPONENT(m_World, Velocity);
    ECS_META_COMPONENT(m_World, Gravity);
    ECS_META_COMPONENT(m_World, BouncePlane);
    ECS_META_COMPONENT(m_World, Bounciness);
    ECS_META_COMPONENT(m_World, ShiverAmount);
    ECS_META_COMPONENT(m_World, FrictionAmount);
    ECS_META_COMPONENT(m_World, Speed);
}

void GameFramework::RegisterSystems() {
    RegisterEcsMeshSystems(m_World);
    RegisterEcsControlSystems(m_World);
}

void GameFramework::Update(float dt) {
}