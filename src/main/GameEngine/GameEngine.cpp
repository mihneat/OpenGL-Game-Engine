#include "main/GameEngine/GameEngine.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

#include "ComponentBase/Components/Logic/Physics/Collider.h"
#include "main/GameEngine/Serialization/Serializer.h"
#include "ComponentBase/Components/Rendering/Camera.h"
#include "core/managers/resource_path.h"
#include "GUI/GUIManager.h"
#include "Managers/GameInstance.h"
#include "Managers/InputManager.h"
#include "Serialization/ObjectSerializer.h"
#include "Systems/FileSystem.h"
#include "Systems/SceneManager.h"
#include "Systems/Editor/EditorRuntimeSettings.h"
#include "Systems/Editor/SceneCamera.h"
#include "Systems/Physics/PhysicsEngine.h"
#include "Systems/Rendering/MaterialManager.h"
#include "Systems/Rendering/MeshResourceManager.h"
#include "Systems/Rendering/ShaderResourceManager.h"

using namespace std;
using namespace m1;
using namespace loaders;
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
    startScene = PATH_JOIN(ENGINE_PATH::ASSETS, "Scenes", "PhysicsScene.scene");

    this->renderingSystem = new RenderingSystem();
    this->physicsEngine = new physics::PhysicsEngine();
    
    LightManager::Init();

    // TODO: Extend to multiple observers
    // "Subscribe" to scene manager updates
    SceneManager::engineRef = this;
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

    shadowMapFBOContainer.SetResolution(glm::ivec2(4096));
    CreateShadowMappingCamera();

    // Create the GameInstance singleton by retrieving it
    managers::GameInstance::Get();

    InputManager::window = window;
    
    ShaderLoader::InitShaders();
    TextureLoader::InitTextures();
    MaterialManager::InitMaterials();
    InitDebugShapes();

    CreateSceneCamera();

    ReloadScene();
}

void GameEngine::InitDebugShapes()
{
    Transform* debugTransform = new Transform();
    MeshRenderer* line = new MeshRenderer(debugTransform, MeshRenderer::Line, "debugLine");
    line->MeshFactory();

    DeleteComponents(debugTransform);
}

void GameEngine::CreateShadowMappingCamera()
{
    // Create the shadow mapping camera
    Transform* shadowMappingTransform = new Transform();
    shadowMappingTransform->Translate(glm::vec3(-250, 300, -250));
    
    shadowMappingCamera = new Camera(shadowMappingTransform);
    shadowMappingTransform->AddComponent(shadowMappingCamera);
    
    shadowMappingCamera->SetOrthographic(1500, 1500, 0.01f, 2000.0f);
    shadowMappingCamera->RotateFirstPerson_OX(glm::radians(45.0f));
    shadowMappingCamera->RotateFirstPerson_OY(glm::radians(45.0f));
}

void GameEngine::CreateSceneCamera()
{
    // Create the scene camera
    Transform* sceneCamTransform = new Transform();
    sceneCamTransform->Translate(glm::vec3(0.0f, 20.0f, -50.0f));
    
    sceneCamera = new SceneCamera(sceneCamTransform);
    sceneCamTransform->AddComponent(sceneCamera);
    sceneCamera->SetPerspective(60, 16.0f / 9.0f);
}

void GameEngine::SaveSceneToFile()
{
    const nlohmann::ordered_json sceneObj = ObjectSerializer::SerializeRootObject(hierarchy);
    
    ofstream fout(startScene);
    fout << sceneObj.dump(2);
}

void GameEngine::ReloadScene()
{
    SceneManager::LoadScene(startScene);
    FindCameras();
}

void GameEngine::HandleSceneLoaded(Transform* root)
{
    if (hierarchy != nullptr)
    {
        mainCam = nullptr;
        secondaryCams.clear();
        
        DeleteComponents(hierarchy);
    }

    hierarchy = root;
}

void GameEngine::FindCameras()
{
    ApplyToComponents(hierarchy, [this](Component* component) {
        Camera* camera = dynamic_cast<Camera*>(component); 
        if (camera != nullptr)
        {
            // TODO: Change after implementing "isMainCam" serialized bool
            if (mainCam == nullptr)
            {
                mainCam = camera;
                return;
            }

            secondaryCams.push_back(camera);
        }
    });
}

