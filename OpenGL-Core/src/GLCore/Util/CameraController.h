#pragma once

#include "Core/Timestep.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

#include "Core/Input.h"
#include "EditorCamera.h"
#include "OrthographicCamera.h"

#include <glm/glm.hpp>
#include <variant>
#include <functional>
#include <array>
#include <type_traits>

class CameraConroller
{
public:
	using Cameras = std::variant<GLCore::Utils::OrthographicCamera, EditorCamera>;
	using Container = std::array<Cameras, 2>;
	using iterator = typename Container::iterator;

	CameraConroller(GLCore::Utils::OrthographicCamera&& camera1, EditorCamera &&camera2);

	void OnUpdate(GLCore::Timestep ts);

	void OnEvent(GLCore::Event &e);

	glm::mat4 GetViewProjMatrix();

	glm::vec3 GetPosition();

	void SwitchCamera();

private:
	Container m_cameras;
	iterator m_current_camera;
};