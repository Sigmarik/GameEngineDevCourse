#include <LevelObject.h>

namespace GameEngine::World
{
	Id LevelObject::AddComponent(const ComponentName& name, const ComponentDesc& desc)
	{
		Component component{name, desc};
		m_ComponentList.insert({ component.GetId(), component});
		return component.GetId();
	}

	LevelObject::Component* LevelObject::GetComponent(Id id)
	{
		auto found = m_ComponentList.find(id);
		if (found != m_ComponentList.end()) [[likely]]
		{
			return &found->second;
		}

		return nullptr;
	}
}