#pragma once

#include <cgame/assets/pak.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "world/gamestate.hpp"

namespace engine
{
    struct gpu_primitive
    {
        unsigned int vaoId;
        unsigned int vertexVboId;
        unsigned int baseColorTextureId = 0;
        int vertexCount;
    };

    struct gpu_model
    {
        std::vector<gpu_primitive> primitives;
        unsigned int instanceVboId;
    };

    class renderer
    {
      public:
        renderer(cgame::assets::pak* pakPtr);

        void draw(GLFWwindow* window,
                  const world::game_state& gameState,
                  const camera& camera) const;

        void toggleDebugMode()
        {
            m_debugModeEnabled = !m_debugModeEnabled;
        }

      private:
        const gpu_model uploadModel(const cgame::assets::virtual_asset_path& path,
                                    int max_instances) const;
        cgame::assets::pak* m_pakPtr;
        bool m_debugModeEnabled = false;
        void drawShip(world::ship ship);
        unsigned int m_defaultShader;
        int m_defaultShaderViewMatrixLocation;
        int m_defaultShaderProjectionMatrixLocation;
        int m_defaultShaderBaseColorTextureLocation;
        gpu_model m_shipModel;
    };
}