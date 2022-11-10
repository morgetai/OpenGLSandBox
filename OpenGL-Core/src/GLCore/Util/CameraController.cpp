#include "CameraController.h"

CameraConroller::CameraConroller(GLCore::Utils::OrthographicCamera &&camera1, EditorCamera &&camera2) : m_cameras{std::move(camera1), std::move(camera2)}
{
    m_current_camera = m_cameras.begin();
}

void CameraConroller::OnUpdate(GLCore::Timestep ts)
{
    for (auto &camera : m_cameras)
    {
        std::visit([&ts](auto &arg)
                   { arg.OnUpdate(ts); },
                   camera);
    }
}

void CameraConroller::OnEvent(GLCore::Event &e)
{
    for (auto &camera : m_cameras)
    {
        std::visit([&e](auto &arg)
                   { arg.OnEvent(e); },
                   camera);
    }
}

glm::mat4 CameraConroller::GetViewProjMatrix()
{
    if (m_current_camera == m_cameras.begin())
    {
        return std::get<0>(*m_current_camera).GetViewProjection();
    }

    return std::get<1>(*m_current_camera).GetViewProjection();
}

glm::vec3 CameraConroller::GetPosition()
{
    if (m_current_camera == m_cameras.begin())
    {
        return std::get<0>(*m_current_camera).GetPosition();
    }

    return std::get<1>(*m_current_camera).GetPosition();
}

void CameraConroller::SwitchCamera()
{
    auto pos = GetPosition();

    if (m_current_camera == m_cameras.begin())
    {
        ++m_current_camera;
    }
    else
    {
        --m_current_camera;
    }
}