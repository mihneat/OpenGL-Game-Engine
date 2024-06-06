#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace component;
using namespace transform;

void Camera::Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
    transform->SetLocalPosition(position);
    transform->forward = glm::normalize(center - position);
    transform->right = glm::cross(transform->forward, up);
    transform->up = glm::cross(transform->right, transform->forward);
}

void Camera::MoveForward(float distance)
{
    glm::vec3 dir = glm::normalize(glm::vec3(transform->forward.x, 0, transform->forward.z));
    transform->Translate(dir * distance);
}

void Camera::TranslateForward(float distance)
{
    transform->Translate(transform->forward * distance);

}

void Camera::TranslateUpward(float distance)
{
    transform->Translate(glm::vec3_up * distance);

}

void Camera::TranslateRight(float distance)
{
    glm::vec3 dir = glm::normalize(glm::vec3(transform->right.x, 0, transform->right.z));
    transform->Translate(dir * distance);

}

void Camera::RotateFirstPerson_OX(float angle)
{
    transform->forward = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, transform->right) * glm::vec4(transform->forward, 1)
    ));

    transform->up = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, transform->right) * glm::vec4(transform->up, 1)
    ));
}

void Camera::RotateFirstPerson_OY(float angle)
{
    transform->forward = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(transform->forward, 1)
    ));

    transform->up = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(transform->up, 1)
    ));

    transform->right = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(transform->right, 1)
    ));

}

void Camera::RotateFirstPerson_OZ(float angle)
{
    transform->right = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, transform->forward) * glm::vec4(transform->right, 1)
    ));

    transform->up = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), angle, transform->forward) * glm::vec4(transform->up, 1)
    ));

}

void Camera::RotateThirdPerson_OX(float angle)
{
    transform->Translate(transform->forward * distanceToTarget);
    RotateFirstPerson_OX(angle);
    transform->Translate(-transform->forward * distanceToTarget);

}

void Camera::RotateThirdPerson_OY(float angle)
{
    transform->Translate(transform->forward * distanceToTarget);
    RotateFirstPerson_OY(angle);
    transform->Translate(-transform->forward * distanceToTarget);

}

void Camera::RotateThirdPerson_OZ(float angle)
{
    transform->Translate(transform->forward * distanceToTarget);
    RotateFirstPerson_OZ(angle);
    transform->Translate(-transform->forward * distanceToTarget);

}

void Camera::SetProjection(const float fov, const float aspectRatio)
{
    isProjection = true;
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 500.0f);
}

void Camera::SetOrtographic(const float width, const float height)
{
    isProjection = false;
    projectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, 0.01f, 5000.0f);
}

glm::mat4 Camera::GetViewMatrix()
{
    // Returns the view matrix
    return glm::lookAt(transform->GetLocalPosition(), transform->GetLocalPosition() + transform->forward, transform->up);
}

glm::vec3 Camera::GetTargetPosition()
{
    return transform->GetLocalPosition() + transform->forward * distanceToTarget;
}

bool Camera::IsLayerRendered(int layer) 
{
    return layers.find(layer) != layers.end(); 
}

void Camera::WindowResize(int width, int height)
{
    if (autoResize) {
        viewportWidthHeight = glm::vec2(width, height);

        if (isProjection)
        {
            SetProjection(60.0f, 1.0f * width / (1.0f * height));
        } else
        {
            SetOrtographic(width, height);
        }
    }
}
