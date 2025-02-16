#pragma once

#include <RenderEngine.h>
#include <RenderThread.h>
#include <Timer.h>
#include <Window/IWindow.h>
#include <Input.h>

namespace GameEngine
{
	class GameObject;

	class Game final
	{
	public:
		Game() = delete;
		Game(
			std::function<bool()> PlatformLoopFunc
		);

	public:
		void Run();
		void Update(float dt);

	private:
		// The main idea behind having this functor is to abstract the common code from the platfrom-specific code
		std::function<bool()> PlatformLoop = nullptr;

	private:
		Core::Timer m_GameTimer;
		std::unique_ptr<Render::RenderThread> m_renderThread;
		std::vector<GameObject*> m_Objects;

		// Is there a way to make these shorter without introducing new abstractions?
		// Somethign like InputManager::getOmnidirectionalInput(...) would "solve" the problem,
		// but I wonder if there is a more elegant way.
		GameEngine::Core::KeyboardInput m_inputForward =
			GameEngine::Core::InputManager::getKeyboardInput("camera::forward", "I");
		GameEngine::Core::KeyboardInput m_inputBack =
			GameEngine::Core::InputManager::getKeyboardInput("camera::back",    "K");
		GameEngine::Core::KeyboardInput m_inputLeft =
			GameEngine::Core::InputManager::getKeyboardInput("camera::left",    "J");
		GameEngine::Core::KeyboardInput m_inputRight =
			GameEngine::Core::InputManager::getKeyboardInput("camera::right",   "L");
		GameEngine::Core::KeyboardInput m_inputUp =
			GameEngine::Core::InputManager::getKeyboardInput("camera::up",      "O");
		GameEngine::Core::KeyboardInput m_inputDown =
			GameEngine::Core::InputManager::getKeyboardInput("camera::down",    "U");
	};
}