#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/EntryPoint.h"
#include "Rainy/Core/Time.h"
#include "Rainy/Core/Input.h"
#include "Rainy/Core/KeyCodes.h"
#include "Rainy/Core/Time.h"

#include "Rainy/Utils/Image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Rainy/Render/Buffer.h"
#include "Rainy/Render/VertexArray.h"
#include "Rainy/Render/FrameBuffer.h"
#include "Rainy/Render/Renderer.h"
#include "Rainy/Render/Texture2D.h"
#include "Rainy/Render/ShaderLibrary.h"
#include "Rainy/Render/Cameras/OrthographicCamera.h"
#include "Rainy/Render/Cameras/PerspectiveCamera.h"

#include "Rainy/Core/Entity.h"
#include "Rainy/3D/Light/Light.h"

using Rainy::Vector2f;
using Rainy::Entity;
using Rainy::VertexBuffer;
using Rainy::IndexBuffer;
using Rainy::VertexArray;
using Rainy::Texture2D;
using Rainy::Image;

constexpr uint32_t RESOLUTION = 8192;
constexpr float MAX_TESS_LEVEL = 32.f;
constexpr float PATCH_STEP = 4.f * MAX_TESS_LEVEL;
constexpr uint32_t MAX_SIZE = RESOLUTION / MAX_TESS_LEVEL;
constexpr float TEXT_STEP = 1.f / float(MAX_SIZE);

class Grid : public Entity
{
	struct PosAndTexCoord
	{
		Vector2f Position, TextureCoord;
	};

public:
	Grid(uint32_t size, Image* hmImage, Image* nmImage, Image* fillImage)
	{
		RN_ASSERT(size <= MAX_SIZE, "Grid: size > MAX_SIZE");
		
		m_tailCoord = { int(size), int(size) };

		std::array<PosAndTexCoord, 4> vertices = {
			PosAndTexCoord{ Vector2f{ 0.f, 0.f }, Vector2f{ 0.f, 0.f } },
			PosAndTexCoord{ Vector2f{ 0.f, PATCH_STEP }, Vector2f{ 0.f, TEXT_STEP } },
			PosAndTexCoord{ Vector2f{ PATCH_STEP, PATCH_STEP }, Vector2f{ TEXT_STEP, TEXT_STEP } },
			PosAndTexCoord{ Vector2f{ PATCH_STEP, 0.f }, Vector2f{ TEXT_STEP, 0.f } }
		};

		std::array<uint32_t, 4> indices = { 0, 1, 2, 3 };

		float indent = float(size) / 2 * PATCH_STEP;

		m_sections.reserve(size * size);

		for (uint32_t y = 0; y < size; y++)
		{
			for (uint32_t x = 0; x < size; x++)
			{
				m_sections.push_back(PosAndTexCoord{
					Vector2f{ x * PATCH_STEP - indent, y * PATCH_STEP - indent }, 
					Vector2f{ x * TEXT_STEP - indent, y * TEXT_STEP - indent}
				});
			}
		}

		m_vertexBuffer = Rainy::VertexBuffer::Create(sizeof(PosAndTexCoord) * vertices.size(), vertices.data());
		auto elemets = { Rainy::BufferElement(Rainy::FLOAT2, false), Rainy::BufferElement(Rainy::FLOAT2, false) };
		m_vertexBuffer->SetBufferLayout({ elemets,  vertices.size() });

		m_sectionsBuffer = Rainy::VertexBuffer::Create(sizeof(PosAndTexCoord) * m_sections.size(), m_sections.data());
		elemets = { Rainy::BufferElement(Rainy::FLOAT2, false, 1), Rainy::BufferElement(Rainy::FLOAT2, false, 1) };
		m_sectionsBuffer->SetBufferLayout({ elemets,  m_sections.size() });

		m_indexBuffer = Rainy::IndexBuffer::Create(sizeof(uint32_t) * indices.size(), indices.size(), indices.data());
		
		m_vertexArray = Rainy::VertexArray::Create();
		m_vertexArray->SetVertexBuffers({ m_vertexBuffer, m_sectionsBuffer });
		m_vertexArray->SetIndexBuffer(m_indexBuffer);
		m_vertexArray->SetInstanceCount(m_sections.size());

		RN_ASSERT(hmImage != nullptr, "hmImage is nullptr");
		RN_ASSERT(nmImage != nullptr, "nmImage is nullptr");

		m_heightMap = Texture2D::Create();
		m_heightMap->TextureData(hmImage);
		m_heightMap->SetTexUnit(0);

		m_normalMap = Texture2D::Create();
		m_normalMap->TextureData(nmImage);
		m_normalMap->SetTexUnit(m_heightMap->GetTexUnit() + 1);
	}

