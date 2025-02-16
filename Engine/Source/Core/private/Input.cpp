#define WIN32_LEAN_AND_MEAN

#include <Input.h>

#include <ini.h>

#include <windows.h>
#include <WindowsX.h>
#include <wrl.h>

#include <WinUser.h>

#include "Keycodes.h"

namespace GameEngine::Core
{
	std::unordered_map<std::string, InputManager::KeyCode> InputManager::m_mappings = {};

	using NameMap = std::unordered_map<std::string, std::string>;

	static int iniHandler(void* user, const char* section, const char* name, const char* value)
	{
		NameMap* config = (NameMap*)user;

		std::string handle;

		if (*section != '\0')
		{
			handle = std::string(section) + "::" + std::string(name);
		}
		else
		{
			handle = std::string(name);
		}

		config->operator[](handle) = std::string(value);

		return 1;
	}

	bool GameEngine::Core::InputManager::importConfig(const char* path)
	{
		NameMap map;

		if (ini_parse(path, iniHandler, (void*) & map) < 0)
		{
			std::string message = std::string("Could not open input config file ") + path + ".";
			std::wstring wideMessage(message.begin(), message.end());
			MessageBox(0, wideMessage.c_str(), 0, 0);
			return true;
		}

		for (auto& [id, keyName] : map)
		{
			auto found = kKeyCodes.find(keyName);
			if (found == kKeyCodes.end())
			{
				std::string message = std::string("Unknown key name \"") + keyName + "\"";
				std::wstring wideMessage(message.begin(), message.end());
				MessageBox(0, wideMessage.c_str(), 0, 0);
			}
			else
			{
				m_mappings[id] = found->second;
			}
		}

		return false;
	}

	std::optional<KeyboardInput> GameEngine::Core::InputManager::getKeyboardInput(const char* id)
	{
		auto found = m_mappings.find(id);
		if (found == m_mappings.end())
		{
			return {};
		}
		return KeyboardInput(found->second);
	}

	KeyboardInput InputManager::getKeyboardInput(const char* id, const char* dflt)
	{
		auto existing = getKeyboardInput(id);
		if (existing) return *existing;

		auto found = kKeyCodes.find(dflt);
		if (found == kKeyCodes.end())
		{
			std::string message = std::string("Unknown default key name \"") + dflt + "\".";
			std::wstring wideMessage(message.begin(), message.end());
			MessageBox(0, wideMessage.c_str(), 0, 0);
			return KeyboardInput(VK_SPACE);
		}
		return KeyboardInput(found->second);
	}

	bool InputManager::getState(KeyCode id)
	{
		return GetKeyState(id) & 0x8000;
	}

	Math::Vector3f solveOmnidirectionalInput(
		const KeyboardInput& forward, const KeyboardInput& back,
		const KeyboardInput& left,    const KeyboardInput& right,
		const KeyboardInput& up,      const KeyboardInput& down)
	{
		Math::Vector3f direction(0.0f, 0.0f, 0.0f);

		// Here comes the lazy definition of the input matrix...
		if (forward.isDown()) direction.z -= 1.0;
		if (back.isDown())    direction.z += 1.0;

		if (left.isDown())    direction.x -= 1.0;
		if (right.isDown())   direction.x += 1.0;

		if (up.isDown())      direction.y += 1.0;
		if (down.isDown())    direction.y -= 1.0;

		return direction.Normalized();
	}
}
