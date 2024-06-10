#include "main/GameEngine/GameEngine.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Camera/CameraFollow.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"

#include <iostream>
#include <vector>
#include <stack>

#include "core/managers/resource_path.h"
#include "GUI/GUIManager.h"
#include "Managers/GameInstance.h"
#include "Managers/InputManager.h"
#include "Systems/FileSystem.h"
#include "Systems/SceneManager.h"
#include "Systems/Editor/EditorRuntimeSettings.h"
#include "Systems/Editor/SceneCamera.h"
#include "Systems/Rendering/MaterialManager.h"

using namespace std;
using namespace m1;
using namespace loaders;
using namespace component;
using namespace rendering;
using namespace transform;
using namespace prefabManager;
using namespace component;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


GameEngine::GameEngine()
{
    // TODO: Read from config file
    string activeScene = PATH_JOIN(ENGINE_PATH::ASSETS, "Scenes", "SteepScene.scene");

    this->renderingSystem = new RenderingSystem();
    
    LightManager::Init();
    SceneManager::LoadScene(activeScene);
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

    // Create the GameInstance singleton by retrieving it
    managers::GameInstance::Get();

    InputManager::window = window;
    
    ShaderLoader::InitShaders();
    TextureLoader::InitTextures();
    MaterialManager::InitMaterials();

    CreateSceneCamera();

    ReloadScene();

    // DEPRECATED - left here in case things break
    // Update initial transforms
    // UpdateTransforms(hierarchy);

    // Start components
    // StartComponents(hierarchy);
}

void GameEngine::CreateSceneCamera()
{
    // Create the scene camera
    Transform* sceneCamTransform = new Transform();
    sceneCamTransform->Translate(glm::vec3(0.0f, 20.0f, -50.0f));
    
    sceneCamera = new SceneCamera(sceneCamTransform);
    sceneCamTransform->AddComponent(sceneCamera);
    sceneCamera->SetProjection(60, 16.0f / 9.0f);
}

void GameEngine::ReloadScene()
{
    if (hierarchy != nullptr)
    {
        mainCam = nullptr;
        secondaryCams.clear();
        markedForDestruction.clear();
        
        DeleteComponents(hierarchy);

        hierarchy = nullptr;
    }
    
    CreateHierarchy();
    renderingSystem->Init(hierarchy, textRenderer);
}

