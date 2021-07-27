#pragma once

#include "core/core.h"
#include "render/camera.h"

namespace Rainy {

	class RAINY_API PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float aspectRation, float fov, float nearPoint, float farPoint,
			Vector3f position = { 0, 0, 0 }, Vector3f rotation = { 0, 0, 0 },
			float roationSpeed = 1, float moveSpeed = 1);

		~PerspectiveCamera() = default;

		void OnUpdate() override;

		Matrix4f GetViewMatrix() const override;

		Matrix4f GetProjectionMatrix() const override;

		void SetPosition(Vector3f position) override;

		void SetRotation(Vector3f rotation) override;

		Vector3f GetPosition() const override;

		Vector3f GetRotation() const override;

		void Freeze();

		void UnFreeze();

		bool IsFreezed() const;

		void RecreateProjectionMatrix(float aspectRation, float fov, float nearPoint, float farPoint);

	private:
		Vector3f GetMoveIncrement(Vector3f direction);

	public:
		float m_rotationSpeed;
		float m_moveSpeed;

	private:
		Matrix4f m_view;
		Matrix4f m_projection;

		float m_near;
		float m_far;
		float m_ar;
		float m_fov;

		Vector3f m_position;
		Vector3f m_rotation;

		bool m_freeze;
	};

}

