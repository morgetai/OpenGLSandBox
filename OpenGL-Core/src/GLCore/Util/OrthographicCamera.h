#pragma once
#include "Core/Timestep.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include <glm/glm.hpp>

namespace GLCore::Utils
{

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float AspectRatio);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3 &GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3 &position);
		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation)
		{
			m_Rotation = rotation;
			RecalculateViewMatrix();
		}
		void SetRotation(float x, float y);

		const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 &GetViewProjection() const { return m_ViewProjectionMatrix; }

		void OnUpdate(GLCore::Timestep ts);
		void OnEvent(GLCore::Event &e);

	private:
		void RecalculateViewMatrix();
		bool OnMouseScrolled(MouseScrolledEvent &e);
		bool OnWindowResized(WindowResizeEvent &e);

	private:
		float m_ZoomLevel = 1.0f;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.0f;

		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 180.0f;
		float m_AspectRatio;
	};

}
