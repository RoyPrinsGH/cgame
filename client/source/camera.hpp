#pragma once
#include "glm/vec3.hpp"

namespace engine
{
    class camera
    {
    public:
        inline const glm::vec3 &getPosition() const { return m_position; };
        inline const glm::vec3 &getTarget() const { return m_target; };
        inline const void setPosition(glm::vec3 position) { m_position = position; };
        inline const void setTarget(glm::vec3 target) { m_target = target; };

    private:
        glm::vec3 m_position;
        glm::vec3 m_target;
    };
}