void GameEngine::FrameStart()
{
    if (GUIManager::GetInstance()->ShouldReloadShaders())
    {
        ReloadShaders();
        GUIManager::GetInstance()->UnmarkReloadShaders();
    }
    
    if (GUIManager::GetInstance()->ShouldPlay())
    {
        // Save the scene if the game is playing
        if (!GUIManager::GetInstance()->IsGamePlaying())
            SaveSceneToFile();
        
        GUIManager::GetInstance()->ToggleGamePlaying();
    }
    
    if (GUIManager::GetInstance()->ShouldPause())
        GUIManager::GetInstance()->ToggleGamePaused();

    if (GUIManager::GetInstance()->IsGameStepping())
        GUIManager::GetInstance()->StopGameStepping();
    
    if (GUIManager::GetInstance()->ShouldStep())
        GUIManager::GetInstance()->ToggleGameStepping();
    
    if (GUIManager::GetInstance()->ShouldReset())
    {
        ReloadScene();
        GUIManager::GetInstance()->UnmarkReset();
        
        // Force a window resize operation
        const glm::ivec2 resolution = GUIManager::GetInstance()->GetGameWindowResolution();
        OnGameWindowResize(resolution.x, resolution.y);
    }

    if (GUIManager::GetInstance()->ShouldSave() && !GUIManager::GetInstance()->IsGamePlaying())
        SaveSceneToFile();

    GUIManager::GetInstance()->UnmarkSave();

    Transform* currTransform = GUIManager::GetInstance()->GetLastSelectedTransform();
    std::string componentToCreate = GUIManager::GetInstance()->RetrieveComponentToCreate();
    if (!componentToCreate.empty() && currTransform != nullptr)
    {
        // Create a new component and attach it to the currently selected object
        currTransform->AddComponent(Serializer::ComponentFactory(componentToCreate, currTransform));
    }

    Component* componentToDelete = GUIManager::GetInstance()->RetrieveComponentToDelete();
    if (componentToDelete != nullptr && currTransform != nullptr)
        currTransform->RemoveComponent(componentToDelete);

    Transform* transformToCreateChild = GUIManager::GetInstance()->RetrieveTransformToCreateChild();
    if (transformToCreateChild != nullptr)
        // Create a new empty child of the object
        new Transform(transformToCreateChild);

    Transform* transformToDelete = GUIManager::GetInstance()->RetrieveTransformToDelete();
    if (transformToDelete != nullptr)
        // Delete marked item
        Transform::Destroy(transformToDelete);
    
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
    // Check if game is active
    if (GUIManager::GetInstance()->IsGameActive())
    {
        this->StartComponents(hierarchy);

        // Unity places the physics update between Start and Update
        physicsEngine->SimulatePhysics(hierarchy, deltaTimeSeconds);
        
        this->UpdateComponents(hierarchy, deltaTimeSeconds);
        this->LateUpdateComponents(hierarchy, deltaTimeSeconds);
    }

    this->DestroyMarkedObjects();
}

void GameEngine::RenderGameView()
{
    // Quit out early if no cameras are rendering
    if (mainCam == nullptr)
        return;

    // Use the Shadow framebuffer to render the shadows :)
    shadowMapFBOContainer.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 4096, 4096);

    // Render the scene through the sun's eyes
    renderingSystem->Render(hierarchy, textRenderer, shadowMappingCamera, shadowMappingCamera, shadowMapFBOContainer.GetDepthTextureID(),
        true, glm::ivec2(4096), GUIManager::GetInstance()->IsGamePlaying(), true, false);

    // Get the game FBO container
    utils::FBOContainer* fboContainer = GUIManager::GetInstance()->GetGameFBOContainer();
    fboContainer->Bind();
    
    // Update rendering components
    glViewport(0, 0, fboContainer->GetResolution().x, fboContainer->GetResolution().y);
    glPolygonMode(GL_FRONT_AND_BACK, EditorRuntimeSettings::debugMode ? GL_LINE : GL_FILL);
    
    // Set the "skybox" color (flat color)
    clearColor = mainCam->skyboxColor;
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderingSystem->Render(hierarchy, textRenderer, mainCam, shadowMappingCamera, shadowMapFBOContainer.GetDepthTextureID(),
        false, fboContainer->GetResolution(), GUIManager::GetInstance()->IsGamePlaying(), true);

    // Render secondary cameras
    for (const auto cam : secondaryCams) {
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport((int)cam->GetViewportDimensions().x, (int)cam->GetViewportDimensions().y,
            (int)cam->GetViewportDimensions().z, (int)cam->GetViewportDimensions().a);

        renderingSystem->Render(hierarchy, textRenderer, cam, shadowMappingCamera, shadowMapFBOContainer.GetDepthTextureID(),
            false, fboContainer->GetResolution(), GUIManager::GetInstance()->IsGamePlaying(), true, false);
    }

    if (drawDebugShadowMappingTextures)
        DrawFramebufferTextures(&shadowMapFBOContainer);

    // Upload FBO data to the texture
    // fboContainer->UploadDataToTexture();
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
    
    sceneCamera->UpdateValues(fboContainer->GetResolution());

    renderingSystem->Render(hierarchy, textRenderer, sceneCamera, shadowMappingCamera, shadowMapFBOContainer.GetDepthTextureID(),
        false, fboContainer->GetResolution(), GUIManager::GetInstance()->IsGamePlaying(), false, false);
}

