#include "main/GameEngine/GameEngine.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Camera/CameraFollow.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"

#include <iostream>
#include <vector>
#include <stack>

#include "core/managers/resource_path.h"
#include "GUI/GUIManager.h"
#include "Systems/FileSystem.h"
#include "Systems/SceneManager.h"
#include "Systems/Editor/EditorRuntimeSettings.h"
#include "Systems/Rendering/MaterialManager.h"

using namespace std;
using namespace m1;
using namespace loaders;
using namespace component;
using namespace rendering;
using namespace transform;
using namespace prefabManager;
using namespace managers;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


GameEngine::GameEngine()
{
    // TODO: Read from config file
    const string defaultScene = PATH_JOIN(FileSystem::rootDirectory, ENGINE_PATH::ASSETS, "Scenes", "EmptyScene.scene");

    this->renderingSystem = new RenderingSystem();
    
    LightManager::Init();
    SceneManager::LoadScene(defaultScene);

    useSceneCamera = false;
}

GameEngine::~GameEngine()
{
    DeleteComponents(hierarchy);
}

void GameEngine::Init()
{
    glm::ivec2 resolution = window->GetResolution();

    textRenderer = new gfxc::TextRenderer(FileSystem::rootDirectory, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::FONTS, "Hack-Bold.TTF"), 30);

    ShaderLoader::InitShaders();
    TextureLoader::InitTextures();
    MaterialManager::InitMaterials();

    CreateHierarchy();

    // Update initial transforms
    ApplyToComponents(hierarchy, [](Component* component) {},
        [](Transform* transform) {
            transform->Update();
        }
        );

    // Start components
    StartComponents(hierarchy);
}

void GameEngine::CreateHierarchy()
{
    // Define the root-level Hierarchy transform
    hierarchy = new Transform();

    // Initialize singleton managers
    Transform* gameManager = new Transform(hierarchy, "Game Manager");
    managers::GameManager::GetInstance(gameManager, this);
    gameManager->AddComponent(managers::GameManager::GetInstance());

    // Create the player
    Transform* player = PrefabManager::CreatePlayer(hierarchy);

    // Create the camera object
    glm::ivec2 resolution = window->GetResolution();
    Transform* camera = PrefabManager::CreateCamera(hierarchy, player, window->props.aspectRatio,
        glm::vec2(0.0f, 0.0f), glm::vec2(resolution.x, resolution.y));
    this->mainCam = camera->GetComponent<Camera>();

    // Create the ground
    Transform* ground = PrefabManager::CreateGround(hierarchy);

    // Create the sun
    Transform* sun = PrefabManager::CreateSun(hierarchy);

    // Create the object spawner
    Transform* objectSpawner = PrefabManager::CreateObjectSpawner(hierarchy);

    // Create the UI
    Transform* rootUI = PrefabManager::CreateUI(this, hierarchy);

    // Create the shader params
    Transform* shaderParams = PrefabManager::CreateShaderParams(hierarchy);
}

void GameEngine::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    clearColor = managers::GameManager::GetInstance()->GetSkyColor();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    // Render the hierarchy in the GUI
    GUIManager::GetInstance()->ShowHierarchy(hierarchy);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void GameEngine::Update(float deltaTimeSeconds)
{
    // Update components
    currCam = mainCam;
    projectionMatrix = currCam->GetProjectionMatrix();
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    glPolygonMode(GL_FRONT_AND_BACK, EditorRuntimeSettings::debugMode ? GL_LINE : GL_FILL);

    this->StartComponents(hierarchy);
    this->UpdateComponents(hierarchy, deltaTimeSeconds);
    this->LateUpdateComponents(hierarchy, deltaTimeSeconds);

    this->DestroyMarkedObjects();

    renderingSystem->Render(hierarchy, currCam, window);

    // Render secondary cameras
    for (const auto cam : secondaryCams) {
        glClear(GL_DEPTH_BUFFER_BIT);

        currCam = cam;
        projectionMatrix = cam->GetProjectionMatrix();
        glViewport((int)currCam->GetViewportDimensions().x, (int)currCam->GetViewportDimensions().y,
            (int)currCam->GetViewportDimensions().z, (int)currCam->GetViewportDimensions().a);

        renderingSystem->Render(hierarchy, currCam, window);
    }

}
// This function does something apparently
// Buna Mihnea
void GameEngine::FrameEnd()
{
    // DrawCoordinateSystem();

    ApplyToComponents(hierarchy, [](Component* component) {                                                                                                                                                     // Hello
        component->FrameEnd();
    });
}

