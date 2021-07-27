#include "camera/perspective_camera.h"
#include "application/application.h"

namespace Rainy {

	/*Vector3f PerspectiveCamera::GetMoveIncrement(Vector3f direction)
	{
		using std::sin;
		using std::cos;

		float xRotRad = ToRadian(m_rotation.x);
		float yRotRad = ToRadian(m_rotation.y);

		Vector3f result = {
			direction.z * sin(yRotRad) - direction.x * cos(yRotRad),
			direction.z * sin(xRotRad) + direction.y,
			direction.z * cos(yRotRad) + direction.x * sin(yRotRad)
		};

		return result;
	}*/

	PerspectiveCamera::PerspectiveCamera(float aspectRation, float fov, float nearPoint, float farPoint,
		Vector3f position, Vector3f rotation, float rotationSpeed, float moveSpeed)
	:	m_position(position),
		m_rotation(rotation),
		m_rotationSpeed(rotationSpeed),
		m_moveSpeed(moveSpeed),
		m_freeze(false)
	{
		m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
		m_projection = CreatePerspectiveMatrix(aspectRation, fov, nearPoint, farPoint);
	}

	void PerspectiveCamera::OnUpdate()
	{
		static std::pair<float, float> prevCursorPosition = Input::GetCursorPosition();
		std::pair<float, float> cursorPosition = Input::GetCursorPosition();

		if (m_freeze)
		{
			prevCursorPosition = cursorPosition;
			return;
		}

		float frameTime = Application::Get()->GetFrameTime();

		Vector3f prevPosition = m_position;
		Vector3f prevRotation = m_rotation;

		m_rotation.x += (cursorPosition.second - prevCursorPosition.second) * m_rotationSpeed * frameTime;
		m_rotation.y -= (cursorPosition.first - prevCursorPosition.first) * m_rotationSpeed * frameTime;
		prevCursorPosition = cursorPosition;

		if (m_rotation.x > 89)
			m_rotation.x = 89;
		else if (m_rotation.x < -89)
			m_rotation.x = -89;

		Vector3f direction = GetLookDirection(m_rotation);
		float yRotRad = ToRadian(m_rotation.y);
		float halfRad = PI / 2.f;
		Vector3f right = { std::sin(yRotRad - halfRad), 0, std::cos(yRotRad - halfRad) };
		Vector3f up = right.cross(direction);

		if (Input::IsKeyPressed(RN_KEY_W))
		{
			m_position += direction * (-m_moveSpeed);
		}
		if (Input::IsKeyPressed(RN_KEY_S))
		{
			m_position += direction * m_moveSpeed;
		}
		if (Input::IsKeyPressed(RN_KEY_D))
		{
			m_position += right * (-m_moveSpeed);
		}
		if (Input::IsKeyPressed(RN_KEY_A))
		{
			m_position += right * m_moveSpeed;
		}
		if (Input::IsKeyPressed(RN_KEY_R))
		{
			direction = { 0, 0, 0 };
			m_position = direction;
			m_rotation = direction;
		}
		if (Input::IsKeyPressed(RN_KEY_Q))
		{
			m_position += up * m_moveSpeed;
		}
		if (Input::IsKeyPressed(RN_KEY_E))
		{
			m_position += up * (-m_moveSpeed);
		}

		if (prevPosition != m_position || prevRotation != m_rotation)
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
			//m_view = CreateLookAtMatrix(m_position, direction + m_position, up);
	}

	Matrix4f PerspectiveCamera::GetViewMatrix() const
	{
		return m_view;
	}

	Matrix4f PerspectiveCamera::GetProjectionMatrix() const
	{
		return m_projection;
	}

	void PerspectiveCamera::SetPosition(Vector3f position)
	{
		if (position != m_position)
		{
			m_position = position;
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
		}
	}

	void PerspectiveCamera::SetRotation(Vector3f rotation)
	{
		if (rotation != m_rotation)
		{
			m_rotation = rotation;
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
		}
	}

	Vector3f PerspectiveCamera::GetPosition() const
	{
		return m_position;
	}

	Vector3f PerspectiveCamera::GetRotation() const
	{
		return m_rotation;
	}

	void PerspectiveCamera::Freeze()
	{
		m_freeze = true;
	}

	void PerspectiveCamera::UnFreeze()
	{
		m_freeze = false;
	}

	bool PerspectiveCamera::IsFreezed() const
	{
		return m_freeze;
	}

	void PerspectiveCamera::RecreateProjectionMatrix(float aspectRation, float fov,
		float nearPoint, float farPoint)
	{
		m_projection = CreatePerspectiveMatrix(aspectRation, fov, nearPoint, farPoint);
	}

}
