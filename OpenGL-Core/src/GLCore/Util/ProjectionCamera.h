#pragma once

#include <glm/glm.hpp>

namespace GLCore::Utils
{
    class ProjectionCamera
    {
    public:
        ProjectionCamera(float aspectRatio);

        void SetProjection(float fov);

        void MoveUp(float move);
        void MoveDown(float move);
        void MoveLeft(float move);
        void MoveRight(float move);

        glm::vec3 GetPosition() const { return m_Position;};

        void SetDirection(float x_offset, float y_offset);
        void SetAspectRation(float aspect_ratio);

        const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    private:
        void RecalculateView();

        glm::vec3 m_camera_up{0.0f, 1.0f, 0.0f};
        glm::vec3 m_camera_direction{0.0f, 0.0f, 0.0f};
        glm::vec3 m_Position = {0.0f, 0.0f, 3.0f};

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        float m_aspect_ratio{};
        float m_yaw{-90.0f};//x
        float m_pitch{0.0f};//y
        float m_fov{45.0f};//y
    };
}