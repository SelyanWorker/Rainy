#pragma once

#include "core/Time.h"
#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/Time.h"

#include "utils/Image.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "application/Application.h"
#include "layers/Layer.h"
#include "render/Buffer.h"
#include "render/VertexArray.h"
#include "render/FrameBuffer.h"
#include "render/Renderer.h"
#include "render/Texture2D.h"
#include "render/ShaderLibrary.h"
#include "camera/OrthographicCamera.h"
#include "camera/PerspectiveCamera.h"

#include "light/Light.h"

#include "../Terrain/Terrainn.h"
#include "../Terrain/Tools.h"
#include "../Terrain/BrushT.h"

extern std::unique_ptr<std::istream> TERRAIN_SHADER_SRC_STREAM_PTR;
extern std::unique_ptr<std::istream> DRAW_NORMALS_SHADER_PTR;
extern std::unique_ptr<std::istream> POINTS_SHADER_PTR;
extern std::unique_ptr<std::istream> BASE_SHADER_PTR;

// cam globals
constexpr float FOV = 60.f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 100000.f;
// cam globals

using Rainy::VertexBuffer;
using Rainy::VertexArray;
using Rainy::Vector2f;
using Rainy::Vector3f;
using Rainy::Vector4f;
using Rainy::Matrix3f;
using Rainy::Matrix4f;

extern te::Terrain* terrain;
extern te::ToolTypes toolType;
extern te::ToolManager* toolManager;
extern te::Brush* brush;

extern bool DRAW_TERRAIN_GRID;
extern bool DRAW_TERRAIN_NORMALS;

extern float CAM_MOVE_SPEED;
extern float CAM_ROTATE_SPEED;

extern float SCULPT_STRENGTH;
extern float SMOOTH_STRENGTH;
extern int SMOOTH_LEVELS;

class TerrainLayer : public Rainy::Layer
{
public:
	TerrainLayer()
	{
		Rainy::Texture2D* colorTexture0 = Rainy::Texture2D::Create();
		colorTexture0->TextureData(m_contentAreaWidth, m_contentAreatHeight, 4, Rainy::TextureDataType::UNSIGNED_BYTE, nullptr);
		Rainy::Texture2D* colorTexture1 = Rainy::Texture2D::Create();
		colorTexture1->TextureData(m_contentAreaWidth, m_contentAreatHeight, 3, Rainy::TextureDataType::FLOAT, nullptr);
		m_frameBuffer = Rainy::FrameBuffer::Create({ colorTexture0, colorTexture1 });

		m_terrainShader = Rainy::ShaderLibrary::CreateShader(TERRAIN_SHADER_SRC_STREAM_PTR.get());
		m_terrainNormalsShader = Rainy::ShaderLibrary::CreateShader(DRAW_NORMALS_SHADER_PTR.get());
		m_pointsShader = Rainy::ShaderLibrary::CreateShader(POINTS_SHADER_PTR.get());
		m_lightBoxShader = Rainy::ShaderLibrary::CreateShader(BASE_SHADER_PTR.get());

		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, FOV, NEAR_PLANE, FAR_PLANE,
			{ 0, 10, 0 }, { 30, 180, 0 }, CAM_ROTATE_SPEED, CAM_MOVE_SPEED);

		//Rainy::EnablePolygonMode();

		m_intersectionPoints.reserve(100);

