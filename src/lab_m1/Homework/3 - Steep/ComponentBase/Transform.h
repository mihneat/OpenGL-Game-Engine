#pragma once

#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "utils/glm_utils.h"
#include <vector>

namespace component {
    class Component;
}

namespace transform
{
    class Transform
    {
    public:
        Transform();
        Transform(Transform *parent, std::string tag = "Default", bool manualRotation = false);
        ~Transform();

        void Update();

        void AddChild(Transform* newChild);
        void RemoveChild(Transform* child);
        /* Safe function - if index is greater than childCount - 1, returns the last child. */
        Transform* GetChild(unsigned int index);
        int GetChildCount();

        void AddComponent(component::Component* component);
        template <class T> T* GetComponent()
        {
            const char* desiredType = typeid(T).name();
            for (int i = 0; i < (int)components.size(); ++i) {
                if (!strcmp(desiredType, typeid(*components[i]).name())) {
                    // Found the component, return it
                    return (T*)components[i];
                }
            }

            // Didn't find the component, return null
            return NULL;
        }

        template <class T> std::vector<T*> GetComponents()
        {
            std::vector<T*> componentVec;

            const char* desiredType = typeid(T).name();
            for (int i = 0; i < (int)components.size(); ++i) {
                if (!strcmp(desiredType, typeid(*components[i]).name())) {
                    // Found the component, return it
                    componentVec.push_back((T*)components[i]);
                }
            }

            // Return the found components
            return componentVec;
        }

        component::Component* GetComponentByIndex(unsigned int index);
        int GetComponentCount();

        std::string GetTag() { return tag; }

        // TODO: Make function static
        Transform* GetTransformByTag(std::string tag);

        static glm::mat4 GetTranslationMatrix(glm::vec3 translate);
        static glm::mat4 GetScalingMatrix(glm::vec3 scale);
        static glm::mat4 GetRotationMatrix(glm::vec3 eulerAngle);
        static glm::mat4 GetRotationMatrixOx(float radians);
        static glm::mat4 GetRotationMatrixOy(float radians);
        static glm::mat4 GetRotationMatrixOz(float radians);

        void Translate(glm::vec3 translate);
        void Scale(glm::vec3 scale);
        void Rotate(glm::vec3 eulerAngle);

        glm::vec3 GetLocalPosition();
        glm::vec3 GetWorldPosition();
        glm::vec3 GetLocalScale();
        glm::vec3 GetWorldScale();
        glm::vec3 GetLocalRotation();

        glm::mat4 GetManualRotationMatrix() { return manualRotationMatrix; }
        void SetManualRotationMatrix(glm::mat4 rotationMatrix) { manualRotationMatrix = rotationMatrix; }

        void SetLocalPosition(glm::vec3 translate);
        void SetScale(glm::vec3 scale);
        void SetLocalRotation(glm::vec3 eulerAngle);

        void ComputeDirectionVectors();

        void UpdateWorldCoordinates();

        glm::mat4 GetModelMatrix() const;
        glm::mat4 ComputeModelMatrix();

        Transform* parent;

        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;

    protected:
        glm::vec3 localPosition;
        glm::vec3 worldPosition;

        glm::vec3 localScale;
        glm::vec3 worldScale;

        glm::vec3 localRotation;
        glm::vec3 worldRotation;

        std::vector<Transform*> children;

        std::vector<component::Component *> components;

        glm::mat4 modelMatrix;

        bool manualRotation;
        glm::mat4 manualRotationMatrix;

        std::string tag;
    };
}   // namespace transform2D
