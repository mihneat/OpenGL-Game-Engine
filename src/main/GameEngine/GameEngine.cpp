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
    // Clear the color and depth buffers of the default FB
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameEngine::Update(float deltaTimeSeconds)
{
    UpdateGameLogic(deltaTimeSeconds);
    RenderGameView();
    RenderSceneView();
}

void GameEngine::UpdateGameLogic(float deltaTimeSeconds)
{
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    this->StartComponents(hierarchy);
    this->UpdateComponents(hierarchy, deltaTimeSeconds);
    this->LateUpdateComponents(hierarchy, deltaTimeSeconds);

    this->DestroyMarkedObjects();
}

void GameEngine::RenderGameView()
{
    // Get the game FBO container
    const utils::FBOContainer* fboContainer = GUIManager::GetInstance()->GetGameFBOContainer();
    fboContainer->Bind();
    
    // Update rendering components
    glViewport(0, 0, fboContainer->GetResolution().x, fboContainer->GetResolution().y);
    glPolygonMode(GL_FRONT_AND_BACK, EditorRuntimeSettings::debugMode ? GL_LINE : GL_FILL);
    
    // Set the "skybox" color (flat colour)
    clearColor = GameManager::GetInstance()->GetSkyColor();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderingSystem->Render(hierarchy, mainCam, fboContainer->GetResolution());

    // Render secondary cameras
    for (const auto cam : secondaryCams) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport((int)cam->GetViewportDimensions().x, (int)cam->GetViewportDimensions().y,
            (int)cam->GetViewportDimensions().z, (int)cam->GetViewportDimensions().a);

        renderingSystem->Render(hierarchy, cam, fboContainer->GetResolution());
    }

    // Upload FBO data to the texture
    fboContainer->UploadDataToTexture();
}

void GameEngine::RenderSceneView()
{
    // DrawCoordinateSystem();
    
    // TODO: Render the scene view
    return;
}

// This function does something apparently
// Buna Mihnea
void GameEngine::FrameEnd()
{
    // Bind back the default FB
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
}

glm::vec2 GameEngine::GetResolution()
{
    glm::ivec2 resolution = window->GetResolution();
    return glm::vec2((float)resolution.x, (float)resolution.y);
}

void GameEngine::DestroyObject(Transform* object)
{
    markedForDestruction.insert(object);
}

void GameEngine::DestroyMarkedObjects()
{
    auto it = markedForDestruction.begin();
    while (it != markedForDestruction.end()) {
        // Store the current object
        Transform* object = *it;
        
        // Get the next object
        ++it;

        // Remove the child from its parent
        object->parent->RemoveChild(object);

        // Recursively destroy the object
        DeleteComponents(object);
    }

    markedForDestruction.clear();
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
        [this](Transform* transform) {
            // Remove any children from the objects marked for destruction
            // TODO: Does this break the iteration through the set??
            if (this->markedForDestruction.find(transform) != this->markedForDestruction.end())
                markedForDestruction.erase(transform);
            
            delete transform;
        }
    );
}
