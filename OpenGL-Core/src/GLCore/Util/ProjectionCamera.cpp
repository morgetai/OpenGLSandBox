#include "ProjectionCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace
{
    static constexpr auto MAX_VERT_ANGLE = 89.0f;
}

using namespace GLCore::Utils;

ProjectionCamera::ProjectionCamera(float aspectRatio) : m_ProjectionMatrix(glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f)),
                                                        m_ViewMatrix(glm::lookAt(m_Position, {0.0f, 0.0f, 0.0f}, m_camera_up)),
                                                        m_ViewProjectionMatrix(m_ProjectionMatrix * m_ViewMatrix),
                                                        m_aspect_ratio(aspectRatio)
{
}

void ProjectionCamera::SetProjection(float fov)
{
    m_fov += fov;

    if (m_fov < 1.0f)
        m_fov = 1.0f;
    if (m_fov > 45.0f)
        m_fov = 45.0f;

    m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect_ratio, 0.1f, 100.0f);

    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void ProjectionCamera::SetDirection(float x_offset, float y_offset)
{
    m_yaw += x_offset;
    m_pitch += y_offset;

    m_yaw = fmodf(m_yaw, 360.0f);
    if (m_yaw < 0.0f)
        m_yaw += 360.0f;

    if (m_pitch > MAX_VERT_ANGLE)
        m_pitch = MAX_VERT_ANGLE;
    else if (m_pitch < -MAX_VERT_ANGLE)
        m_pitch = -MAX_VERT_ANGLE;

    m_camera_direction = {
        std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch)),
        std::sin(glm::radians(m_pitch)),
        std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch))};
    RecalculateView();
}

void ProjectionCamera::MoveUp(float move)
{
    m_Position += move * m_camera_direction;
    RecalculateView();
}

void ProjectionCamera::MoveDown(float move)
{
    m_Position -= move * m_camera_direction;
    RecalculateView();
}

void ProjectionCamera::MoveLeft(float move)
{
    m_Position -= move * glm::normalize(glm::cross(m_camera_direction, m_camera_up));
    RecalculateView();
}

void ProjectionCamera::MoveRight(float move)
{
    m_Position += move * glm::normalize(glm::cross(m_camera_direction, m_camera_up));
    RecalculateView();
}

void ProjectionCamera::RecalculateView()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_camera_direction, m_camera_up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void ProjectionCamera::SetAspectRation(float aspect_ratio)
{
    m_aspect_ratio = aspect_ratio;

    m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect_ratio, 0.1f, 100.0f);
}
