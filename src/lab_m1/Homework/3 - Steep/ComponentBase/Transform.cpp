#include "lab_m1/Homework/3 - Steep/ComponentBase/Transform.h"

#include <iostream>
#include <stack>

using namespace std;
using namespace transform;
using namespace component;


Transform::Transform()
{
    this->localPosition = glm::vec3();
    this->worldPosition = glm::vec3();

    this->localScale = glm::vec3(1.0f);
    this->worldScale = glm::vec3(1.0f);

    this->localRotation = glm::vec3(0.0f);
    this->worldRotation = glm::vec3(0.0f);

    this->parent = NULL;
    this->modelMatrix = glm::mat4(1.0f);

    this->up = glm::vec3_up;
    this->right = glm::vec3_right;
    this->forward = glm::vec3_forward;

    this->tag = "Default";

    this->manualRotation = false;
    this->manualRotationMatrix = glm::mat4(1.0f);
}

Transform::Transform(Transform* parent, std::string tag, bool manualRotation) : Transform()
{
    this->parent = parent;
    this->parent->AddChild(this);
    this->tag = tag;
    this->manualRotation = manualRotation;
}


Transform::~Transform()
{
}

void Transform::Update()
{
    UpdateWorldCoordinates();
    ComputeModelMatrix();
}

void Transform::AddChild(Transform* newChild)
{
    this->children.push_back(newChild);
    newChild->parent = this;
}

void Transform::RemoveChild(Transform* child)
{
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == child) {
            children.erase(it);
            return;
        }
    }
}

Transform* Transform::GetChild(unsigned int index)
{
    index = min((int)index, this->GetChildCount() - 1);
    return this->children[index];
}

int Transform::GetChildCount()
{
    return (int)this->children.size();
}

void Transform::AddComponent(Component* component)
{
    components.push_back(component);
}

Component* Transform::GetComponentByIndex(unsigned int index)
{
    index = min((int)index, this->GetComponentCount() - 1);
    return this->components[index];
}

int Transform::GetComponentCount()
{
    return (int)this->components.size();
}

Transform* Transform::GetTransformByTag(std::string tag)
{
    // Get a reference to the hierarchy
    Transform* currentTransform = this;
    while (currentTransform->parent != NULL) {
        currentTransform = currentTransform->parent;
    }

    // Transform recursion into iterative with a stack
    stack<Transform*> transformStack;
    transformStack.push(currentTransform);

    // Go through all transforms in the scene
    while (!transformStack.empty()) {
        // Update the root
        currentTransform = transformStack.top();
        transformStack.pop();

        // Add all of root's children to the stack
        for (int i = 0; i < currentTransform->GetChildCount(); ++i) {
            transformStack.push(currentTransform->GetChild(i));
        }

        // Check if we've found the transform
        if (tag == currentTransform->GetTag()) {
            return currentTransform;
        }
    }

    // We have NOT found any transforms with the given tag
    return NULL;
}

glm::mat4 Transform::GetTranslationMatrix(glm::vec3 translate)
{
    return glm::transpose(glm::mat4(
        1.0f, 0.0f, 0.0f, translate.x,
        0.0f, 1.0f, 0.0f, translate.y,
        0.0f, 0.0f, 1.0f, translate.z,
        0.0f, 0.0f, 0.0f,        1.0f
    ));
}

glm::mat4 Transform::GetScalingMatrix(glm::vec3 scale)
{
    return glm::transpose(glm::mat4(
        scale.x,    0.0f,    0.0f, 0.0f,
           0.0f, scale.y,    0.0f, 0.0f,
           0.0f,    0.0f, scale.z, 0.0f,
           0.0f,    0.0f,    0.0f, 1.0f
    ));
}

glm::mat4 Transform::GetRotationMatrix(glm::vec3 eulerAngle)
{
    return GetRotationMatrixOz(eulerAngle.z) * GetRotationMatrixOy(eulerAngle.y) * GetRotationMatrixOx(eulerAngle.x);
}

glm::mat4 Transform::GetRotationMatrixOx(float radians)
{
    return glm::transpose(glm::mat4(
        1.0f,              0.0f,               0.0f, 0.0f,
        0.0f, glm::cos(radians),  glm::sin(radians), 0.0f,
        0.0f,-glm::sin(radians),  glm::cos(radians), 0.0f,
        0.0f,              0.0f,               0.0f, 1.0f
    ));
}

