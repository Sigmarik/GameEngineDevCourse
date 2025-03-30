#pragma once

#include <map>

#include <World/export.h>

namespace GameEngine::World
{
	using Id = unsigned long long;

	class WORLD_API LevelObject final
	{
	public:
		using ComponentName = std::string;
		using ComponentDesc = std::string;
		struct Component
		{
			Component(const ComponentName& name = "UnnamedComponent", const ComponentDesc& desc = "")
				: first(name), second(desc)
			{
				static Id s_Id = 0;
				m_id = ++s_Id;
			};

			ComponentName first;
			ComponentDesc second;

			Id GetId() const { return m_id; }

		private:
			Id m_id = 0;
		};

		// I really wanted not to touch engine's code, as I would have if I
		// were developing an editor, but, unfotunately, there currently is
		// no way of searching the object for a component, and a map
		// structure makes the process a lot more convenient.
		using ComponentList = std::map<Id, Component>;

	public:
		LevelObject() { static Id s_Id = 0; m_id = ++s_Id; }
		~LevelObject() = default;

	public:
		void SetName(const char* name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		Id AddComponent(const ComponentName& name, const ComponentDesc& desc);
		Component* GetComponent(Id id);
		const ComponentList& GetComponents() const { return m_ComponentList; }
		ComponentList& GetComponents() { return m_ComponentList; }
		Id GetId() const { return m_id; }

	private:
		std::string m_Name = "None";
		ComponentList m_ComponentList;

		Id m_id = 0;
	};
}