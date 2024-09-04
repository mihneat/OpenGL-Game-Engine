#include "main/GameEngine/ComponentBase/Transform.h"

#include <iostream>
#include <stack>

using namespace std;
using namespace transform;
using namespace component;


Transform::Transform()
{
    this->localPosition = glm::vec3();
    this->localScale = glm::vec3(1.0f);
    this->localRotation = glm::vec3();

    this->parent = nullptr;
    this->modelMatrix = glm::mat4(1.0f);
    this->rotationMatrix = glm::mat4(1.0f);

    this->up = glm::vec3_up;
    this->right = glm::vec3_right;
    this->forward = glm::vec3_forward;

    this->tag = "New Transform";
    
    UpdateChildren(true);
}

Transform::Transform(Transform* parent, const std::string& name, std::string tag) : Transform()
{
    parent->AddChild(this);
    this->tag = tag;
    this->name = name;
}


Transform::~Transform()
{
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

Transform* Transform::GetChild(unsigned int index) const
{
    index = min((int)index, this->GetChildCount() - 1);
    return this->children[index];
}

int Transform::GetChildCount() const
{
    return (int)this->children.size();
}

void Transform::AddComponent(Component* component)
{
    components.push_back(component);
}

void Transform::RemoveComponent(Component* component)
{
    for (auto it = components.begin(); it != components.end(); ++it) {
        if (*it == component) {
            components.erase(it);
            delete component;
            return;
        }
    }
}

Component* Transform::GetComponentByIndex(unsigned int index) const
{
    index = min((int)index, this->GetComponentCount() - 1);
    return this->components[index];
}

int Transform::GetComponentCount() const
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

    // Transform recursion into iteration with a stack
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

Transform* Transform::Instantiate(Transform* transform)
{
    for (int i = 0; i < transform->GetComponentCount(); ++i) {
        Component* curr = transform->GetComponentByIndex(i);

        // Check if the component is enabled
        if (!curr->IsActive()) {
            continue;
        }

        // Call Awake function of component
        curr->hasAwakeActivated = true;
        curr->Awake();
    }

    return transform;
}

void Transform::Destroy(Transform* transform)
{
    transform->markedForDeletion = true;
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

glm::mat4 Transform::GetRotationMatrix() const
{
    return rotationMatrix;
}

void Transform::Translate(glm::vec3 translate)
{
    this->localPosition += translate;
    UpdateChildren();
}

void Transform::Scale(glm::vec3 scale)
{
    this->localScale += scale;
    UpdateChildren();
}

void Transform::Rotate(glm::vec3 rotate)
{
    this->localRotation += rotate;
    UpdateChildren(true);
}

void Transform::UpdateChildren(bool updateRotation)
{
    // Transform recursion into iterative with a stack
    stack<Transform*> transformStack;
    transformStack.push(this);

    // Go through all transforms in the scene
    while (!transformStack.empty()) {
        // Update the root
        Transform* currentTransform = transformStack.top();
        transformStack.pop();

        // Add all of root's children to the stack
        for (int i = 0; i < currentTransform->GetChildCount(); ++i) {
            transformStack.push(currentTransform->GetChild(i));
        }

        if (updateRotation) currentTransform->ComputeDirectionVectors();
        currentTransform->ComputeModelMatrix();
    }
}

glm::vec3 Transform::GetLocalPosition() const
{
    return this->localPosition;
}

glm::vec3 Transform::GetWorldPosition() const
{
    return glm::vec3(GetModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::vec3 Transform::GetLocalScale() const
{
    return this->localScale;
}

glm::vec3 Transform::GetLocalRotation() const
{
    return this->localRotation;
}

void Transform::SetLocalPosition(const glm::vec3& translate)
{
    this->localPosition = translate;
    UpdateChildren();
}

void Transform::SetScale(const glm::vec3& scale)
{
    this->localScale = scale;
    UpdateChildren();
}

void Transform::SetLocalRotation(const glm::vec3& eulerAngle)
{
    this->localRotation = eulerAngle;
    UpdateChildren(true);
}

void Transform::ComputeDirectionVectors()
{
    // The root object cannot rotate
    if (parent == nullptr)
        return;
    
    // Initialize the rotation matrix
    glm::mat4 newRotationMatrix = parent->GetRotationMatrix();
    
    newRotationMatrix = glm::rotate(newRotationMatrix, -this->localRotation.x, glm::vec3(1.0, 0.0, 0.0));
    newRotationMatrix = glm::rotate(newRotationMatrix, this->localRotation.y, glm::vec3(0.0, 1.0, 0.0));
    newRotationMatrix = glm::rotate(newRotationMatrix, -this->localRotation.z, glm::vec3(0.0, 0.0, 1.0));

    // Cache it
    rotationMatrix = newRotationMatrix;
    
    // Rotate the vectors
    forward = normalize(glm::vec3(rotationMatrix * glm::vec4(glm::vec3_forward, 1.0f)));
    right = normalize(glm::vec3(rotationMatrix * glm::vec4(glm::vec3_right, 1.0f)));
    up = normalize(glm::vec3(rotationMatrix * glm::vec4(glm::vec3_up, 1.0f)));
    
    // // Ox Rotation
    // forward = normalize(eulerAngles(rotate(glm::quat(forward), -this->localRotation.x, right)));
    // up = normalize(eulerAngles(rotate(glm::quat(up), -this->localRotation.x, right)));
    //
    // // Oy Rotation
    // forward = normalize(eulerAngles(rotate(glm::quat(forward), -this->localRotation.y, up)));
    // right = normalize(eulerAngles(rotate(glm::quat(right), -this->localRotation.y, up)));
    //
    // // Oz Rotation
    // right = normalize(eulerAngles(rotate(glm::quat(right), -this->localRotation.z, forward)));
    // up = normalize(eulerAngles(rotate(glm::quat(up), -this->localRotation.z, forward)));
    
    // // Ox Rotation
    // forward = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), -this->localRotation.x, right) * glm::vec4(forward, 1)
    // ));
    //
    // up = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), -this->localRotation.x, right) * glm::vec4(up, 1)
    // ));
    //
    // // Oy Rotation
    // forward = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), this->localRotation.y, up) * glm::vec4(forward, 1)
    // ));
    //
    // right = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), this->localRotation.y, up) * glm::vec4(right, 1)
    // ));
    //
    // // Oz Rotation
    // right = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), -this->localRotation.z, forward) * glm::vec4(right, 1)
    // ));
    //
    // up = glm::normalize(glm::vec3(
    //     glm::rotate(glm::mat4(1.0f), -this->localRotation.z, forward) * glm::vec4(up, 1)
    // ));
}

glm::mat4 Transform::GetModelMatrix() const
{
    return this->modelMatrix;
}

inline void Transform::ComputeModelMatrix()
{
    // Initialize the model matrix
    glm::mat4 newModelMatrix = glm::mat4(1.0f);

    if (parent != NULL) {
        newModelMatrix = parent->GetModelMatrix();
    }

    newModelMatrix = glm::translate(newModelMatrix, this->localPosition);
    newModelMatrix = glm::rotate(newModelMatrix, -this->localRotation.x, glm::vec3(1.0, 0.0, 0.0));
    newModelMatrix = glm::rotate(newModelMatrix, this->localRotation.y, glm::vec3(0.0, 1.0, 0.0));
    newModelMatrix = glm::rotate(newModelMatrix, -this->localRotation.z, glm::vec3(0.0, 0.0, 1.0));
    newModelMatrix = glm::scale(newModelMatrix, this->localScale);

    // Cache it
    modelMatrix = newModelMatrix;
}
