#include "lab_m1/Homework/3 - Steep/Steep.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Camera/CameraFollow.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"

#include <iostream>
#include <vector>
#include <stack>

using namespace std;
using namespace m1;
using namespace loaders;
using namespace component;
using namespace transform;
using namespace prefabManager;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Steep::Steep()
{
    LightManager::Init();

    CreateHierarchy();

    debugMode = false;
    useSceneCamera = false;
}

void Steep::CreateHierarchy()
{
    // Define the root-level Hierarchy transform
    hierarchy = new Transform();

    // Initialize singleton managers
    Transform* gameManager = new Transform(hierarchy);
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

Steep::~Steep()
{
    DeleteComponents(hierarchy);
}


void Steep::Init()
{
    glm::ivec2 resolution = window->GetResolution();

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.TTF"), 30);

    ShaderLoader::InitShaders(this);
    TextureLoader::InitTextures(this);

    // Update initial transforms
    ApplyToComponents(hierarchy, [](Component* component) {},
        [](Transform* transform) {
            transform->Update();
        }
        );

    // Start components
    StartComponents(hierarchy);
}


void Steep::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    clearColor = managers::GameManager::GetInstance()->GetSkyColor();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Steep::Update(float deltaTimeSeconds)
{
    // Update components
    currCam = mainCam;
    projectionMatrix = currCam->GetProjectionMatrix();
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    glPolygonMode(GL_FRONT_AND_BACK, debugMode ? GL_LINE : GL_FILL);

    float deltaTimeTrunc = ((int)(deltaTimeSeconds * 100000)) / 100000.0f;
    this->StartComponents(hierarchy);
    this->UpdateComponents(hierarchy, deltaTimeSeconds);
    this->SetUniforms(); // Rendering happens in LateUpdate, so set all uniforms *after* updating the scene
    this->LateUpdateComponents(hierarchy, deltaTimeSeconds);

    /*glClear(GL_DEPTH_BUFFER_BIT);
    this->RerenderComponents(hierarchy, deltaTimeSeconds);*/
    this->DestroyMarkedObjects();

    // Render secondary cameras
    for (auto cam : secondaryCams) {
        glClear(GL_DEPTH_BUFFER_BIT);

        currCam = cam;
        projectionMatrix = cam->GetProjectionMatrix();
        glViewport((int)currCam->GetViewportDimensions().x, (int)currCam->GetViewportDimensions().y,
            (int)currCam->GetViewportDimensions().z, (int)currCam->GetViewportDimensions().a);
        this->RerenderComponents(hierarchy, deltaTimeSeconds);
    }

}
// This function does something apparently
// Buna Mihnea
void Steep::FrameEnd()
{
    // DrawCoordinateSystem();

    ApplyToComponents(hierarchy, [](Component* component) {                                                                                                                                                     // Hello
        component->FrameEnd();
    });
}

void Steep::GenerateMesh(
    Mesh* mesh,
    std::vector<VertexFormat> vertices,
    std::vector<unsigned int> indices
)
{
    mesh->InitFromData(vertices, indices);

    // Actually create the mesh from the data
    AddMeshToList(mesh);
}

void Steep::DrawMesh(
    std::string meshName,
    std::string shader,
    glm::mat4 modelMatrix,
    bool useViewMatrix,
    std::string texture,
    float texScale
)
{
    RenderMesh(meshes[meshName], shaders[shader], modelMatrix, useViewMatrix, texture, texScale);
}

void Steep::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    RenderMesh(mesh, shader, modelMatrix, true, "", 1.0f);
}

void Steep::SetUniforms()
{
    // Set the uniforms for all shaders
    Shader* shader = shaders["Steep"];
    shader->Use();

    //GLint car_position = glGetUniformLocation(shader->program, "car_position");
    //glUniform3f(car_position, playerCarRef->GetWorldPosition().x, playerCarRef->GetWorldPosition().y, playerCarRef->GetWorldPosition().z);

    //GLint scale_factor = glGetUniformLocation(shader->program, "scale_factor");
    //glUniform1f(scale_factor, scaleFactor);

    GLint eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(eye_position, 1, glm::value_ptr(mainCam->transform->GetWorldPosition()));

    GLint time_of_day = glGetUniformLocation(shader->program, "time_of_day");
    glUniform1f(time_of_day, timeOfDay);

    int cnt = 0;
    // Send light information
    for (int i = 0; i < LightManager::maxLights; ++i)
    {
        if (LightManager::lights[i].isUsed == true) {
            ++cnt;
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].isUsed");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform1i(location, LightManager::lights[i].isUsed);
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].type");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform1i(location, LightManager::lights[i].type);
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].intensity");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform1f(location, LightManager::lights[i].intensity);
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].position");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].position));
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].color");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].color));
        }

        {
            std::string name = std::string("lights[") + std::to_string(i) + std::string("].direction");
            GLuint location = glGetUniformLocation(shader->program, name.c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].direction));
        }
    }

    // cout << "Current active lights: " << cnt << "\n";

    //// Change distortion mode
    //GLint distort_mode = glGetUniformLocation(shader->program, "distort_mode");
    //glUniform1i(distort_mode, distortionMode);

    //// Send fog data
    //GLint fog_color = glGetUniformLocation(shader->program, "fog_color");
    //glUniform4f(fog_color, clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    //GLint render_fog = glGetUniformLocation(shader->program, "render_fog");
    //glUniform1i(render_fog, true);
}

