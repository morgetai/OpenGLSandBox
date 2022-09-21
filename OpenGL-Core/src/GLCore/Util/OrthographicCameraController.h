#pragma once

#include "Util/OrthographicCamera.h"
#include "Util/ProjectionCamera.h"
#include "Core/Timestep.h"
#include "Core/Timer.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"

#include <glm/glm.hpp>

namespace GLCore::Utils
{
	template <typename Camer_Type>
	class OrthographicCameraController : public Camer_Type
	{
		// template <typename... Args>
		// OrthographicCameraController(Args &&...args) { m_camera(std::forward<Args>(args)...); }
		void OnUpdate(Timestep ts) { static_cast<Camer_Type>(*this).OnUpdate(ts); };
		void OnEvent(Event &e) { static_cast<Camer_Type>(*this).OnEvent(e); };
		glm::mat4 GetViewProjMatrix() const { return static_cast<Camer_Type>(*this).GetViewProjMatrix(); };
		void SetZoomLevel(float level) const { return static_cast<Camer_Type>(*this).SetZoomLevel(level); };

	private:
		Camer_Type m_Camera;
	};

	template <>
	class OrthographicCameraController<OrthographicCamera>
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = true) : m_AspectRatio(aspectRatio),
																				m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
																				m_Rotation(rotation) {}

		void OnUpdate(Timestep ts)
		{
			if (Input::IsKeyPressed(HZ_KEY_A))
			{
				m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(HZ_KEY_D))
			{
				m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}

			if (Input::IsKeyPressed(HZ_KEY_W))
			{
				m_CameraPosition.x -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(HZ_KEY_S))
			{
				m_CameraPosition.x += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}

			if (m_Rotation)
			{
				if (Input::IsKeyPressed(HZ_KEY_Q))
					m_CameraRotation += m_CameraRotationSpeed * ts;
				if (Input::IsKeyPressed(HZ_KEY_E))
					m_CameraRotation -= m_CameraRotationSpeed * ts;

				if (m_CameraRotation > 180.0f)
					m_CameraRotation -= 360.0f;
				else if (m_CameraRotation <= -180.0f)
					m_CameraRotation += 360.0f;

				m_Camera.SetRotation(m_CameraRotation);
			}

			m_Camera.SetPosition(m_CameraPosition);

			m_CameraTranslationSpeed = m_ZoomLevel;
		}

		void OnEvent(Event &e)
		{
			EventDispatcher dispatcher(e);
			dispatcher.Dispatch<MouseScrolledEvent>(GLCORE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
			dispatcher.Dispatch<WindowResizeEvent>(GLCORE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
		}

		glm::mat4 GetViewProjMatrix() const
		{
			return m_Camera.GetViewProjectionMatrix();
		}

		void SetZoomLevel(float level) { m_ZoomLevel = level; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent &e)
		{
			m_ZoomLevel -= e.GetYOffset() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
			return false;
		}
		bool OnWindowResized(WindowResizeEvent &e)
		{
			m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
			return false;
		}

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
		float m_CameraRotation = 0.0f; // In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};

	template <>
	class OrthographicCameraController<ProjectionCamera>
	{
	public:
		OrthographicCameraController(float width, float height) : m_Camera(width / height), m_last_x(width / 2), m_last_y(height / 2)
		{
		}

		void OnEvent(Event &e)
		{
			EventDispatcher dispatcher(e);
			dispatcher.Dispatch<MouseScrolledEvent>(GLCORE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
			dispatcher.Dispatch<WindowResizeEvent>(GLCORE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
			dispatcher.Dispatch<MouseMovedEvent>(GLCORE_BIND_EVENT_FN(OrthographicCameraController::OnMouseMoved));
		}

		void OnUpdate(Timestep ts)
		{
			float move = ts * m_CameraTranslationSpeed;
			if (Input::IsKeyPressed(HZ_KEY_A))
			{
				m_Camera.MoveLeft(move);
			}
			else if (Input::IsKeyPressed(HZ_KEY_D))
			{
				m_Camera.MoveRight(move);
			}

			if (Input::IsKeyPressed(HZ_KEY_W))
			{
				m_Camera.MoveUp(move);
			}
			else if (Input::IsKeyPressed(HZ_KEY_S))
			{
				m_Camera.MoveDown(move);
			}
		}

		glm::mat4 GetViewProjMatrix() const
		{
			return m_Camera.GetViewProjectionMatrix();
		}

		void SetZoomLevel(float level) { m_ZoomLevel = level; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent &e)
		{
			m_ZoomLevel = e.GetYOffset() * 0.2f;
			m_Camera.SetProjection(m_ZoomLevel);
			return true;
		}
		bool OnWindowResized(WindowResizeEvent &e)
		{
			m_Camera.SetAspectRation(e.GetWidth() / e.GetHeight());
			return true;
		}
		bool OnMouseMoved(MouseMovedEvent &e)
		{
			m_Camera.SetDirection((e.GetX() - m_last_x) * 0.1f, (m_last_y - e.GetY()) * 0.1f);

			m_last_x = e.GetX();
			m_last_y = e.GetY();

			return true;
		}

		float m_CameraTranslationSpeed{5.0f};
		float m_ZoomLevel{1.0f};

		ProjectionCamera m_Camera;
		float m_last_x;
		float m_last_y;
	};
}