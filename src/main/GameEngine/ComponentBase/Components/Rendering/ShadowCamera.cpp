#include "ShadowCamera.h"

#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

#include <glm/gtc/quaternion.hpp>

#include <iostream>

using namespace std;
using namespace component;
using namespace transform;

// no-op
void ShadowCamera::UpdateFrustum() { }

glm::mat4 ShadowCamera::GetViewMatrix()
{
    return viewMatrix;
}

void ShadowCamera::FitOrthographicProjectionToCameras(const std::vector<Camera*>& cameras, glm::vec3 lightDirection)
{
    std::vector<glm::vec4> frustumCorners;
    for (Camera* camera : cameras)
    {
        const auto camInverseProjView = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
        for (int x = -1; x <= 1; x += 2)
        {
            for (int y = -1; y <= 1; y += 2)
            {
                for (int z = -1; z <= 1; z += 2)
                {
                    glm::vec4 cuboidCorner = glm::vec4(x, y, z, 1.0f);
    
                    cuboidCorner = camInverseProjView * cuboidCorner;
                    cuboidCorner /= cuboidCorner.w;

                    frustumCorners.push_back(cuboidCorner);
                }
            }
        }
    }

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : frustumCorners)
    {
        center += glm::vec3(v);
    }
    center /= frustumCorners.size();
    
    const auto lightView = glm::lookAt(
        center - lightDirection,
        center,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    viewMatrix = lightView;
    
    glm::vec3 minPoint = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxPoint = glm::vec3(std::numeric_limits<float>::lowest());
    for (const auto& corner : frustumCorners)
    {
        glm::vec4 viewCorner = lightView * corner;
        minPoint.x = glm::min(minPoint.x, viewCorner.x);
        minPoint.y = glm::min(minPoint.y, viewCorner.y);
        minPoint.z = glm::min(minPoint.z, viewCorner.z);
        
        maxPoint.x = glm::max(maxPoint.x, viewCorner.x);
        maxPoint.y = glm::max(maxPoint.y, viewCorner.y);
        maxPoint.z = glm::max(maxPoint.z, viewCorner.z);
    }

    // Expand the frustum on the Z axis
    static constexpr float zMult = 2.0f;
    if (minPoint.z < 0)
        minPoint.z *= zMult;
    else
        minPoint.z /= zMult;
    
    if (maxPoint.z < 0)
        maxPoint.z /= zMult;
    else
        maxPoint.z *= zMult;
   
    
    isPerspective = false;
    projectionMatrix = glm::ortho(minPoint.x, maxPoint.x, minPoint.y, maxPoint.y, minPoint.z, maxPoint.z);
}
