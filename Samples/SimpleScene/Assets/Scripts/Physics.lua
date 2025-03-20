local ecs = require "ecs"

local function rand_flt(from, to)
	local maxNumber = 32767
	return from + (math.random(maxNumber) / maxNumber) * (to - from)
end

local function move(it)
	for pos, vel, ent in ecs.each(it) do
		pos.x = pos.x + vel.x * it.delta_time
		pos.y = pos.y + vel.y * it.delta_time
		pos.z = pos.z + vel.z * it.delta_time
	end
end

local function gravity(it)
	for pos, vel, grav, plane, ent in ecs.each(it) do
		local planeEpsilon = 0.1

		if plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon then
			do return end
		end

		vel.x = vel.x + grav.x * it.delta_time
		vel.y = vel.y + grav.y * it.delta_time
		vel.z = vel.z + grav.z * it.delta_time
	end
end

local function FrictionSystem(it)
	for vel, friction, ent in ecs.each(it) do
		vel.x = vel.x - vel.x * friction.value * it.delta_time
		vel.y = vel.y - vel.y * friction.value * it.delta_time
		vel.z = vel.z - vel.z * friction.value * it.delta_time
	end
end

local function ShiverSystem(it)
	for pos, shiver, ent in ecs.each(it) do
		pos.x = pos.x + rand_flt(-shiver.value, shiver.value)
		pos.y = pos.y + rand_flt(-shiver.value, shiver.value)
		pos.z = pos.z + rand_flt(-shiver.value, shiver.value)
	end
end

local function BounceSystem(it)
	for pos, vel, plane, bounciness, ent in ecs.each(it) do
		local dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z
		local dotVel = plane.x * vel.x + plane.y * vel.y + plane.z * vel.z

		if dotPos < plane.w then
			pos.x = pos.x - (dotPos - plane.w) * plane.x
			pos.y = pos.y - (dotPos - plane.w) * plane.y
			pos.z = pos.z - (dotPos - plane.w) * plane.z

			vel.x = vel.x - (1.0 + bounciness.value) * plane.x * dotVel
			vel.y = vel.y - (1.0 + bounciness.value) * plane.y * dotVel
			vel.z = vel.z - (1.0 + bounciness.value) * plane.z * dotVel
		end
	end
end

local function DestroyOnTimerSystem(it)
	for despawn, marker, ent in ecs.each(it) do
		if despawn.ticking then
			despawn.m_timeSinceStart = despawn.m_timeSinceStart + ecs.world_info().delta_time;
			if despawn.m_timeSinceStart >= despawn.duration then
				marker.beingDestroyed = true
			end
		end
	end
end

local function DestroyIntersectingSystem(it)
	for alphaPos, alphaColl, alphaMarker, alphaEnt in ecs.each(it) do
		for betaPos, betaColl, betaMarker, betaEnt in ecs.each(it) do
			if ecs.get_alive(alphaEnt) > ecs.get_alive(betaEnt) then
				local deltaX = alphaPos.x - betaPos.x
				local deltaY = alphaPos.y - betaPos.y
				local deltaZ = alphaPos.z - betaPos.z

				local distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ
				local radiusSum = alphaColl.radius + betaColl.radius

				if distance < radiusSum * radiusSum then
					alphaMarker.beingDestroyed = true
					betaMarker.beingDestroyed = true
				end
			end
		end
	end
end

local function DestroyIntersectingSystem_dbg(it)
	for alphaPos, alphaColl, alphaMarker, alphaEnt in ecs.each(it) do
		if ecs.is_alive(alphaEnt) then
			alphaPos.x = alphaPos.x + 0.01
		end

		local count = 0

		for betaPos, betaColl, betaMarker, betaEnt in ecs.each(it) do
			-- if not ecs.get_alive(alphaEnt) == ecs.get_alive(betaEnt) then
			-- 	alphaPos.z = alphaPos.z + 0.01
			-- 	betaPos.z = betaPos.z + 0.01
			-- end

			count = count + 1
			-- alphaPos.x = alphaPos.x + (alphaEnt - betaEnt)
			-- if ecs.get_alive(alphaEnt) > ecs.get_alive(betaEnt) and false then
			-- 	local deltaX = alphaPos.x - betaPos.x
			-- 	local deltaY = alphaPos.y - betaPos.y
			-- 	local deltaZ = alphaPos.z - betaPos.z

			-- 	local distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ
			-- 	local radiusSum = alphaColl.radius + betaColl.radius

			-- 	if distance < radiusSum * radiusSum and false then
			-- 		alphaMarker.beingDestroyed = true
			-- 		betaMarker.beingDestroyed = true
			-- 	end

			-- 	alphaPos.x = alphaPos.x + 0.01
			-- 	betaPos.x = betaPos.x + 0.01
			-- end
		end

		alphaPos.z = alphaPos.z + (count - 1) * 0.01
	end
end

ecs.system(move, "Move", ecs.OnUpdate, "Position, Velocity")
ecs.system(gravity, "grav", ecs.OnUpdate, "Position, Velocity, Gravity, BouncePlane")
ecs.system(FrictionSystem, "FrictionSystem", ecs.OnUpdate, "Velocity, FrictionAmount")
ecs.system(ShiverSystem, "ShiverSystem", ecs.OnUpdate, "Position, ShiverAmount")
ecs.system(BounceSystem, "BounceSystem", ecs.OnUpdate, "Position, Velocity, BouncePlane, Bounciness")
ecs.system(DestroyIntersectingSystem, "DestroyIntersectingSystem", ecs.OnUpdate,
	"Position, SphereCollider, CanBeDestroyed")
ecs.system(DestroyOnTimerSystem, "DestroyOnTimerSystem", ecs.OnUpdate, "TimedDespawn, CanBeDestroyed")
