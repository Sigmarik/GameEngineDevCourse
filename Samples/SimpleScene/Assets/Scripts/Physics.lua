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

		-- `do return end` is NOT the same as `continue`
		if plane.x * pos.x + plane.y * pos.y + plane.z * pos.z > plane.w + planeEpsilon then
			vel.x = vel.x + grav.x * it.delta_time
			vel.y = vel.y + grav.y * it.delta_time
			vel.z = vel.z + grav.z * it.delta_time
		end
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

local function TickTimer(it)
	for timer, ent in ecs.each(it) do
		timer.remainder = timer.remainder - ecs.world_info().delta_time
	end
end

local function HideOnTimer(it)
	for timer, visibility, ent in ecs.each(it) do
		if timer.remainder < 0.0 then
			visibility.visible = false
		end
	end
end

local function CollideColliders(it)
	-- This thing still refuses to process interactions between objects with
	-- differing components.
	-- The only "reasonable" solution I see is creating a dedicated entity for
	-- collision processing and attaching it as a child to all the other
	-- components.
	-- BUT IT WOULD STILL REQUIRE INTERACTIONS BETWEEN DISTINCT OBJECTS!!!

	-- OR collisions can be processed by a completely separate system
	-- (e.g. Havok or PhysX) and its events can be "transmitted" by a set of
	-- parameters in related components.
	-- This still does not answer the question of "how to write
	-- inter-entity interactions that are purely gameplay-specific", though...

	-- I even though I have found the culprit once. 
	for collider_1, position_1, velocity_1, ent_1 in ecs.each(it) do
		for collider_2, position_2, velocity_2, ent_2 in ecs.each(it) do
			-- This is the shittiest physics simulation I've ever created...
			-- I half expected a big "You Have Angered The Gods" message to
			-- appear while writing this.
			if ent_1 < ent_2 then
				local diff_x = position_1.x - position_2.x
				local diff_y = position_1.y - position_2.y
				local diff_z = position_1.z - position_2.z

				local distance = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z
				local radius = collider_1.radius + collider_2.radius

				if distance < radius * radius then
					local dot_1 = velocity_1.x * diff_x + velocity_1.y * diff_y + velocity_1.z * diff_z
					local dot_2 = velocity_2.x * diff_x + velocity_2.y * diff_y + velocity_2.z * diff_z

					position_1.x = position_1.x + diff_x;
					position_1.y = position_1.y + diff_y;
					position_1.z = position_1.z + diff_z;

					position_2.x = position_2.x - diff_x;
					position_2.y = position_2.y - diff_y;
					position_2.z = position_2.z - diff_z;

					velocity_1.x = diff_x;
					velocity_1.y = diff_y;
					velocity_1.z = diff_z;
					velocity_2.x = -diff_x;
					velocity_2.y = -diff_y;
					velocity_2.z = -diff_z;
				end
			end
		end
	end
end

ecs.system(move, "Move", ecs.OnUpdate, "Position, Velocity")
ecs.system(gravity, "grav", ecs.OnUpdate, "Position, Velocity, Gravity, BouncePlane")
ecs.system(FrictionSystem, "FrictionSystem", ecs.OnUpdate, "Velocity, FrictionAmount")
ecs.system(ShiverSystem, "ShiverSystem", ecs.OnUpdate, "Position, ShiverAmount")
ecs.system(BounceSystem, "BounceSystem", ecs.OnUpdate, "Position, Velocity, BouncePlane, Bounciness")
ecs.system(TickTimer, "TickTimer", ecs.OnUpdate, "DestructionTimer")
ecs.system(HideOnTimer, "HideOnTimer", ecs.OnUpdate, "DestructionTimer, Visibility")
ecs.system(CollideColliders, "CollideColliders", ecs.OnUpdate, "Collider, Position, Velocity")
