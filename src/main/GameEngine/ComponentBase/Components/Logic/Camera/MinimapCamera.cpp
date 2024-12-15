#include "MinimapCamera.h"

void component::MinimapCamera::Start()
{
    camera = transform->GetComponent<Camera>();

    camera->Set(glm::vec3(0, 1000, 0), glm::vec3(0, 0, 1), -glm::vec3_forward);
    camera->SetOrthographic(1000, 1000);

    camera->SetRenderLayers({ static_cast<int>(MeshRenderer::LayerEnum::Minimap) });
}

void component::MinimapCamera::WindowResize(int width, int height)
{
    if (camera == nullptr)
    {
        camera = transform->GetComponent<Camera>();
        if (camera == nullptr)
            return;
    }
    
    camera->viewportBottomLeft = glm::vec2(width - 200 - 20, 20);
    camera->viewportWidthHeight = glm::vec2(200, 200);
}