void GameEngine::CreateHierarchy()
{
    // Define the root-level Hierarchy transform
    hierarchy = new Transform();

    // Create a persistent singleton object
    // The users should not declare their own singletons, but rather
    // use the engine-created object
    // TODO: Add the object to "DontDestroyOnLoad" when the functionality is added
    Transform* gameInstance = new Transform(hierarchy, "Game Instance");
    managers::GameInstance::gameInstance->AttachTransform(gameInstance);

    // The following components are added by the user within the editor
    gameInstance->AddComponent(new GameManager(gameInstance, this));
    
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
    if (GUIManager::GetInstance()->ShouldPlay())
        GUIManager::GetInstance()->ToggleGamePlaying();
    
    if (GUIManager::GetInstance()->ShouldPause())
        GUIManager::GetInstance()->ToggleGamePaused();
    
    if (GUIManager::GetInstance()->ShouldReset())
    {
        ReloadScene();
        GUIManager::GetInstance()->UnmarkReset();
    }
    
    // Clear the color and depth buffers of the default FB
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set engine resolution
    const glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// This executes before the input callbacks from world.cpp
void GameEngine::PreUpdate()
{
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    AwakeComponents(hierarchy);
}

void GameEngine::Update(float deltaTimeSeconds)
{
    UpdateGameLogic(deltaTimeSeconds);
    RenderGameView();
    RenderSceneView();
}

void GameEngine::UpdateGameLogic(float deltaTimeSeconds)
{
    // Update transforms regardless if the game is playing or not
    UpdateTransforms(hierarchy);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;

    this->StartComponents(hierarchy);
    this->UpdateTransforms(hierarchy);
    this->UpdateComponents(hierarchy, deltaTimeSeconds);
    this->UpdateTransforms(hierarchy);
    this->LateUpdateComponents(hierarchy, deltaTimeSeconds);
    this->UpdateTransforms(hierarchy);

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
    clearColor = managers::GameInstance::Get()->GetComponent<GameManager>()->GetSkyColor();
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
    // Get the scene FBO container
    const utils::FBOContainer* fboContainer = GUIManager::GetInstance()->GetSceneFBOContainer();
    fboContainer->Bind();
    
    // Update rendering components
    glViewport(0, 0, fboContainer->GetResolution().x, fboContainer->GetResolution().y);
    glPolygonMode(GL_FRONT_AND_BACK, EditorRuntimeSettings::debugMode ? GL_LINE : GL_FILL);
    
    // Set the "skybox" color (flat colour)
    constexpr glm::vec4 defaultSkyboxColor = glm::vec4(40, 40, 40, 255) / 255.0f;
    glClearColor(defaultSkyboxColor.r, defaultSkyboxColor.g, defaultSkyboxColor.b, defaultSkyboxColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the scene camera
    Transform* transformToFocus = GUIManager::GetInstance()->GetTransformToFocus();
    if (transformToFocus != nullptr && GUIManager::GetInstance()->IsSceneHovered())
    {
        sceneCamera->FocusTransform(transformToFocus);
        GUIManager::GetInstance()->FinishTransformFocus();
    }
    
    sceneCamera->transform->Update();
    sceneCamera->UpdateValues(fboContainer->GetResolution());

    // TODO: Why no work?
    // DrawCoordinateSystem();

    renderingSystem->Render(hierarchy, sceneCamera, fboContainer->GetResolution(), false);

    // Upload FBO data to the texture
    fboContainer->UploadDataToTexture();
}

// This function does something apparently
// Buna Mihnea
void GameEngine::FrameEnd()
{
    // Bind back the default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
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
    // Update the scene camera
    sceneCamera->InputUpdate(deltaTime, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    ApplyToComponents(hierarchy, [deltaTime, mods](Component* component) {
        component->InputUpdate(deltaTime, mods);
    });
}


void GameEngine::OnKeyPress(int key, int mods)
{
    // Update the scene camera
    sceneCamera->KeyPress(key, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;

    if (key == GLFW_KEY_F3) {
        EditorRuntimeSettings::debugMode = !EditorRuntimeSettings::debugMode;
    }

    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyPress(key, mods);
    });
}


void GameEngine::OnKeyRelease(int key, int mods)
{
    // Update the scene camera
    sceneCamera->KeyRelease(key, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyRelease(key, mods);
    });
}


void GameEngine::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Update the scene camera
    const int invertedMouseY = window->GetResolution().y - mouseY;
    sceneCamera->MouseMove(mouseX, invertedMouseY, deltaX, deltaY);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    // Add mouse move event
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, deltaX, deltaY](Component* component) {
        component->MouseMove(mouseX, invertedMouseY, deltaX, deltaY);
    });
}


void GameEngine::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Update the scene camera
    const int invertedMouseY = window->GetResolution().y - mouseY;
    sceneCamera->MouseBtnPress(mouseX, invertedMouseY, button, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    // Add mouse button press event
    // Invert mouse position such that (0, 0) is on the bottom left
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, button, mods](Component* component) {
        component->MouseBtnPress(mouseX, invertedMouseY, button, mods);
    });
}


void GameEngine::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Update the scene camera
    const int invertedMouseY = window->GetResolution().y - mouseY;
    sceneCamera->MouseBtnRelease(mouseX, invertedMouseY, button, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    // Add mouse button release event
    // Invert mouse position such that (0, 0) is on the bottom left
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, button, mods](Component* component) {
        component->MouseBtnRelease(mouseX, invertedMouseY, button, mods);
    });
}


void GameEngine::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Update the scene camera
    const int invertedMouseY = window->GetResolution().y - mouseY;
    sceneCamera->MouseScroll(mouseX, invertedMouseY, offsetX, offsetY);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;
    
    // Add mouse scroll event
    // Invert mouse position such that (0, 0) is on the bottom left
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, offsetX, offsetY](Component* component) {
        component->MouseScroll(mouseX, invertedMouseY, offsetX, offsetY);
    });
}


void GameEngine::OnWindowResize(int width, int height)
{
    // This is to be used during the BUILT version of the game
    
    // ApplyToComponents(hierarchy, [width, height](Component* component) {
    //     component->WindowResize(width, height);
    // });
}

void GameEngine::OnGameWindowResize(int width, int height)
{
    EditorRuntimeSettings::resolution = {width, height};
    
    ApplyToComponents(hierarchy, [width, height](Component* component) {
        component->WindowResize(width, height);
    });

    UpdateTransforms(hierarchy);
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

void GameEngine::AwakeComponents(Transform* currentTransform)
{
    // Start the components
    ApplyToComponents(currentTransform, [](Component* component) {
        if (!component->GetHasAwakeActivated()) {
            component->SetHasAwakeActivated();
            component->Awake();
        }
    });
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
    }, [](Transform* transform) {
            transform->Update();
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

void GameEngine::UpdateTransforms(Transform* currentTransform)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [](Component* _) {},
        [](Transform* transform) {
            transform->Update();
        }
        );
}
