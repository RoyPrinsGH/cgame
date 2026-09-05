#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <cgame/graphics/render.hpp>

namespace cgame::graphics
{
    void render(render_backend& backend,
                const render_snapshot& snapshot,
                int fbWidth,
                int fbHeight)
    {
        if (fbWidth < 1 || fbHeight < 1)
            return;

        const camera& camera = snapshot.camera;

        const float aspectRatio =
            static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

        const glm::mat4 projection = glm::perspective(
            glm::radians(camera.fovY), aspectRatio, camera.nearPlane, camera.farPlane);

        const glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);

        backend.beginFrame(view, projection, fbWidth, fbHeight);

        shader_handle activeShader;

        for (const render_entry& entry : snapshot.entries)
        {
            if (entry.instances.empty())
                continue;

            backend.uploadInstances(entry.model, entry.instances);

            if (entry.shader.id != activeShader.id)
            {
                backend.activateShader(entry.shader);
                activeShader = entry.shader;
            }

            backend.draw(entry.model, static_cast<int>(entry.instances.size()));
        }

        if (activeShader.valid())
            backend.deactivateShader();

        backend.endFrame();
    }
}