void Steep::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const bool renderUI, const std::string texture, const float texScale)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();

    // Render an object using the specified shader and the specified position
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, 
        useSceneCamera ? glm::value_ptr(GetSceneCamera()->GetViewMatrix()) : (renderUI ? glm::value_ptr(currCam->GetViewMatrix()) : glm::value_ptr(glm::mat4(1.0f))));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, 
        useSceneCamera ? glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()) : (renderUI ? glm::value_ptr(projectionMatrix) : glm::value_ptr(glm::ortho(0.0f, (float)window->GetResolution().x, 0.0f, (float)window->GetResolution().y, 0.01f, 500.0f))));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Set texture data
    if (texture != "")
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mapTextures[texture]->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_color"), 1);
        glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 1);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 0);
    }

    // Send texture scale
    glUniform1f(glGetUniformLocation(shader->program, "tex_scale"), texScale);

    // Send ground data
    if (texture == TextureLoader::GetTexture(TextureLoader::Snow)) {
        GLint is_scrolling = glGetUniformLocation(shader->program, "is_scrolling");
        glUniform1i(is_scrolling, 1);
    }
    else {
        GLint is_scrolling = glGetUniformLocation(shader->program, "is_scrolling");
        glUniform1i(is_scrolling, 0);
    }

    GLint scroll_amount = glGetUniformLocation(shader->program, "scroll_amount");
    glUniform2fv(scroll_amount, 1, glm::value_ptr(playerOffset));

    mesh->Render();
}

void Steep::LoadMesh(std::string meshId, std::string meshName, std::string meshPath)
{
    Mesh* mesh = new Mesh(meshId);
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, meshPath), meshName);
    meshes[mesh->GetMeshID()] = mesh;
}

void Steep::LoadTexture(std::string texId, std::string texPath)
{
    // Set the source directory
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES);

    // Load the texture
    Texture2D* texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, texPath).c_str(), GL_REPEAT);
    mapTextures[texId] = texture;
}

void Steep::LoadShader(std::string shaderName, std::string vertexShaderPath, std::string fragmentShaderPath)
{
    // Set the source directory
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS);

    // Create a shader program for drawing face polygon with the color of the normal
    Shader* shader = new Shader(shaderName);
    shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, vertexShaderPath), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, fragmentShaderPath), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}

glm::vec2 Steep::GetResolution()
{
    glm::ivec2 resolution = window->GetResolution();
    return glm::vec2((float)resolution.x, (float)resolution.y);
}

bool Steep::GetDebugMode()
{
    return debugMode;
}

void Steep::DestroyObject(Transform* object)
{
    markedForDestruction.push(object);
}

void Steep::DestroyMarkedObjects()
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


void Steep::OnInputUpdate(float deltaTime, int mods)
{
    ApplyToComponents(hierarchy, [deltaTime, mods](Component* component) {
        component->InputUpdate(deltaTime, mods);
    });
}


void Steep::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_F1) {
        useSceneCamera = !useSceneCamera;
    }

    if (key == GLFW_KEY_F3) {
        debugMode = !debugMode;
    }

    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyPress(key, mods);
    });
}


void Steep::OnKeyRelease(int key, int mods)
{
    ApplyToComponents(hierarchy, [key, mods](Component* component) {
        component->KeyRelease(key, mods);
    });
}


void Steep::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, deltaX, deltaY](Component* component) {
        component->MouseMove(mouseX, invertedMouseY, deltaX, deltaY);
    });
}


void Steep::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    // Invert mouse position such that (0, 0) is on the bottom left
    const int invertedMouseY = window->GetResolution().y - mouseY;
    ApplyToComponents(hierarchy, [mouseX, invertedMouseY, button, mods](Component* component) {
        component->MouseBtnPress(mouseX, invertedMouseY, button, mods);
    });
}


void Steep::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Steep::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Steep::OnWindowResize(int width, int height)
{
    ApplyToComponents(hierarchy, [width, height](Component* component) {
        component->WindowResize(width, height);
    });
}

void Steep::ApplyToComponents(
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


void Steep::StartComponents(Transform* currentTransform)
{
    // Start the components
    ApplyToComponents(currentTransform, [](Component* component) {
        if (!component->GetHasStartActivated()) {
            component->SetHasStartActivated();
            component->Start();
        }
    });
}

void Steep::UpdateComponents(Transform* currentTransform, const float deltaTime)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [deltaTime](Component* component) {
        component->Update(deltaTime);
        }, [](Transform* transform) {
            transform->Update();
        }
        );
}

void Steep::LateUpdateComponents(Transform* currentTransform, const float deltaTime)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [deltaTime](Component* component) {
        component->LateUpdate(deltaTime);
    });
}

void Steep::RerenderComponents(Transform* currentTransform, const float deltaTime)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [deltaTime](Component* component) {
        if (dynamic_cast<component::IRenderable*>(component)) {
            component->LateUpdate(deltaTime);
        }
    });
}

void Steep::DeleteComponents(Transform* currentTransform)
{
    // Update the transform and the components
    ApplyToComponents(currentTransform, [](Component* component) {
            delete component;
        }, [](Transform* transform) { },
        [](Transform* transform) {
            delete transform;
        }
    );
}
