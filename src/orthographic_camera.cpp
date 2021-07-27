
#include "camera/orthographic_camera.h"
#include "core/input.h"

namespace Rainy {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom,
                                           float top, float nearPoint, float farPoint, Vector3f position, Vector3f rotation)
	:	m_position(position),
		m_rotation(rotation),
		m_orthographic(CreateOrthographicMatrix(left, right, bottom, top, nearPoint, farPoint)),
		m_view(CreateViewMatrix(m_position, rotation.x, rotation.y))
	{}

	void OrthographicCamera::OnUpdate()
	{
		Vector3f prevPosition = m_position;
		Vector3f prevRotation = m_rotation;

		if (Input::IsKeyPressed(RN_KEY_W))
		{

		}
		if (Input::IsKeyPressed(RN_KEY_S))
		{

		}
		if (Input::IsKeyPressed(RN_KEY_D))
		{

		}
		if (Input::IsKeyPressed(RN_KEY_A))
		{

		}

		if(prevPosition != m_position || prevRotation != m_rotation)
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
	}

	Matrix4f OrthographicCamera::GetViewMatrix() const
	{
		return m_view;
	}

	Matrix4f OrthographicCamera::GetProjectionMatrix() const
	{
		return m_orthographic;
	}

	void OrthographicCamera::SetPosition(Vector3f position)
	{
		if (position != m_position)
		{
			m_position = position;
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
		}
	}

	void OrthographicCamera::SetRotation(Vector3f rotation)
	{
		if (rotation != m_rotation)
		{
			m_rotation = rotation;
			m_view = CreateViewMatrix(m_position, m_rotation.x, m_rotation.y);
		}
	}

	Vector3f OrthographicCamera::GetPosition() const
	{
		return m_position;
	}

	Vector3f OrthographicCamera::GetRotation() const
	{
		return m_rotation;
	}

}
