#pragma once



//
//class SpriteLayer : public Rainy::Layer
//{
//public:
//	SpriteLayer()
//	{
//		m_frameBuffer = Rainy::FrameBuffer::Create(m_width, m_height);
//		m_shaderProgram = Rainy::ShaderLibrary::CreateShader("res/shaders/Sprite.glsl");
//
//		Rainy::Image* image = Rainy::Image::Create("res/2D/fire-skull.png");
//		RN_ASSERT(image, "Sandbox.cpp 28");
//		
//		Rainy::Texture2D* texture = Rainy::Texture2D::Create();
//		texture->TextureData(image);
//		
//		{
//			m_spriteSheet = new Rainy::SpriteSheet(texture, 1, 8);
//			float lifeTime = 0.15f;
//			m_fireSkull = new Rainy::Sprite(m_spriteSheet, { 1, 1, lifeTime });
//			m_fireSkull->AddSpriteFrame(1, 2, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 3, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 4, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 5, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 6, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 7, lifeTime);
//			m_fireSkull->AddSpriteFrame(1, 8, lifeTime);
//		}
//
//
//		/*
//		m_projectionMatrix = Rainy::GetOrthographicMatrix(-float(m_width)/2, float(m_width) /2, 
//			-float(m_height)/2, float(m_height)/2, 0.1f, 100.f);
//		*/
//
//		
//		float aspectRation = float(m_height) / m_width;
//		float widthHalf = float(m_width) / 2;
//		float heightHalf = float(m_height) / 2;
//		//RN_CORE_INFO("AspectRatio: {0}", aspectRation);
//		m_camera = new Rainy::OrthographicCamera(-1, 1, -1, 1,
//			-1.f, 1.f, { 0, 0, 0 }, { 0, 0, 0 });
//	}
//
//	~SpriteLayer()
//	{}
//
//	void OnEvent(Rainy::Event& e) override
//	{}
//
//	void OnUpdate() override
//	{
//		m_camera->OnUpdate();
//		Render();
//	}
//
//	void Render()
//	{
//		m_frameBuffer->Bind();
//		
//		m_shaderProgram->Bind();
//		m_shaderProgram->SetUniform("projectionMatrix", m_camera->GetProjectionMatrix());
//		m_shaderProgram->SetUniform("viewMatrix", m_camera->GetViewMatrix());
//
//		Rainy::Clear();
//		m_fireSkull->Draw(m_shaderProgram);
//
//		m_frameBuffer->UnBind();
//	}
//
//	void OnImGuiRender() override
//	{
//		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
//
//		ImGui::Begin("Test");
//		float width = ImGui::GetWindowWidth();
//		float height = ImGui::GetWindowHeight();
//		ImGui::GetWindowDrawList()->AddImage(
//			(void *)m_frameBuffer->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
//			ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + height),
//			ImVec2(0, 1), ImVec2(1, 0));
//		ImGui::End();
//
//		ImGui::PopStyleVar();
//	}
//
//private:
//	uint32_t m_width = 800;
//	uint32_t m_height = 600;
//
//	Rainy::FrameBuffer* m_frameBuffer;
//	Rainy::Shader* m_shaderProgram;
//	Rainy::SpriteSheet* m_spriteSheet;
//	Rainy::Sprite* m_fireSkull;
//
//	Rainy::OrthographicCamera* m_camera;
//};
//
//class LandscapeLayer : public Rainy::Layer
//{
//public:
//	LandscapeLayer()
//	{
//		m_frameBuffer = Rainy::FrameBuffer::Create(m_contentAreaWidth, m_contentAreatHeight);
//		m_mainShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Landscape.glsl");
//		m_pointsShader = Rainy::ShaderLibrary::CreateShader("res/shaders/Points.glsl");
//
//		float aspectRation = float(m_contentAreaWidth) / m_contentAreatHeight;
//		float widthHalf = float(m_contentAreaWidth) / 2;
//		float heightHalf = float(m_contentAreatHeight) / 2;
//		//RN_CORE_INFO("AspectRatio: {0}", aspectRation);
//		m_perspCamera = new Rainy::PerspectiveCamera(aspectRation, 60.f, 0.1f, 100.f,
//			{ 0, 1, -8 }, { -10, 0, 0 }, 35.f, 0.01f);
//
//		box = new Rainy::Box({ 1, 1, 1 });
//
//		//landscape = new Landscape({0, 0, 0}, 10, 10, 32, 1.f);
//		//current_tool->setLandscape(landscape);
//
//		newLandscape = new NewLandscape({ 0, 0, 0 }, 4, 4, 8, 1.f);
//
//		m_intersectionPoints.reserve(1000);
//
//		m_interPointsBuffer = VertexBuffer::Create(m_intersectionPoints.capacity() * sizeof(Vector3f),
//			m_intersectionPoints.data());
//		auto elements = { Rainy::BufferElement(Rainy::FLOAT3, false) };
//		m_interPointsBuffer->SetBufferLayout({elements, 1000});
//		m_interPointsVertexArray = VertexArray::Create();
//		m_interPointsVertexArray->SetVertexBuffer(m_interPointsBuffer);
//		/*m_interPointsBuffer = Rainy::CreateBuffer();
//		Rainy::BufferData(m_interPointsBuffer, m_intersectionPoints.data(), 
//			m_intersectionPoints.capacity() * sizeof(Vector3f));
//		m_interPointsArrayBuffer = Rainy::CreateVertexArray();
//		Rainy::BufferAttach(m_interPointsArrayBuffer, m_interPointsBuffer, 0, 3);*/
//	}
//
//	~LandscapeLayer()
//	{}
//
//	void OnEvent(Rainy::Event& e) override
//	{}
//
//	void OnUpdate() override
//	{
//		if (m_focused && Rainy::Input::IsKeyPressed(Rainy::RN_KEY_LEFT_ALT))
//		{
//			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
//			Rainy::Application::Get()->DisableCursor();
//			m_perspCamera->UnFreeze();
//		}
//		else
//		{
//			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
//			Rainy::Application::Get()->EnableCursor();
//			m_perspCamera->Freeze();
//		}
//
//		m_perspCamera->OnUpdate();
//
//		// old tool code
//		Vector3f rayDir = ViewportSpaceToWorldSpace(
//			m_windowCursorPos,m_perspCamera->GetProjectionMatrix(), m_perspCamera->GetViewMatrix(),
//			m_contentAreaWidth, m_contentAreatHeight);
//		rayDir.normalize();
//	
//		static constexpr float RAY_DISTANCE = 10000.f;
//	/*	current_tool->updateLastIntersectionPoint({ m_perspCamera->GetPosition(),
//			rayDir, RAY_DISTANCE });*/
//		//static Vector3f interPoint = current_tool->getLastIntersectionPoint();
//		/*if (interPoint != current_tool->getLastIntersectionPoint())
//			interPoint = current_tool->getLastIntersectionPoint();*/		
//
//		//static Vector3f interPoint;
//		Vector3f interPoint;
//		bool intersaction =
//			newLandscape->RayIntersection(interPoint, rayDir, m_perspCamera->GetPosition(), RAY_DISTANCE);
//
//		// ћожет быть рассчитывать точку Intersection point по нажатию мыши? 
//		if (Rainy::Input::IsMouseKeyPressed(Rainy::RN_MOUSE::BUTTON_1))
//		{
//			// passed time in milliseconds
//			using Rainy::TimeStep;
//			
//			static constexpr float DELAY = 7.5;
//			static float prevTime = TimeStep::GetTime().GetMilli() - DELAY;
//			TimeStep currentTime = TimeStep::GetTime();
//			if ((currentTime.GetMilli() - prevTime) >= DELAY)
//			{
//				NewLandscapeSection* section = newLandscape->GetSection(interPoint);
//				if (section != nullptr && intersaction)
//				{
//					section->selected = !section->selected;
//					m_intersectionPoints.push_back(interPoint);
//					
//					static uint32_t prevCapacity = m_intersectionPoints.capacity();
//					size_t pointCount = m_intersectionPoints.size();
//					if (prevCapacity < pointCount)
//					{
//						m_intersectionPoints.reserve(pointCount * 2);
//						prevCapacity = m_intersectionPoints.capacity();
//						m_interPointsBuffer->Reallocate(m_intersectionPoints.capacity() * sizeof(Vector3f), nullptr);
//						m_interPointsBuffer->SubData(0, m_intersectionPoints.size() * sizeof(Vector3f),
//							m_intersectionPoints.data());
//						/*Rainy::BufferData(m_interPointsBuffer, nullptr,
//							m_intersectionPoints.capacity() * sizeof(Vector3f));
//						Rainy::BufferSubData(m_interPointsBuffer, 0,
//							m_intersectionPoints.size() * sizeof(Vector3f),
//							m_intersectionPoints.data());*/
//					}
//					else
//					{
//						m_interPointsBuffer->SubData((m_intersectionPoints.size() - 1) * sizeof(Vector3f),
//							sizeof(Vector3f), &m_intersectionPoints.back());
//						/*Rainy::BufferSubData(m_interPointsBuffer,
//							(m_intersectionPoints.size() - 1) * sizeof(Vector3f),
//							sizeof(Vector3f), &m_intersectionPoints.back());*/
//					}
//					//current_tool->update(*(current_brush));
//				}
//				else
//				{
//					newLandscape->AddSection(interPoint);
//				}
//			}
//			prevTime = currentTime.GetMilli();
//			/*Rainy::BufferData(m_interPointsBuffer, m_intersectionPoints.data(), 
//				m_intersectionPoints.size() * sizeof(Vector3f));*/
//			
//		}
//		//
//
//		Render();
//
//	}
//
//	void Render()
//	{
//		m_frameBuffer->Bind();
//		
//		Rainy::Clear();
//	
//		m_mainShader->Bind();
//
//		m_mainShader->SetUniformMatrix4f("projectionMatrix", m_perspCamera->GetProjectionMatrix());
//		m_mainShader->SetUniformMatrix4f("viewMatrix", m_perspCamera->GetViewMatrix());
//		m_mainShader->SetUniformMatrix4f("modelMatrix", box->GetModelMatrix());
//
//		Rainy::EnablePolygonMode();
//		//box->OnDraw();
//		newLandscape->Draw(m_mainShader);
//		Rainy::DisablePolygonMode();
//
//		float prevPointSize = Rainy::GetPointSize();
//		Rainy::SetPointSize(5.f);
//		m_pointsShader->Bind();
//		m_pointsShader->SetUniformMatrix4f("projectionMatrix", m_perspCamera->GetProjectionMatrix());
//		m_pointsShader->SetUniformMatrix4f("viewMatrix", m_perspCamera->GetViewMatrix());
//		m_pointsShader->SetUniformVector4f("pointColor", Rainy::Vector4f(1.f, 0, 0, 1));
//		
//		//Rainy::DrawArray(Rainy::RN_POINTS, m_interPointsArrayBuffer, m_intersectionPoints.size());
//		Rainy::DrawVertexArray(Rainy::RN_POINTS, m_interPointsVertexArray);
//		Rainy::SetPointSize(prevPointSize);
//
//		m_frameBuffer->UnBind();
//	}
//
//	void OnImGuiRender() override
//	{
//		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
//		
//		static uint16_t titleHeight = ImGui::GetFrameHeight();
//
//		ImGui::Begin("Test");
//
//		float windowWidth = ImGui::GetWindowWidth();
//		float windowHeight = ImGui::GetWindowHeight();
//
//		ImVec2 currentWinPos = ImGui::GetCurrentWindow()->Pos;
//
//		auto cursorPosition = Rainy::Input::GetCursorPosition();
//		
//		m_windowCursorPos = { cursorPosition.first - currentWinPos.x,
//			cursorPosition.second - currentWinPos.y - titleHeight };
//		/*RN_APP_INFO("mainWinPos: x {0}, y {1};",
//			mainWinPos.first, mainWinPos.second);*/
//
//		/*RN_APP_INFO("WinCursorPos: x {0}, y {1}; ",
//			m_windowCursorPos.x, m_windowCursorPos.y);*/
//
//		/*RN_APP_INFO("currentWinPos: x {0}, y {1}; viewPortPos: x {2}, y {3}; ",
//			currentWinPos.x, currentWinPos.y, viewPortPos.x, viewPortPos.y);
//*/
//
//		/*m_mousePosXOffset = std::max(currentWinPos.x - viewPortPos.x, 0.f);
//		m_mousePosYOffset = std::max(currentWinPos.y - viewPortPos.y + titleHeight, 0.f);*/
//
//	/*	RN_APP_INFO("mouseOffset: x {0}, y {1}; ",
//			m_mousePosXOffset, m_mousePosYOffset); 
//*/
//		
//
//		if (windowWidth != m_contentAreaWidth || 
//			(windowHeight - titleHeight) != m_contentAreatHeight)
//		{
//			/*ImVec2 nodePos = node->Pos;
//			ImVec2 winPos = node->VisibleWindow->Pos;
//
//			RN_APP_INFO("nodePos: x {0}, y {1}; winPos - nodePos: x {2}, y {3}", 
//				nodePos.x, nodePos.y, winPos.x , winPos.y);
//			*/
//			m_contentAreaWidth = windowWidth;
//			m_contentAreatHeight = windowHeight - titleHeight;
//
//			m_frameBuffer->SetSize(m_contentAreaWidth, m_contentAreatHeight);
//			m_perspCamera->RecreateProjectionMatrix(float(m_contentAreaWidth) / m_contentAreatHeight, 60.f, 0.1f, 100.f);
//			
//			Render();
//		}
//
//		ImGui::GetWindowDrawList()->AddImage(
//			(void *)m_frameBuffer->GetNative(), ImVec2(ImGui::GetCursorScreenPos()),
//			ImVec2(ImGui::GetCursorScreenPos().x + windowWidth, ImGui::GetCursorScreenPos().y + windowHeight),
//			ImVec2(0, 1), ImVec2(1, 0)
//		);
//
//
//		/*{
//			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
//			ImVec2 vMax = ImGui::GetWindowContentRegionMax();
//
//			vMin.x += ImGui::GetWindowPos().x;
//			vMin.y += ImGui::GetWindowPos().y;
//			vMax.x += ImGui::GetWindowPos().x;
//			vMax.y += ImGui::GetWindowPos().y;
//
//			ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
//		}*/
//
//		// set camera update flag
//		m_focused = ImGui::IsWindowFocused();
//		
//		ImGui::End();
//
//		ImGui::PopStyleVar();
//	}
//
//private:
//	uint32_t m_contentAreaWidth = 1920;
//	uint32_t m_contentAreatHeight = 1080;
//
//	Vector2f m_windowCursorPos;
//
//	Rainy::FrameBuffer* m_frameBuffer;
//	Rainy::Shader* m_mainShader;
//
//	//Landscape* landscape;
//	NewLandscape* newLandscape;
//	Rainy::Box* box;
//	Rainy::PerspectiveCamera* m_perspCamera;
//	
//	Rainy::Shader* m_pointsShader;
//	std::vector<Vector3f> m_intersectionPoints;
//	VertexBuffer* m_interPointsBuffer;
//	VertexArray* m_interPointsVertexArray;
//
//	bool m_focused;
//
//	/*BasicTool* current_tool = new SculptTool(newLandscape, 0.1f);
//	Brush* current_brush = new Brush(1, 0.5f, smoothFalloffFunction);*/
//};
