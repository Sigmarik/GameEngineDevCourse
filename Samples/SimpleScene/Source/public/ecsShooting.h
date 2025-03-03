#pragma once

#include <flecs.h>
#include <functional>
#include <Vector.h>

// Is there a better way of implementing blueprints/spawn templates?

// I'd really love to have an interface simmilar to
/*
```C++
flecs::entity batEnemy = m_World.entity()
	.set(PrimaryProjectile{FireballTemplate})
	.set(SecondaryProjectile{BloodSplashTemplate});
```
*/

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

struct CanDie {
private:
	bool _ = false;
};

struct SphereCollider
{
	float radius;
};

struct TimedDespawn
{
	float duration;

	bool ticking = false;

	// Another con of ECS: everything has to be public.
	// Is there a way to keep certain things private without
	// reintroducing `tick(dt)` method to components?
	double m_timeSinceStart = 0.0f;
};

struct AmoRefill
{
	unsigned amount;
};

struct Owner
{
	flecs::entity entity;
};

void RegisterEcsCombatSystems(flecs::world& world);
