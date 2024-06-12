#pragma once

#include "main/GameEngine/ComponentBase/Component.h"
#include "utils/glm_utils.h"
#include <vector>

#include "main/GameEngine/Systems/SceneManager.h"

class GUIManager;

namespace component {
    class Component;
}

namespace m1
{
    class GameEngine;
}

namespace transform
{
    class Transform
    {
    public:
        Transform();
        Transform(Transform *parent, const std::string& name = "New Transform", std::string tag = "Default", bool manualRotation = false);
        ~Transform();

        void Update();

        void AddChild(Transform* newChild);
        void RemoveChild(Transform* child);
        /* Safe function - if index is greater than childCount - 1, returns the last child. */
        Transform* GetChild(unsigned int index) const;
        int GetChildCount() const;

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

        // TODO: Refactor this, it probably needs a GUID of a serialized object??
        static Transform* Instantiate(Transform* transform);

        static void Destroy(Transform* transform);

        component::Component* GetComponentByIndex(unsigned int index) const;
        int GetComponentCount() const;

        std::string GetName() const { return name; }
        std::string GetTag() const { return tag; }
        bool GetManualRotation() const { return manualRotation; }

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

        glm::vec3 GetLocalPosition() const;
        glm::vec3 GetWorldPosition() const;
        glm::vec3 GetLocalScale() const;
        glm::vec3 GetWorldScale() const;
        glm::vec3 GetLocalRotation() const;

        glm::mat4 GetManualRotationMatrix() const { return manualRotationMatrix; }
        void SetManualRotationMatrix(glm::mat4 rotationMatrix) { manualRotationMatrix = rotationMatrix; }

        void SetLocalPosition(glm::vec3 translate);
        void SetScale(glm::vec3 scale);
        void SetLocalRotation(glm::vec3 eulerAngle);

        void ComputeDirectionVectors();

        void UpdateWorldCoordinates();

        glm::mat4 GetModelMatrix() const;
        glm::mat4 ComputeModelMatrix();

        Transform* parent;

        glm::vec3 forward = glm::vec3_forward;
        glm::vec3 right = glm::vec3_right;
        glm::vec3 up = glm::vec3_up;

    protected:
        std::string name; 
        
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

    private:
        bool markedForDeletion = false;
        
        friend class GUIManager;
        friend class m1::GameEngine;
    };
}   // namespace transform2D