	~Grid()
	{
		delete m_vertexBuffer;
		delete m_sectionsBuffer;
		delete m_indexBuffer;
		delete m_vertexArray;
		delete m_heightMap;
		delete m_normalMap;
	}

	void Draw(Rainy::Shader* shader)
	{
		shader->Bind();
		m_heightMap->Bind();
		m_normalMap->Bind();
		
		shader->SetUniform("modelMatrix", GetModelMatrix());
		shader->SetUniform("maxTessLevel", MAX_TESS_LEVEL);
		shader->SetUniform("heightMap", int(m_heightMap->GetTexUnit()));
		shader->SetUniform("normalMap", int(m_normalMap->GetTexUnit()));
		shader->SetUniform("heightFactor", 1000.f);
		
		shader->SetUniform("detailLevel", 3000.f);

		Rainy::DrawVertexArray(Rainy::RN_PATCHES, m_vertexArray);
		
		m_normalMap->UnBind();
		m_heightMap->UnBind();
		shader->UnBind();
	}

	void AddSectionToTail()
	{
		m_tailCoord.x++;
		if (m_tailCoord.x > MAX_SIZE)
		{
			m_tailCoord.y++;
			m_tailCoord.x--;
		}
		RN_ASSERT(m_tailCoord.x <= MAX_SIZE && m_tailCoord.y <= MAX_SIZE, 
			"m_tailCoord.x > MAX_SIZE || m_tailCoord.y > MAX_SIZE");

		m_sections.push_back({
			Vector2f{ m_tailCoord.x * PATCH_STEP, m_tailCoord.y * PATCH_STEP },
			Vector2f{ m_tailCoord.x * TEXT_STEP, m_tailCoord.y * TEXT_STEP }
		});

		m_sectionsBuffer->Reallocate(m_sections.size(), m_sections.data());
	}

private:
	Rainy::Vector2i m_tailCoord;

	VertexBuffer* m_vertexBuffer;
	VertexBuffer* m_sectionsBuffer;
	IndexBuffer* m_indexBuffer;
	VertexArray* m_vertexArray;

	Texture2D* m_heightMap;
	Texture2D* m_normalMap;

	std::vector<PosAndTexCoord> m_sections;
};


// cam globals
constexpr float FOV = 60.f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 10000.f;
// cam globals

class TestTesselationLayer : public Rainy::Layer
{
public:
	TestTesselationLayer()
	{
		m_frameBuffer = Rainy::FrameBuffer::Create(m_contentAreaWidth, m_contentAreatHeight);
		m_terrainShader = Rainy::ShaderLibrary::CreateShader("res/shaders/TesselationTest.glsl");
		//m_lightBoxShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Terrain.glsl");

		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, FOV, NEAR_PLANE, FAR_PLANE,
			{ 0, 200, -2 }, { 30, 0, 0 }, 15.f, 2.0f);

		Rainy::EnablePolygonMode();

		/*Rainy::Image* image = Rainy::Image::Create("res/textures/hm.jpg");
		Rainy::Texture2D* heightMap = Rainy::Texture2D::Create();
		heightMap->TextureData(image);*/

		//constexpr uint32_t TEST_SIZE = 8193;
		//std::vector<uint8_t> someData;
		//someData.resize(TEST_SIZE * TEST_SIZE + TEST_SIZE, 0);
		//Texture2D* tex = Texture2D::Create();
		//tex->TextureData(TEST_SIZE, TEST_SIZE, 1, someData.data());

		Rainy::Image* hmi = Rainy::Image::Create("res/textures/hm.jpg");
		Rainy::Image* nmi = Rainy::Image::Create("res/textures/nm.jpg");