void GameEngine::DrawFramebufferTextures(utils::FBOContainer* container)
{
    // Render the color texture on the screen
    glViewport(20, 20, 200, 200);

    RenderTextureScreen(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_VIEW_COLOR_TEXTURE), container->GetColorTextureID());

    // Render the depth texture on the screen
    glViewport(220, 20, 200, 200);

    RenderTextureScreen(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_VIEW_DEPTH_TEXTURE), container->GetDepthTextureID());
}

void GameEngine::RenderTextureScreen(Shader *shader, unsigned int textureID)
{
    if (!shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set uniforms for near and far plane of the
    // projection transformation in the light space
    GLint loc_light_space_near_plane = glGetUniformLocation(shader->program, "light_space_near_plane");
    glUniform1f(loc_light_space_near_plane, 0.01);

    GLint loc_light_space_far_plane = glGetUniformLocation(shader->program, "light_space_far_plane");
    glUniform1f(loc_light_space_far_plane, 2000.0f);

    // Set texture uniform
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 1);

    // Draw the object
    if (drawPlane == nullptr)
    {
        drawPlane = new MeshRenderer(nullptr, MeshRenderer::Square, "Square");
        drawPlane->MeshFactory();
    }
    
    const Mesh *mesh = MeshResourceManager::meshes[std::to_string(drawPlane->meshType)];
    mesh->Render();
}

// This function does something apparently
// Buna Mihnea
void GameEngine::FrameEnd()
{
    // Bind back the default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
}

// TODO: This should be in a separate Input class I think 
glm::vec4 GameEngine::ExtractSelectionData(int mouseX, int mouseY)
{
    glm::vec4 selectionData(0.0f);
    
    GUIManager::GetInstance()->GetGameFBOContainer()->Bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
 
    glReadPixels(mouseX, mouseY, 1, 1, GL_RGBA, GL_FLOAT, &selectionData[0]);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return selectionData;
}

void GameEngine::DestroyMarkedObjects()
{
    // Transform recursion into iterative with a stack
    stack<Transform*> transformStack;
    transformStack.push(hierarchy);

    // Go through all transforms in the scene
    while (!transformStack.empty()) {
        // Update the root
        Transform* currentTransform = transformStack.top();
        transformStack.pop();

        if (currentTransform->markedForDeletion) {
            // Remove the child from its parent
            currentTransform->parent->RemoveChild(currentTransform);

            // Recursively destroy the object
            DeleteComponents(currentTransform);
            
            continue;
        }

        // Add all of root's children to the stack
        for (int i = 0; i < currentTransform->GetChildCount(); ++i) {
            transformStack.push(currentTransform->GetChild(i));
        }
    }
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
    if (key == GLFW_KEY_F1) {
        drawDebugShadowMappingTextures = !drawDebugShadowMappingTextures;
    }

    if (key == GLFW_KEY_F3) {
        EditorRuntimeSettings::debugMode = !EditorRuntimeSettings::debugMode;
    }
    
    // Update the scene camera
    sceneCamera->KeyPress(key, mods);
    
    // Check if game is active
    if (!GUIManager::GetInstance()->IsGameActive())
        return;
    
    if (!GUIManager::GetInstance()->ReceiveGameInput())
        return;

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
        });
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
            delete transform;
        }
    );
}
