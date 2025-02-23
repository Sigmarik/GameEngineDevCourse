#pragma once

#include <utility>

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <Timer.h>

namespace GameEngine
{
	// If UActorComponent class and Unity component system had a bastard child,
	// this class would have been it.
	class ObjectModule
	{
	public:
		virtual void Update(float dt, size_t frame) {};

		// TODO: Intermodular communication?
		// TODO: Enable/disable logics?

		friend class GameObject;

	protected:
		// We don't want empty object modules to be constructible from the outside,
		// yet we don't want them to be fully virtual.
		ObjectModule() = default;

		ObjectModule(const ObjectModule&) = delete;
		ObjectModule(ObjectModule&&) = delete;
		ObjectModule& operator=(const ObjectModule&) = delete;
		ObjectModule& operator=(ObjectModule&&) = delete;

		GameObject& GetOwner() { return *m_Owner; }

	private:
		GameObject* m_Owner = nullptr;
	};

	// Not sure why generic GameObject class is not a part of the CORE_API, but what do I know...
	class GameObject final
	{
	public:
		GameObject() = default;

		// TODO: Remove copy/move constructors?
		// We don't need them anyways and they break module linkage.

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		void UpdateModules(float dt, size_t frame)
		{
			static bool __recursion_protector = false;
			//! Do not call module updates of an object from within a module
			assert(!__recursion_protector);
			__recursion_protector = true;

			for (ObjectModule* module : m_Modules)
			{
				module->Update(dt, frame);
			}

			__recursion_protector = false;
		}

		template <class ModuleT, class ...Args>
		void AddModule(Args&& ...args) {
			m_Modules.push_back(new ModuleT(std::forward<Args>(args)...));
			m_Modules.back()->m_Owner = this;
		}

		// TODO: Module accessors?
		// GameObject::GetModule<MyModule>() ?
		// GameObject::ForEachModule<MyModule>(functor) ?
		// Which one would be better?

	protected:
		// TODO: Make visual representation of an object a module?
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();

		// TODO: Who tf cares about freeing allocated resources, really?
		// The engine can go up in flames once I am done.
		std::vector<ObjectModule*> m_Modules{};
	};

	namespace Modules
	{
		// Not exactly the PlayerController one would expect, as it
		// executes the movement itself, instead of providing an interface
		// for other components to process movement input.
		class PlayerController : public ObjectModule
		{
		public:
			PlayerController();

			virtual void Update(float dt, size_t frame) override;
		private:
			int m_Velocity = 0;
		};

		class BouncyMovement : public ObjectModule
		{
		public:
			virtual void Update(float dt, size_t frame) override;

		private:
			float m_Velocity = 0;
		};

		class FloatyMovement : public ObjectModule
		{
		public:
			virtual void Update(float dt, size_t frame) override;

		private:
			Core::Timer m_Timer{};
		};
	}
}