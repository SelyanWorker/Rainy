#pragma once

#include "render/camera.h"

// orthographic - ve.y strong word

namespace Rainy {

	class RAINY_API OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float nearPoint, float farPoint,
                           Vector3f position = { 0, 0, 0 }, Vector3f rotation = { 0, 0, 0 });

		~OrthographicCamera() = default;

		void OnUpdate() override;

		Matrix4f GetViewMatrix() const override;

		Matrix4f GetProjectionMatrix() const override;

		void SetPosition(Vector3f position) override;

		void SetRotation(Vector3f rotation) override;

		Vector3f GetPosition() const override;

		Vector3f GetRotation() const override;

	private:
		Vector3f m_position;
		Vector3f m_rotation;

		Matrix4f m_orthographic;
		Matrix4f m_view;
	};

}
