#pragma once

#include <flecs.h>
#include <functional>
#include <Vector.h>

using ProjectileSpawner = std::function<void(
	flecs::world& world,
	const GameEngine::Math::Vector3f& position,
	const GameEngine::Math::Vector3f& direction,
	flecs::entity& owner
)>;

struct PrimaryProjectile
{
	ProjectileSpawner spawner;

	bool m_triedToShootLastFrame = false;
};

struct Magazine
{
	unsigned capacity;
	unsigned count;
};

struct ReloadDuration
{
	float duration;

	// Another con of ECS: everything has to be public
	// (or have public modifying methods, there is no way
	// to limit something's scope to just the update function).
	// Is there a way to keep certain things private without
	// reintroducing the `tick(dt)` method to components?
	double m_timeSinceStart = 0.0f;
};

struct ContactDamage
{
	float damage;
};

struct CanDie
{
private:
	bool _ = false;
};

struct Owner
{
	flecs::entity entity;
};

#undef ECS_META_IMPL
#ifndef GAME_FRAMEWORK
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

ECS_STRUCT(SphereCollider,
{
	float radius;
});

ECS_STRUCT(TimedDespawn,
{
	float duration;
	bool ticking;
	float m_timeSinceStart;
});

struct AmoRefill
{
	int amount;
};

void RegisterEcsCombatSystems(flecs::world& world);
