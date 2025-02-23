#include <GameObject.h>

#include <Input/InputHandler.h>

namespace GameEngine
{
	namespace Modules
	{
		PlayerController::PlayerController()
		{
			//! TODO: Will break upon module destruction,
			//! FIX ASAP!

			// TODO: Plus there is currently no way to know if
			// the input has stopped. What is the point of the
			// event-based input system, then?
			Core::g_InputHandler->RegisterCallback("GoUp", 
				[this]() { m_Velocity = 1; });
			Core::g_InputHandler->RegisterCallback("GoDown",
				[this]() { m_Velocity = -1; });
		}

		void PlayerController::Update(float dt, size_t frame)
		{
			GameObject& owner = GetOwner();
			Math::Vector3f pos = owner.GetPosition();

			pos.y += m_Velocity * dt * 100.0f;
			m_Velocity = 0;

			owner.SetPosition(pos, frame);
		}

		void BouncyMovement::Update(float dt, size_t frame)
		{
			GameObject& owner = GetOwner();
			Math::Vector3f pos = owner.GetPosition();

			m_Velocity -= dt * 9.8;
			pos.y += m_Velocity * dt;

			if (pos.y <= 0.0)
			{
				m_Velocity = abs(m_Velocity);
				pos.y = abs(pos.y);
			}

			owner.SetPosition(pos, frame);
		}

		void FloatyMovement::Update(float dt, size_t frame)
		{
			GameObject& owner = GetOwner();
			Math::Vector3f pos = owner.GetPosition();

			// Derivative of sin(t)
			// TODO: Not the best way to implement wavy motion,
			// but this will do for now. Fix if objects start
			// floating away over time.
			float shift = cos(m_Timer.GetTotalTime());
			pos.y += shift * dt * 10.0f;

			m_Timer.Tick();

			owner.SetPosition(pos, frame);
		}

	}
}