		m_grid = new Grid(MAX_SIZE, hmi, nmi, hmi);
		m_grid->SetScale({ 32.f / MAX_TESS_LEVEL });
		/*
		Rainy::TimeStep time0 = Rainy::TimeStep::GetTime();
		m_grid->AddSectionToTail();
		RN_APP_INFO("m_grid->AddSectionToTail(): Worst case! {0}", Rainy::TimeStep::GetTime().GetSeconds() - time0.GetSeconds());
*/
		//m_grid = new Grid(128, 32, hmi, nmi);
		//m_grid->SetScale({0.1f});
		// light setup
		m_light = new Rainy::Light();
		m_light->SetPosition({ 0, 100.f, 0 });

		// tests
		bool result = false;
		
	}

	~TestTesselationLayer()
	{}

	void OnEvent(Rainy::Event& e) override
	{}

	void OnUpdate() override
	{
		using Rainy::Input;

		if (m_focused && Input::IsKeyPressed(Rainy::RN_KEY_LEFT_ALT))
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			Rainy::Application::Get()->DisableCursor();
			m_perspCamera->UnFreeze();
		}
		else
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
			Rainy::Application::Get()->EnableCursor();
			m_perspCamera->Freeze();
		}
		m_perspCamera->OnUpdate();

		if (Input::IsMouseKeyPressed(Rainy::RN_MOUSE::BUTTON_1))
		{
			
		}

		Render();

		static float timeSum = 0;
		float frameTime = Rainy::Application::Get()->GetFrameTime();
		timeSum += frameTime;

		if (timeSum >= 1)
		{
			RN_APP_TRACE("FPS: {0}", 1.f / frameTime);
			timeSum = 0;
		}
	}

	void Render()
	{
		m_frameBuffer->Bind();
		Rainy::Clear();

		m_terrainShader->Bind();
		m_terrainShader->SetUniform("modelMatrix", Rainy::Matrix4f());
		m_terrainShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_terrainShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_light->ToShader(m_terrainShader);

		Rainy::EnableCullFace();
		m_grid->Draw(m_terrainShader);
		Rainy::DisableCullFace();


		m_frameBuffer->UnBind();
	}

	void OnImGuiRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		static uint16_t titleHeight = ImGui::GetFrameHeight();

		ImGui::Begin("Tesselation Test");

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		ImVec2 currentWinPos = ImGui::GetCurrentWindow()->Pos;

		auto cursorPosition = Rainy::Input::GetCursorPosition();
		auto windowPosition = Rainy::Application::Get()->GetWindow()->GetWindowsPosition();

		m_windowCursorPos = { cursorPosition.first - windowPosition.first - currentWinPos.x,
			cursorPosition.second - windowPosition.second - currentWinPos.y - titleHeight };

		if (windowWidth != m_contentAreaWidth || (windowHeight - titleHeight) != m_contentAreatHeight)
		{
			m_contentAreaWidth = windowWidth;
			m_contentAreatHeight = windowHeight - titleHeight;

			m_frameBuffer->SetSize(m_contentAreaWidth, m_contentAreatHeight);
			m_perspCamera->RecreateProjectionMatrix(float(m_contentAreaWidth) / m_contentAreatHeight, FOV, NEAR_PLANE, FAR_PLANE);

			Render();
		}

		ImGui::GetWindowDrawList()->AddImage(
			(void *)m_frameBuffer->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + windowWidth, ImGui::GetCursorScreenPos().y + windowHeight),
			ImVec2(0, 1), ImVec2(1, 0)
		);
		// set camera update flag
		m_focused = ImGui::IsWindowFocused();

		ImGui::End();

		ImGui::PopStyleVar();
	}

private:
	uint32_t m_contentAreaWidth = 1920;
	uint32_t m_contentAreatHeight = 1080;

	Vector2f m_windowCursorPos;
	bool m_focused;

	Rainy::FrameBuffer* m_frameBuffer;
	Rainy::Shader* m_terrainShader;
	Rainy::Shader* m_lightBoxShader;

	Rainy::PerspectiveCamera* m_perspCamera;

	Grid* m_grid;
	Rainy::Light* m_light;

};
