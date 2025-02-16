#pragma once

#include <Core/export.h>

#include <Vector.h>

#include <string>
#include <unordered_map>
#include <optional>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API KeyboardInput;

		class CORE_API InputManager
		{
			InputManager() = delete;

		public:
			friend class KeyboardInput;

			
			static bool importConfig(const char* path);

			static std::optional<KeyboardInput> getKeyboardInput(const char* id);
			static KeyboardInput getKeyboardInput(const char* id, const char* dflt);

		private:
			using KeyCode = int;

			static bool getState(KeyCode id);

			static std::unordered_map<std::string, KeyCode> m_mappings;
		};

		class CORE_API KeyboardInput
		{
			KeyboardInput(InputManager::KeyCode key) : m_key(key) {}

		public:
			friend class InputManager;

			bool isDown() const { return InputManager::getState(m_key); }

		private:
			InputManager::KeyCode m_key;
		};

		CORE_API Math::Vector3f solveOmnidirectionalInput(
			const KeyboardInput& forward, const KeyboardInput& back,
			const KeyboardInput& left,    const KeyboardInput& right,
			const KeyboardInput& up,      const KeyboardInput& down);
	}
}