		m_interPointsBuffer = VertexBuffer::Create(m_intersectionPoints.capacity() * sizeof(Vector3f),
			m_intersectionPoints.data());
		auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false) };
		m_interPointsBuffer->SetBufferLayout({ elements, m_intersectionPoints.capacity() });
		m_interPointsVertexArray = VertexArray::Create();
		m_interPointsVertexArray->SetVertexBuffers({ m_interPointsBuffer });

		// terrain setup
		terrain = new te::Terrain(te::TerrainInitSize::_2, te::TerrainInitSize::_2);
		//terrain->SetShader(te::Terrain::terrainShader);
		//terrain->SetScale(0.1f);

		// tool and brush setuo
		toolManager = new te::ToolManager(terrain);
		brush = new te::Brush(10.f, 1.f);

		// light setup
		m_light = new Rainy::Light();
		m_light->SetPosition({ 100.f, 100.f, 0 });

		ImGui::SetNextWindowSize(ImVec2{ 1024, 768 });
		ImGui::SetNextWindowPos(ImVec2{ 0,19 });



		// test
		/*Rainy::Image* testImg = Rainy::Image::Create("res/textures/nm.jpg");
		Rainy::Texture2D* testTexture = Rainy::Texture2D::Create();
		testTexture->TextureData(testImg);
		Rainy::Image* testImg1 = Rainy::Image::Create(
			reinterpret_cast<uint8_t*>(testTexture->GetTextureData()),
			3, testTexture->GetWidth(), testTexture->GetHeight()
		);
		testImg1->SaveImage("res/textures/nmout.jpg", Rainy::ImageTypes::RN_JPEG);

		delete testImg1;
		delete testTexture;
		delete testImg;*/
		// test
	}

	~TerrainLayer()
	{}

	void OnEvent(Rainy::Event& e) override
	{}

	void OnUpdate() override
	{
		using Rainy::Input;

		m_perspCamera->m_rotationSpeed = CAM_ROTATE_SPEED;
		m_perspCamera->m_moveSpeed = CAM_MOVE_SPEED;

		static bool pButtomRelease = true;
		if (pButtomRelease && Input::IsKeyPressed(Rainy::RN_KEY_P))
		{
			static bool polyModeEnable = false;
			polyModeEnable = !polyModeEnable;
			if (polyModeEnable) Rainy::EnablePolygonMode();
			else Rainy::DisablePolygonMode();
			pButtomRelease = false;
		}
		else if (Input::IsKeyReleased(Rainy::RN_KEY_P))
		{
			pButtomRelease = true;
		}

		if (Input::IsKeyPressed(Rainy::RN_KEY_F))
		{
			terrain->Flat();
		}

		static bool uButtomRelease = true;
		if (uButtomRelease && Input::IsKeyPressed(Rainy::RN_KEY_U))
		{
			toolManager->Undo();
			uButtomRelease = false;
		}
		else if (Input::IsKeyReleased(Rainy::RN_KEY_U)) uButtomRelease = true;

		static bool rButtomRelease = true;
		if (rButtomRelease && Input::IsKeyPressed(Rainy::RN_KEY_R))
		{
			toolManager->Redo();
			rButtomRelease = false;
		}
		else if (Input::IsKeyReleased(Rainy::RN_KEY_R)) rButtomRelease = true;

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

		Vector3f rayDir = ViewportSpaceToWorldSpace(
			m_windowCursorPos, m_perspCamera->GetProjectionMatrix(), m_perspCamera->GetViewMatrix(),
			m_contentAreaWidth, m_contentAreatHeight);
		rayDir.normalize();

		constexpr float RAY_DISTANCE = 100000.f;
		bool intersaction =
			terrain->RayIntersection(m_interPoint, rayDir, m_perspCamera->GetPosition(), RAY_DISTANCE);

		if (m_focused && Input::IsMouseKeyPressed(Rainy::RN_MOUSE::BUTTON_1))
		{
			toolManager->BeginEdit();

			static constexpr float DELAY = 40;
			static float prevTime = Rainy::TimeStep::GetTime().GetMilli() - DELAY;
			Rainy::TimeStep currentTime = Rainy::TimeStep::GetTime();
			//if ((currentTime.GetMilli() - prevTime) >= DELAY)
			if (true)
			{
				//RN_APP_INFO("Intersection {0}, Point: {1}", intersaction, m_interPoint.str());

				if (toolType == te::ToolTypes::ADD_SECTION)
					terrain->AddSection({ m_interPoint.x, m_interPoint.z });

				if (intersaction)
				{
					if (toolType == te::ToolTypes::DELETE_SECTION)
						terrain->RemoveSection({ m_interPoint.x, m_interPoint.z });
					else
					{
						static Vector2f staticCursorPos = m_windowCursorPos;
						static Vector3f prevInterPoint = m_interPoint;
						if (staticCursorPos != m_windowCursorPos)
						{
							staticCursorPos = m_windowCursorPos;
							prevInterPoint = m_interPoint;
						}
						/*if(!m_useSmoothTool) toolManager->ApplySculpt(*brush, prevInterPoint, 1.25f);
						else toolManager->ApplySmooth(*brush, prevInterPoint, 1.25f, 1);*/

						switch (toolType)
						{
						case te::ToolTypes::SCULPT:
							toolManager->ApplySculpt(*brush, prevInterPoint, SCULPT_STRENGTH); break;
						case te::ToolTypes::SMOOTH:
							toolManager->ApplySmooth(*brush, prevInterPoint, SMOOTH_STRENGTH, SMOOTH_LEVELS); break;
						default:
							break;
						}
					}

					m_intersectionPoints.push_back(m_interPoint);

					static uint32_t prevCapacity = m_intersectionPoints.capacity();
					uint64_t pointCount = m_intersectionPoints.size();

					auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false) };
					m_interPointsBuffer->SetBufferLayout({ elements, pointCount });

					if (prevCapacity < pointCount)
					{
						m_intersectionPoints.reserve(pointCount * 2);
						prevCapacity = m_intersectionPoints.capacity();
						m_interPointsBuffer->Reallocate(m_intersectionPoints.capacity() * sizeof(Vector3f), nullptr);
						m_interPointsBuffer->SubData(0, m_intersectionPoints.size() * sizeof(Vector3f),
							m_intersectionPoints.data());
					}
					else
					{
						m_interPointsBuffer->SubData((m_intersectionPoints.size() - 1) * sizeof(Vector3f),
							sizeof(Vector3f), &m_intersectionPoints.back());
					}
				}
			}
			prevTime = currentTime.GetMilli();
		}
		else
		{
			toolManager->EndEdit();
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

		Matrix4f projM = m_perspCamera->GetProjectionMatrix();
		Matrix4f viewM = m_perspCamera->GetViewMatrix();
		//RN_APP_INFO("ndcIntePoint : {0}", ndcIntePoint.str());

		m_terrainShader->Bind();
		m_terrainShader->SetUniform("projectionMatrix", projM);
		m_terrainShader->SetUniform("viewMatrix", viewM);
		m_terrainShader->SetUniform("interPoint", m_interPoint);
		m_terrainShader->SetUniform("brushSize", brush->size);
		m_terrainShader->SetUniform("brushColor", Rainy::Vector3f{ 0, 1.f, 0 });
		m_terrainShader->SetUniform("drawGrid", DRAW_TERRAIN_GRID);
		m_terrainShader->SetUniform("gridSize",  0.025f);
		m_terrainShader->SetUniform("gridColor", Rainy::Vector4f{ 0, 1.f, 0, 1.f });
		m_light->ToShader(m_terrainShader);

		Rainy::EnableCullFace();
		terrain->Draw(m_terrainShader);
		Rainy::DisableCullFace();

		if (DRAW_TERRAIN_NORMALS)
		{
			m_terrainNormalsShader->Bind();
			m_terrainNormalsShader->SetUniform("projectionMatrix", projM);
			m_terrainNormalsShader->SetUniform("viewMatrix", viewM);
			Rainy::SetLineWidth(2.f);
			terrain->Draw(m_terrainNormalsShader);
			Rainy::SetLineWidth(1.f);
		}

		/*float prevPointSize = Rainy::GetPointSize();
		Rainy::SetPointSize(3.f);
		m_pointsShader->Bind();
		m_pointsShader->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_pointsShader->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_pointsShader->SetUniform("pointColor", Rainy::Vector4f(1.f, 0, 0, 1));

		Rainy::DrawVertexArray(Rainy::RN_POINTS, m_interPointsVertexArray);
		Rainy::SetPointSize(prevPointSize)*/;

		m_frameBuffer->UnBind();
	}

	void OnImGuiRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		static uint16_t titleHeight = ImGui::GetFrameHeight();

		//ImGuiID testId = ImGui::GetWindowDockID();
		//ImGui::SetNextWindowDockID(testId, 0);

		ImGui::Begin("Terrain");

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
			(void *)m_frameBuffer->GetAttachmentColorTexture(0)->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + windowWidth, ImGui::GetCursorScreenPos().y + windowHeight),
			ImVec2(0, 1), ImVec2(1, 0)
		);
		// set camera update flag
		m_focused = ImGui::IsWindowFocused();

		ImGui::End();

		ImGui::PopStyleVar();
	}

	//void GenerateHeightMap(const char* file, Rainy::ImageTypes type)
	//{
	//	Rainy::Image* heightMap = terrain->GetHeightMapImage();
	//	if (!heightMap->SaveImage(file, type)) RN_APP_ERROR("Image {0} didn't save", file);
	//}

	//Rainy::Image* GenerateNormalMap()
	//{
	//	Rainy::Texture2D* normalTexture = m_frameBuffer->GetAttachmentColorTexture(1);
	//	uint32_t ntWidth = normalTexture->GetWidth();
	//	uint32_t ntHeight = normalTexture->GetHeight();
	//	float* normalsData = static_cast<float*>(normalTexture->GetTextureData());
	//	for (size_t i = 0; i < ntWidth * ntHeight * 3; i += 3)
	//	{
	//		float r = normalsData[i];
	//		float g = normalsData[i + 1];
	//		float b = normalsData[i + 2];
	//	}

	//	return nullptr;
	//	/*Rainy::Image* normalMap;
	//	if (!normalMap->SaveImage(file, type)) RN_APP_ERROR("Image {0} didn't save", file);*/
	//}

private:
	uint32_t m_contentAreaWidth = 1920;
	uint32_t m_contentAreatHeight = 1080;

	Vector2f m_windowCursorPos;
	bool m_focused;

	Rainy::Shader* m_pointsShader;
	std::vector<Vector3f> m_intersectionPoints;
	VertexBuffer* m_interPointsBuffer;
	VertexArray* m_interPointsVertexArray;

	Rainy::FrameBuffer* m_frameBuffer;
	Rainy::Shader* m_lightBoxShader;

	Rainy::PerspectiveCamera* m_perspCamera;

	Rainy::Shader* m_terrainShader;
	Rainy::Shader* m_terrainNormalsShader;

	Rainy::Light* m_light;

	Vector3f m_interPoint;
};


