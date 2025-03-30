#include <LevelEditor/LevelEditor.h>

#include <map>
#include <memory>

#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	namespace Editor
	{
		// An internal component representation would have been better than
		// working directly with the serialization.
		static Math::Vector3f DescrToVector(const World::LevelObject::ComponentDesc& desc)
		{
			Math::Vector3f vector;
			std::stringstream ss(desc);
			std::string temp;

			// Parse the first float
			std::getline(ss, temp, ',');
			vector.x = std::stof(temp);

			// Parse the second float
			std::getline(ss, temp, ',');
			vector.y = std::stof(temp);

			// Parse the third float
			std::getline(ss, temp, ',');
			vector.z = std::stof(temp);

			return vector;
		}
		
		static World::LevelObject::ComponentDesc VectorToDescr(const Math::Vector3f& vector) {
			std::ostringstream oss;
			oss << vector.x << "," << vector.y << "," << vector.z;
			return oss.str();
		}


		LevelEditor::LevelEditor(flecs::world& world)
		{
			m_FlecsWorld = &world;

			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (auto& [objectId, levelObject] : m_Level->GetLevelObjects())
			{
				RegisterToFlecs(levelObject);
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world, *m_Level);
		}

		void LevelEditor::RegisterToFlecs(World::LevelObject& levelObject)
		{
			flecs::entity entity = m_FlecsWorld->entity(levelObject.GetName().c_str());

			World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

			World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
				[](auto& componentPair)
				{
					return !std::strcmp(componentPair.second.first.c_str(), "Position");
				}
			);

			World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
				[](auto& componentPair)
				{
					return !std::strcmp(componentPair.second.first.c_str(), "GeometryPtr");
				}
			);

			if (positionAttribute != componentList.end() &&
				geometryAttribute != componentList.end())
			{
				assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second.second));

				entity.set(EntitySystem::LevelEditorECS::PositionDesc{ levelObject.GetId(), positionAttribute->second.GetId()});

				// Can be set to 0 since it doesn't matter now, will be updated by the system
				entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
				entity.set(GeometryPtr{
					reinterpret_cast<RenderCore::Geometry*>(
						World::WorldParser::GetCustomComponents()[geometryAttribute->second.second]
						)
					});
			}
		}

		void LevelEditor::Draw()
		{
			std::map<std::string*, std::unique_ptr<Math::Vector3f>> vectorMappers{};

			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (auto& [objId, levelObject] : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (auto& [compId, component] : levelObject.GetComponents())
						{
							// Some sort of component type recognition?
							// We need a parser and a serializer for both editor and an engine.
							// Why not share them, and use the data in its favorable format?
							if (component.first == "Position")
							{
								// Pretty cool to make a bunch of allocation every tick, eh?
								std::unique_ptr<Math::Vector3f> correspVector =
									std::make_unique<Math::Vector3f>(DescrToVector(component.second));
								// Assuming Math::Vector3f contains three floats and nothing else...
								// 
								// Imagine trying to find a bug in an editor after you added a few flags to a structure.
								// This won't even produce segfaults, let alone compilation errors!
								ImGui::InputFloat3(component.first.c_str(), &correspVector->x);
								vectorMappers[&component.second] = std::move(correspVector);
							}
							else
							{
								ImGui::InputText(component.first.c_str(), &component.second);
							}
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (ImGui::Button("Create a cube"))
			{
				CreateCube();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			ImGui::End();

			// ImGui seems pretty cool...
			// I love the fact that it automatically filters out invelid inputs
			// and hides all the underlying update logics.
			// I wonder it it is possible to set up a custom value filter...
			// There is a `Callback` parameter that can help me answer the question,
			// though I am a bit too bussy right now to dive into that rabbit hole.
			for (auto& [descrPtr, vector] : vectorMappers)
			{
				*descrPtr = VectorToDescr(*vector);
			}
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::CreateCube()
		{
			assert(m_Level.has_value());

			static unsigned sCubeCount = 0;
			++sCubeCount;
			World::LevelObject cube;
			cube.SetName((std::stringstream{} << "Cube" << sCubeCount).str().c_str());
			cube.AddComponent("Position", "0.0,0.0,0.0");
			cube.AddComponent("GeometryPtr", "Cube");
			m_Level->AddLevelObject(cube);
			RegisterToFlecs(cube);
		}
	}
}