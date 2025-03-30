#include <Level.h>

namespace GameEngine::World
{
	Level::Level(std::string& name)
		: m_Name(std::move(name))
	{
	}

	void Level::AddLevelObject(const LevelObject& levelObject)
	{
		m_LevelObjectList.insert({ levelObject.GetId(), levelObject });
	}

	LevelObject* Level::GetLevelObject(Id id)
	{
		auto found = m_LevelObjectList.find(id);
		if (found != m_LevelObjectList.end()) [[likely]]
		{
			return &found->second;
		}

		return nullptr;
	}
}