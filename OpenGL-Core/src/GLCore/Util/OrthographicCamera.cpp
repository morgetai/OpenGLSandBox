
#include "OrthographicCamera.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"
#include "Events/Event.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GLCore::Utils
{
	OrthographicCamera::OrthographicCamera(float AspectRatio)
		: m_ProjectionMatrix(glm::ortho(-AspectRatio * m_ZoomLevel, AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel, -1.0f, 1.0f)), m_ViewMatrix(1.0f), m_AspectRatio(AspectRatio)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetPosition(const glm::vec3 &position)
	{
		m_Position = position;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top,  -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
							  glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::OnUpdate(GLCore::Timestep ts)
	{
		if (Input::IsKeyPressed(Key::A))
		{
			m_Position.x -= cos(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
			m_Position.y -= sin(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			m_Position.x += cos(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
			m_Position.y += sin(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(Key::W))
		{
			m_Position.x -= sin(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
			m_Position.y += cos(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::S))
		{
			m_Position.x += sin(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
			m_Position.y -= cos(glm::radians(m_Rotation)) * m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(Key::Q))
			m_Rotation += m_CameraRotationSpeed * ts;
		if (Input::IsKeyPressed(Key::E))
			m_Rotation -= m_CameraRotationSpeed * ts;

		if (m_Rotation > 180.0f)
			m_Rotation -= 360.0f;
		else if (m_Rotation <= -180.0f)
			m_Rotation += 360.0f;

		m_CameraTranslationSpeed = m_ZoomLevel;

		RecalculateViewMatrix();
	}
	void OrthographicCamera::OnEvent(GLCore::Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCamera::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCamera::OnWindowResized));
	}

	bool OrthographicCamera::OnMouseScrolled(MouseScrolledEvent &e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthographicCamera::OnWindowResized(WindowResizeEvent &e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	void OrthographicCamera::SetRotation(float x, float y)
	{
		glm::mat4 transform;
		glm::rotate(transform, glm::radians(y * 100) ,glm::vec3(1.0f, 0.0f, 0.0f));
		glm::rotate(transform, glm::radians(x * 100) ,glm::vec3(0.0f, 1.0f, 0.0f));

		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), m_Position) * transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		//glm::rotate(glm::mat4(1.f), glm::radians(y) ,glm::vec3(1.0f, 0.0f, 0.0f));
	}
}