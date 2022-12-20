#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		
		SetDarkTheme();

		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 1.0f, 1.0f };
		pinkSphere.Roughness = 0.1f;

		Material& ground = m_Scene.Materials.emplace_back();
		ground.Albedo = { 0.2f, 0.8f, 0.2f };
		ground.Roughness = 0.5f;

		Material& Bulb = m_Scene.Materials.emplace_back();
		Bulb.Emission = glm::vec3(0.0f, 1.0f, 0.0f);

		// SPHERES
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
		
		
		// LIGHTS
		{
			Light light;
			light.Direction = glm::vec3{ -1.0f };
			m_Scene.Lights.push_back(light);
		}

		
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			m_Realtime = false;
			Render();
		}
		
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}


		

		ImGui::Checkbox("Realtime", &m_Realtime);
		ImGui::DragInt("Bounces", &m_Renderer.GetSettings().Bounces);
		

		ImGui::End();

		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material Index", &sphere.MaterialIndex, 1.0f, 0, m_Scene.Materials.size()-1);
			
			
			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& mat = m_Scene.Materials[i];
		
			ImGui::Text("Lit Material");
			ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.Albedo));
			ImGui::DragFloat("Rougness", &mat.Roughness, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &mat.Metallic, 0.1f, 0.0f, 3.0f);
			ImGui::DragFloat("Specular", &mat.Specular, 0.1f, 0.0f, 100);

			ImGui::ColorEdit3("Emmision", glm::value_ptr(mat.Emission));


			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::Separator();
		for (size_t i = 0; i < m_Scene.Lights.size(); i++)
		{
			ImGui::PushID(i);

			Light& light = m_Scene.Lights[i];

			ImGui::ColorEdit3("Light Color", glm::value_ptr(light.Color));
			ImGui::DragFloat3("Light Direction", glm::value_ptr(light.Direction));


			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		bool open = true;
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		if (m_Realtime) {
			Render();
		}

		
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		if (!m_Realtime) {
			m_Renderer.ResetFrameIndex();
			for (int i = 0; i <= m_Renderer.GetSettings().Samples; i++) {
				m_Renderer.Render(m_Scene, m_Camera);
			}
		}
		else {
			m_Renderer.Render(m_Scene, m_Camera);
		}

		m_LastRenderTime = timer.ElapsedMillis();
		std::cout << "Render Time: " << m_LastRenderTime << "ms" << std::endl;

	}

	static void SetDarkTheme() {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	}

	static void SetLightTheme() {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 1.0f-0.1f, 1.0f-0.105f, 1.0f - 0.11f, 1.0f };
		colors[ImGuiCol_Text] = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };


		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 1.0f - 0.2f, 1.0f - 0.205f, 1.0f - 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 1.0f - 0.3f,1.0f - 0.305f,1.0f - 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 1.0f - 0.15f,1.0f - 0.1505f,1.0f - 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 1.0f - 0.2f,1.0f - 0.205f,1.0f - 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 1.0f - 0.3f, 1.0f - 0.305f, 1.0f - 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f,1.0f - 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 1.0f - 0.2f, 1.0f - 0.205f, 1.0f - 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 1.0f - 0.3f, 1.0f - 0.305f, 1.0f - 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f, 1.0f - 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f, 1.0f - 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 1.0f - 0.38f, 1.0f - 0.3805f, 1.0f - 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 1.0f - 0.28f, 1.0f - 0.2805f, 1.0f - 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f, 1.0f - 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 1.0f - 0.2f, 1.0f - 0.205f, 1.0f - 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f, 1.0f - 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		colors[ImGuiCol_MenuBarBg] = ImVec4{ 1.0f - 0.15f, 1.0f - 0.1505f, 1.0f - 0.151f, 1.0f };
	}
private:

	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	bool m_Realtime = false;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);


	app->PushLayer<ExampleLayer>();
		app->SetMenubarCallback([app]()
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit"))
					{
						app->Close();
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Preferences")) {
					if (ImGui::MenuItem("Dark Theme"))
					{
						ExampleLayer::SetDarkTheme();
					}
					if (ImGui::MenuItem("Light Theme"))
					{
						ExampleLayer::SetLightTheme();
					}
					ImGui::EndMenu();
				}
			});
	return app;
};