glm::vec2 GameEngine::GetResolution()
{
    glm::ivec2 resolution = window->GetResolution();
    return glm::vec2((float)resolution.x, (float)resolution.y);
}

void GameEngine::DestroyObject(Transform* object)
{
    markedForDestruction.push(object);
}

void GameEngine::DestroyMarkedObjects()
{
    while (!markedForDestruction.empty()) {
        // Get the first object
        Transform* object = markedForDestruction.top();
        markedForDestruction.pop();

        // Remove the child from its parent
        object->parent->RemoveChild(object);

        // Recursively destroy the object
        // PROBLEM: If a child of this object is ALSO marked for deletion, we might get
        // double free errors, please keep this in mind :( fkin c++
        DeleteComponents(object);
    }
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void GameEngine::OnInputUpdate(float deltaTime, int mods)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    ApplyToComponents(hierarchy, [deltaTime, mods](Component* component) {
        component->InputUpdate(deltaTime, mods);
    });
}


void GameEngine::OnKeyPress(int key, int mods)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    if (key == GLFW_KEY_F1) {
        useSceneCamera = !useSceneCamera;
    }

    if (key == GLFW_KEY_F3) {
        EditorRuntimeSettings::debugMode = !EditorRuntimeSettings::debugMode;
    }

    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyPress(key, mods);
    });
}


void GameEngine::OnKeyRelease(int key, int mods)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyRelease(key, mods);
    });
}


void GameEngine::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, deltaX, deltaY](Component* component) {
        component->MouseMove(mouseX, invertedMouseY, deltaX, deltaY);
    });
}


void GameEngine::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    // Add mouse button press event
    // Invert mouse position such that (0, 0) is on the bottom left
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, button, mods](Component* component) {
        component->MouseBtnPress(mouseX, invertedMouseY, button, mods);
    });
}


void GameEngine::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    // Add mouse button release event
    // Invert mouse position such that (0, 0) is on the bottom left
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, button, mods](Component* component) {
        component->MouseBtnRelease(mouseX, invertedMouseY, button, mods);
    });
}


void GameEngine::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    if (GUIManager::GetInstance()->IsGUIInput())
        return;
    
    // Add mouse scroll event
    // Invert mouse position such that (0, 0) is on the bottom left
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, offsetX, offsetY](Component* component) {
        component->MouseScroll(mouseX, invertedMouseY, offsetX, offsetY);
    });
}


void GameEngine::OnWindowResize(int width, int height)
{
    ApplyToComponents(hierarchy, [width, height](Component* component) {
        component->WindowResize(width, height);
    });
}

void GameEngine::ApplyToComponents(
    Transform* currentTransform,
    function<void(Component*)> componentApplication,
    function<void(Transform*)> transformApplicationApriori,
    function<void(Transform*)> transformApplicationAposteriori
)
{
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

        // Apply to transform before components
        transformApplicationApriori(currentTransform);

        // Call the Start function of each component
        for (int i = 0; i < currentTransform->GetComponentCount(); ++i) {
            Component* curr = currentTransform->GetComponentByIndex(i);

            // Check if the component is enabled
            if (!curr->IsActive()) {
                continue;
            }

            // Apply to component
            componentApplication(curr);
        }

        // Apply to transform after components
        transformApplicationAposteriori(currentTransform);
    }
}


void GameEngine::StartComponents(Transform* currentTransform)
{
    // Start the components
    ApplyToComponents(currentTransform, [](Component* component) {
        if (!component->GetHasStartActivated()) {
            component->SetHasStartActivated();
            component->Start();
        }
    });
}

void GameEngine::UpdateComponents(Transform* currentTransform, const float deltaTime)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [deltaTime](Component* component) {
        component->Update(deltaTime);
        }, [](Transform* transform) {
            transform->Update();
        }
        );
}

void GameEngine::LateUpdateComponents(Transform* currentTransform, const float deltaTime)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [deltaTime](Component* component) {
        component->LateUpdate(deltaTime);
    });
}

void GameEngine::DeleteComponents(Transform* currentTransform)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [](Component* component) {
            delete component;
        }, [](Transform* _) { },
        [](Transform* transform) {
            delete transform;
        }
    );
}
