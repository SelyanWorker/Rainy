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

#include "Rainy/3D/Box.h"

using Rainy::Entity;
using Rainy::VertexBuffer;
using Rainy::IndexBuffer;
using Rainy::VertexArray;
using Rainy::Texture2D;
using Rainy::Image;

constexpr float PATCH_STEP = 1.f;

class Grid : public Entity
{
public:
	Grid(uint32_t width, uint32_t height, Image* hmImage = nullptr, Image* nmImage = nullptr)
	:	m_width(width),
		m_heigth(height)
	{
		uint32_t vInW = width + 1;
		uint32_t vInH = height + 1;

		float uTexStep = 1.f / float(width);
		float vTexStep = 1.f / float(height);

		float wIndent = width / 2 * PATCH_STEP;
		float hIndent = height / 2 * PATCH_STEP;

		float mainHeight = 0;
		float heightStep = 0.05f;

		std::vector<float> vertices;
		vertices.reserve(4 * vInW * vInH);
		for (uint32_t y = 0; y < vInH; y++)
		{
			for (uint32_t x = 0; x < vInW; x++)
			{
				vertices.push_back(x * PATCH_STEP - wIndent);
				vertices.push_back(0);
				vertices.push_back(y * PATCH_STEP - hIndent);

				vertices.push_back(x * uTexStep);
				vertices.push_back(y * vTexStep);
			}
		}

		std::vector<uint32_t> indices;
		indices.reserve(width * height * 6);
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				indices.push_back(y * vInW + x);
				indices.push_back(y * vInW + x + 1);
				indices.push_back((y + 1) * vInW + (x + 1));
				
				indices.push_back((y + 1) * vInW + (x + 1));
				indices.push_back((y + 1) * vInW + x);
				indices.push_back(y * vInW + x);
			}
		}

		m_vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(float), vertices.data());
		auto elements = { Rainy::BufferElement(Rainy::ShaderDataType::FLOAT3, false), Rainy::BufferElement(Rainy::ShaderDataType::FLOAT2, false) };
		m_vertexBuffer->SetBufferLayout(Rainy::BufferLayout{ elements, vertices.size() / 5 });

		m_indexBuffer = IndexBuffer::Create(indices.size() * sizeof(uint32_t), indices.size(), indices.data());

		m_vertexArray = VertexArray::Create();
		m_vertexArray->SetVertexBuffers({ m_vertexBuffer });
		m_vertexArray->SetIndexBuffer(m_indexBuffer);

		RN_ASSERT(hmImage != nullptr, "hmImage is nullptr");
		RN_ASSERT(nmImage != nullptr, "nmImage is nullptr");

		m_heightMap = Texture2D::Create();
		m_heightMap->TextureData(hmImage);
		m_heightMap->SetTexUnit(0);

		m_normalMap = Texture2D::Create();;
		m_normalMap->TextureData(nmImage);
		m_normalMap->SetTexUnit(m_heightMap->GetTexUnit() + 1);
	}

	~Grid() = default;

	void Draw(Rainy::Shader* shader)
	{
		shader->Bind();
		//m_heightMap->Bind();
		m_normalMap->Bind();

		shader->SetUniform("modelMatrix", GetModelMatrix());
		shader->SetUniform("heightMap", int(m_heightMap->GetTexUnit()));
		shader->SetUniform("normalMap", int(m_normalMap->GetTexUnit()));
		shader->SetUniform("heightFactor", 10.f);

		Rainy::DrawVertexArray(Rainy::RN_TRIANGLES, m_vertexArray);

		m_normalMap->UnBind();
		//m_heightMap->UnBind();
		shader->UnBind();
	}

private:
	uint32_t m_width;
	uint32_t m_heigth;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	VertexArray* m_vertexArray;

	Texture2D* m_heightMap;
	Texture2D* m_normalMap;
};


// cam globals
constexpr float FOV = 60.f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 10000.f;
// cam globals

using Rainy::VertexBuffer;
using Rainy::VertexArray;
using Rainy::Vector2f;
using Rainy::Vector3f;

class GridTestLayer : public Rainy::Layer
{
public:
	GridTestLayer()
	{
		m_frameBuffer = Rainy::FrameBuffer::Create(m_contentAreaWidth, m_contentAreatHeight);
		m_terrainShader = Rainy::ShaderLibrary::CreateShader("res/shaders/GridHMandNM.glsl");
		m_basicShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Base.glsl");

		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, FOV, NEAR_PLANE, FAR_PLANE,
			{ 0, 10, 0 }, { 30, 180, 0 }, 15.f, 0.1f);

		Rainy::EnablePolygonMode();

		Rainy::Image* hmi = Rainy::Image::Create("res/textures/hm.jpg");
		Rainy::Image* nmi = Rainy::Image::Create("res/textures/nm.jpg");

		m_grid = new Grid(8192, 8192, hmi, nmi);
		m_grid->SetRotation({ 180, 0, 0 });
		m_box = new Rainy::Box({ 1, 1, 1 });
		m_box->SetPosition({ 0, 20.f, 0 });
		// light setup
		m_light = new Rainy::Light();
		m_light->SetPosition({ 0, 100.f, 0 });

		// tests
		bool result = false;

		Rainy::EnableCullFace();
	}

	~GridTestLayer()
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
		Rainy::Setup();

		m_terrainShader->Bind();
		m_light->ToShader(m_terrainShader);
		m_terrainShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_terrainShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_terrainShader->SetUniform("modelMatrix", m_grid->GetModelMatrix());
		m_grid->Draw(m_terrainShader);
		m_terrainShader->UnBind();

		m_basicShader->Bind();
		m_basicShader->SetUniform("modelMatrix", m_box->GetModelMatrix());
		m_basicShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_basicShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_basicShader->SetUniform("mainColor", Rainy::Vector4f{1.f, 0, 0, 1.f});
		m_box->Draw();
		m_basicShader->UnBind();


		m_frameBuffer->UnBind();
	}

	void OnImGuiRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		static uint16_t titleHeight = ImGui::GetFrameHeight();

		ImGui::Begin("Grid Test");

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		ImVec2 currentWinPos = ImGui::GetCurrentWindow()->Pos;

		auto cursorPosition = Rainy::Input::GetCursorPosition();
		auto windowPosition = Rainy::Application::Get()->GetWindow()->GetWindowsPosition();

		m_windowCursorPos = { cursorPosition.first - windowPosition.first - currentWinPos.x,
			cursorPosition.second - windowPosition.second - currentWinPos.y - titleHeight };

		/*	RN_APP_TRACE("WinPos : x{0}, y{1} m_windowCursorPos : x{2}, y{3}",
				currentWinPos.x, currentWinPos.y, cursorPosition.first, cursorPosition.second);
	*/
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
	Rainy::Shader* m_basicShader;

	Rainy::PerspectiveCamera* m_perspCamera;

	Grid* m_grid;
	Rainy::Box* m_box;

	Rainy::Light* m_light;
};