glm::mat4 Transform::GetRotationMatrixOy(float radians)
{
    return glm::transpose(glm::mat4(
        glm::cos(radians), 0.0f,  glm::sin(radians), 0.0f,
                     0.0f, 1.0f,               0.0f, 0.0f,
       -glm::sin(radians), 0.0f,  glm::cos(radians), 0.0f,
                     0.0f, 0.0f,               0.0f, 1.0f
    ));
}

glm::mat4 Transform::GetRotationMatrixOz(float radians)
{
    return glm::transpose(glm::mat4(
        glm::cos(radians),  glm::sin(radians), 0.0f, 0.0f,
       -glm::sin(radians),  glm::cos(radians), 0.0f, 0.0f,
                     0.0f,               0.0f, 1.0f, 0.0f,
                     0.0f,               0.0f, 0.0f, 1.0f
    ));
}

void Transform::Translate(glm::vec3 translate)
{
    this->localPosition += translate;
}

void Transform::Scale(glm::vec3 scale)
{
    this->localScale += scale;
}

void Transform::Rotate(glm::vec3 rotate)
{
    this->localRotation += rotate;
    ComputeDirectionVectors();
}

glm::vec3 Transform::GetLocalPosition()
{
    return this->localPosition;
}

glm::vec3 Transform::GetWorldPosition()
{
    return glm::vec3(GetModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::vec3 Transform::GetLocalScale()
{
    return this->localScale;
}

glm::vec3 Transform::GetWorldScale()
{
    return this->worldScale;
}

glm::vec3 Transform::GetLocalRotation()
{
    return this->localRotation;
}

void Transform::SetLocalPosition(glm::vec3 translate)
{
    this->localPosition = translate;
}

void Transform::SetScale(glm::vec3 scale)
{
    this->localScale = scale;
}

void Transform::SetLocalRotation(glm::vec3 eulerAngle)
{
    this->localRotation = eulerAngle;
    ComputeDirectionVectors();
}

void Transform::ComputeDirectionVectors()
{
    // Reset directions
    up = glm::vec3_up;
    right = glm::vec3_right;
    forward = glm::vec3_forward;

    // Ox Rotation
    forward = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), -this->localRotation.x, right) * glm::vec4(forward, 1)
    ));

    up = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), -this->localRotation.x, right) * glm::vec4(up, 1)
    ));

    // Oy Rotation
    forward = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), this->localRotation.y, up) * glm::vec4(forward, 1)
    ));

    //up = glm::normalize(glm::vec3(
    //    glm::rotate(glm::mat4(1.0f), -this->localRotation.y, glm::vec3(0, 1, 0)) * glm::vec4(up, 1)
    //));

    right = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), this->localRotation.y, up) * glm::vec4(right, 1)
    ));

    // Oz Rotation
    right = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), -this->localRotation.z, forward) * glm::vec4(right, 1)
    ));

    up = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), -this->localRotation.z, forward) * glm::vec4(up, 1)
    ));
}

void Transform::UpdateWorldCoordinates()
{
    // If this transform is at the top of the hierarchy, the local values are already absolute
    if (this->parent == NULL) {
        // Copy constructor for the vectors, just in case
        this->worldPosition = glm::vec3(this->localPosition);
        this->worldScale = glm::vec3(this->localScale);
        this->worldRotation = glm::vec3(this->localRotation);

        return;
    }

    // Otherwise, modify the parameters based on the parent's absolute coordinates
    this->worldPosition = glm::vec3(this->parent->worldPosition + this->localPosition);
    this->worldScale = glm::vec3(this->parent->worldScale.x * this->localScale.x, this->parent->worldScale.y * this->localScale.y, this->parent->worldScale.z * this->localScale.z);
    this->worldRotation = glm::vec3(this->parent->worldRotation + this->localRotation);
}

glm::mat4 Transform::GetModelMatrix() const
{
    return this->modelMatrix;
}

glm::mat4 Transform::ComputeModelMatrix()
{
    // Initialize the model matrix
    glm::mat4 newModelMatrix = glm::mat4(1.0f);

    if (parent != NULL) {
        newModelMatrix = parent->GetModelMatrix();
    }

    // Translate it
    newModelMatrix *= GetTranslationMatrix(this->localPosition);

    // Scale it
    newModelMatrix *= GetScalingMatrix(this->localScale);

    // Rotate it
    newModelMatrix *= manualRotation ? GetManualRotationMatrix() : GetRotationMatrix(this->localRotation);

    // Cache it
    modelMatrix = newModelMatrix;

    // Return the matrix
    return newModelMatrix;
}
