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

#include "Rainy/3D/Light/Light.h"

using Rainy::Vector3f;
using Rainy::Vector2f;

constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 100.f;
constexpr float FOV = 60.f;

class BoxLayer : public Rainy::Layer
{
public:
	BoxLayer()
	{
		m_frameBuffer = Rainy::FrameBuffer::Create(m_contentAreaWidth, m_contentAreatHeight);
		m_shaderProgram = Rainy::ShaderLibrary::CreateShader("res/shaders/Base.glsl");
		//m_shaderProgramToNormals = Rainy::ShaderLibrary::CreateShader("res/shaders/Normal.glsl");

		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
		float widthHalf = float(m_contentAreaWidth) / 2;
		float heightHalf = float(m_contentAreatHeight) / 2;
		//RN_CORE_INFO("AspectRatio: {0}", aspectRation);
		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, FOV, NEAR_PLANE, FAR_PLANE,
			{ 0, 0, -8 }, { 0, 0, 0 }, 35.f, 0.01f);

		m_box0 = new Rainy::Box({ 1, 1, 1 });
		m_box0->SetPosition({ 0, 0, 0 });
		m_box1 = new Rainy::Box({ 1, 1, 1 });
		m_box1->SetPosition({ 0, 0, 2 });
		m_box2 = new Rainy::Box({ 1, 1, 1 });
		m_box2->SetPosition({ 0, 0, 4 });

		m_sunStrength = 0.5f;
		//Rainy::EnablePolygonMode();
	}

	~BoxLayer()
	{}

	void OnEvent(Rainy::Event& e) override
	{}

	void OnUpdate() override
	{
		using Rainy::Input;

		if (Input::IsKeyPressed(Rainy::RN_KEY_UP))
			m_sunStrength += Rainy::Application::Get()->GetFrameTime();
		if (Input::IsKeyPressed(Rainy::RN_KEY_DOWN))
			m_sunStrength -= Rainy::Application::Get()->GetFrameTime();

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

		/*Vector3f rayDir = ViewportSpaceToWorldSpace(
			m_windowCursorPos, m_perspCamera->GetProjectionMatrix(), m_perspCamera->GetViewMatrix(),
			m_contentAreaWidth, m_contentAreatHeight);
		rayDir.normalize();

		Vector3f interPoint;
		float rayScale;
		if (Rainy::InterRayAndSphere(m_box0->GetPosition(), m_box0->GetScale().x,
			m_perspCamera->GetPosition(), rayDir, interPoint, rayScale))
			RN_APP_INFO("Intersection with box, InterPoint: {0}, VecScale: {1}", interPoint.str(), rayScale);
*/

		Render();
	}

	void Render()
	{
		//RN_APP_TRACE("Persp matrix:\n {0}", m_perspCamera->GetProjectionMatrix().String());
		

		m_frameBuffer->Bind();
		Rainy::Clear();

		m_shaderProgram->Bind();
		m_shaderProgram->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_shaderProgram->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		
		/*m_shaderProgram->SetUniform("intensity", m_sunStrength);
		m_shaderProgram->SetUniform("lightPosition", Rainy::Vector3f(100, 100, -100));*/

		Rainy::RendererTest();

		Rainy::EnableCullFace();
		m_shaderProgram->SetUniform("modelMatrix", m_box0->GetModelMatrix());
		m_shaderProgram->SetUniform("mainColor", Rainy::Vector4f(1.f, 0, 0, 1));
		m_box0->Draw();
		m_shaderProgram->SetUniform("modelMatrix", m_box1->GetModelMatrix());
		m_shaderProgram->SetUniform("mainColor", Rainy::Vector4f(0, 1.f, 0, 1));
		m_box1->Draw();
		m_shaderProgram->SetUniform("modelMatrix", m_box2->GetModelMatrix());
		m_shaderProgram->SetUniform("mainColor", Rainy::Vector4f(0, 0, 1.f, 1));
		m_box2->Draw();
		Rainy::DisableCullFace();

		/*m_shaderProgramToNormals->Bind();
		m_shaderProgramToNormals->SetUniform("projectionMatrix", m_perspCamera->GetProjectionMatrix());
		m_shaderProgramToNormals->SetUniform("viewMatrix", m_perspCamera->GetViewMatrix());
		m_shaderProgramToNormals->SetUniform("modelMatrix", m_box0->GetModelMatrix());
		float prevLineWidth = Rainy::GetLineWidth();
		Rainy::SetLineWidth(3.f);
		m_box0->Draw();
		Rainy::SetLineWidth(prevLineWidth);*/

		m_frameBuffer->UnBind();
	}

	void OnImGuiRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("BoxLayer");

		static uint16_t titleHeight = ImGui::GetFrameHeight();

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
			m_perspCamera->RecreateProjectionMatrix(float(m_contentAreaWidth) / m_contentAreatHeight, 
				FOV, NEAR_PLANE, FAR_PLANE);

			Render();
		}

		ImGui::GetWindowDrawList()->AddImage(
			(void *)m_frameBuffer->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + windowWidth, ImGui::GetCursorScreenPos().y + windowHeight),
			ImVec2(0, 1), ImVec2(1, 0));
		
		m_focused = ImGui::IsWindowFocused();
		ImGui::End();

		ImGui::PopStyleVar();
	}

private:
	uint32_t m_contentAreaWidth = 1920;
	uint32_t m_contentAreatHeight = 1080;

	Vector2f m_windowCursorPos;
	bool m_focused;
	float m_sunStrength;

	Rainy::FrameBuffer* m_frameBuffer;
	Rainy::Shader* m_shaderProgram;
	//Rainy::Shader* m_shaderProgramToNormals;

	Rainy::Box* m_box0;
	Rainy::Box* m_box1;
	Rainy::Box* m_box2;
	Rainy::PerspectiveCamera* m_perspCamera;
};