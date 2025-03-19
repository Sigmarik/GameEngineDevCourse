#include <ecsShooting.h>

#include <ecsPhys.h>
#include <ecsMesh.h>
#include <ecsControl.h>
#include <ecsEntityManagement.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Camera.h>
#include <RenderObject.h>
#include <RenderThread.h>
#include <ECS/ecsSystems.h>

#include <iostream>

void RegisterEcsCombatSystems(flecs::world& world)
{
	// Would have been better to introduce an entirely new component
	// for forward direction storage, but who am I to make smart decisions...
	world.system<
		PrimaryProjectile,
		ControllerPtr,
		const Position,
		CameraPtr
	>().each([&](flecs::entity e, 
		PrimaryProjectile& projectile,
		ControllerPtr& controller,
		const Position& position,
		CameraPtr& camera)
	{
		Magazine* magazine = e.get_mut<Magazine>();
		bool hasAmmo = !magazine || magazine->count > 0;

		bool input = controller.ptr->IsPressed("Shoot");
		bool hasToShoot = input && !projectile.m_triedToShootLastFrame;
		if (input)
		{
			projectile.m_triedToShootLastFrame = true;
		}
		else
		{
			projectile.m_triedToShootLastFrame = false;
		}

		if (hasToShoot && hasAmmo)
		{
			if (magazine)
			{
				--magazine->count;
			}

			GameEngine::Math::Vector3f pos{ position.x, position.y, position.z };
			projectile.spawner(world, pos, camera.ptr->GetViewDir(), e);
		}

		if (magazine && magazine->count == 0)
		{
			// If only we had timed events of some sort...

			ReloadDuration* reload = e.get_mut<ReloadDuration>();
			if (reload && reload->m_timeSinceStart > reload->duration)
			{
				reload->m_timeSinceStart = 0.0f;
			}
		}
	});

	// Oh boy does Java not have boilerplate code compared to this...
	world.system<
		const Position,
		const SphereCollider,
		const CanDie
	>().each([&](flecs::entity entityAlpha,
		const Position& alphaPos,
		const SphereCollider& alphaCollider,
		const CanDie&)
	{
		if (entityAlpha.has<MarkedForDestruction>()) return;

		world.each([&](flecs::entity entityBeta,
			const Position& betaPos,
			const SphereCollider& betaCollider,
			const ContactDamage&)
		{
			if (entityAlpha == entityBeta) return;
			if (!entityBeta.is_alive()) return;
			if (entityBeta.has<MarkedForDestruction>()) return;

			GameEngine::Math::Vector3f posA{ alphaPos.x, alphaPos.y, alphaPos.z };
			GameEngine::Math::Vector3f posB{ betaPos.x,  betaPos.y,  betaPos.z };

			float distance = (posA - posB).GetLength();
			if (distance > alphaCollider.radius + betaCollider.radius) return;

			if (entityAlpha.has<AmoRefill>() && entityBeta.has<Owner>())
			{
				entityBeta.get<Owner>()->entity;
				flecs::entity shooter = entityBeta.get<Owner>()->entity;
				if (shooter.is_alive() && shooter.has<Magazine>())
				{
					shooter.get_mut<Magazine>()->count += entityAlpha.get<AmoRefill>()->amount;
				}
			}

			ConsciousDestroy(entityAlpha);
			ConsciousDestroy(entityBeta);
		});
	});

	static const GameEngine::EntitySystem::ECS::RenderThreadPtr* renderThread =
		world.get<GameEngine::EntitySystem::ECS::RenderThreadPtr>();

	world.system<
		TimedDespawn
	>().each([&](flecs::entity e, TimedDespawn& despawn)
	{
		if (!despawn.ticking) return;

		despawn.m_timeSinceStart += world.delta_time();

		if (despawn.m_timeSinceStart > despawn.duration)
		{
			ConsciousDestroy(e);
		}
	});

	world.system<
		ReloadDuration,
		Magazine
	>().each([&](ReloadDuration& reload, Magazine& mag)
	{
		reload.m_timeSinceStart += world.delta_time();
		if (reload.m_timeSinceStart >= reload.duration && mag.count == 0)
		{
			mag.count = mag.capacity;
		}
	});
}
