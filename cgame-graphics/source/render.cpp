#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <variant>

#include <cgame/graphics/render.hpp>

namespace cgame::graphics
{
    void render(render_backend& backend, const render_snapshot& snapshot)
    {
        const int fbWidth = snapshot.framebufferWidth;
        const int fbHeight = snapshot.framebufferHeight;

        if (fbWidth < 1 || fbHeight < 1)
            return;

        const camera& camera = snapshot.camera;

        const float aspectRatio =
            static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

        const glm::mat4 projection = glm::perspective(
            glm::radians(camera.fovY), aspectRatio, camera.nearPlane, camera.farPlane);

        const glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);

        backend.beginFrame(
            view, projection, fbWidth, fbHeight, snapshot.clearColour);

        shader_handle activeShader;

        for (const render_entry& entry : snapshot.entries)
        {
            if (entry.shader != activeShader)
            {
                backend.activateShader(entry.shader);
                activeShader = entry.shader;
            }

            if (auto* instances =
                    std::get_if<std::span<const glm::mat4>>(&entry.transform))
            {
                if (instances->empty())
                    continue;

                backend.uploadInstances(entry.model, *instances);
                backend.draw(entry.model,
                             static_cast<int>(instances->size()));
            }
            else
            {
                backend.draw(entry.model, std::get<glm::mat4>(entry.transform));
            }
        }

        if (activeShader.valid())
            backend.deactivateShader();

        backend.endFrame();
    }
}
