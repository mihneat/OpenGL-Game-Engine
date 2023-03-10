#include "lab_m1/Homework/3 - Steep/PrefabManager.h"
#include "lab_m1/Homework/3 - Steep/Managers/TextureLoader.h"

using namespace std;
using namespace loaders;
using namespace managers;
using namespace component;
using namespace transform;
using namespace prefabManager;

Transform* PrefabManager::CreateCamera(Transform* parent, Transform* player, const float aspectRatio,
    const glm::vec2 viewportBottomLeft, const glm::vec2 viewportTopRight)
{
    // Create the camera object
    Transform* camera = new Transform(parent);
    Camera* camComponent = new Camera(camera, glm::vec3(0), glm::vec3_up, { MeshRenderer::Default, MeshRenderer::UI });
    camera->AddComponent(camComponent);
    camera->AddComponent(new CameraFollow(camera, player, 100.0f, 65.0f)); // Initial: 100.0f, 55.0f

    camComponent->SetProjection(60, aspectRatio);

    return camera;
}

//Transform* PrefabManager::CreateUICamera(Transform* parent, const glm::vec2 viewportBottomLeft, const glm::vec2 viewportTopRight)
//{
//    // Create the camera object
//    Transform* camera = new Transform(parent);
//    camera->Translate(glm::vec3(0.0f, 220.0f, 80.0f));
//    Camera* camComponent = new Camera(camera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3_up, viewportBottomLeft, viewportTopRight, { MeshRenderer::UI });
//    camera->AddComponent(camComponent);
//
//    camComponent->Set(glm::vec3(0.0f), glm::vec3_f, glm::vec3_up);
//    camComponent->SetOrtographic(fixedDimensions.x, fixedDimensions.y);
//
//    return camera;
//}

Transform* PrefabManager::CreatePlayer(Transform* parent)
{
    // Create the empty player (SET TO MANUAL ROTATION)
    Transform* player = new Transform(parent, "Player", true);
    player->SetManualRotationMatrix(glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6.0f, glm::vec3_right));
    player->AddComponent(new PlayerController(player));

    // Create the player mesh
    Transform* playerMesh = new Transform(player);

    Transform* playerBody = new Transform(playerMesh);
    playerBody->AddComponent(new MeshRenderer(playerBody, MeshRenderer::CubeMesh, "playerBody", "Steep",
        MeshRenderer::Default, glm::vec3(4.0f, 10.0f, 4.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)));

    Transform* playerSki1 = new Transform(playerMesh);
    playerSki1->Translate(glm::vec3(-1.8f, -4.8f,  1.8f));
    playerSki1->AddComponent(new MeshRenderer(playerSki1, MeshRenderer::Cube, "playerSki1", "Steep",
        MeshRenderer::Default, glm::vec3(1.0f, 0.2f, 7.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

    Transform* playerSki2 = new Transform(playerMesh);
    playerSki2->Translate(glm::vec3( 1.8f, -4.8f,  1.8f));
    playerSki2->AddComponent(new MeshRenderer(playerSki2, MeshRenderer::Cube, "playerSki2", "Steep",
        MeshRenderer::Default, glm::vec3(1.0f, 0.2f, 7.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

    // Create the debug-only collision box
    Transform* collisionSphere = new Transform(player);
    collisionSphere->AddComponent(new MeshRenderer(collisionSphere, MeshRenderer::Sphere, "playerCollider", "Steep",
        MeshRenderer::Default, glm::vec3(4.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true, true));

    return player;
}

Transform* PrefabManager::CreateGround(Transform* parent)
{
    // Create the ground
    Transform* ground = new Transform(parent);
    ground->Translate(glm::vec3(0.0f, -10.0f, 0.0f)); // Initial: -5.8f
    ground->Rotate(glm::vec3(glm::pi<float>() / 2.0f, 0.0f, 0.0f));  // Make plane horizontal
    ground->Rotate(-glm::vec3(glm::pi<float>() / 6.0f, 0.0f, 0.0f)); // Slope incline
    ground->AddComponent(new MeshRenderer(ground, MeshRenderer::FragmentedSquare, "snowGround", "Steep",
        MeshRenderer::Default, glm::vec3(200.0f, 200.0f, 1.0f), glm::vec4(1.0f)));
    ground->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::Snow), 4.0f);
    ground->AddComponent(new GroundStick(ground));

    return ground;
}

Transform* PrefabManager::CreateSun(Transform* parent)
{
    // Create the sun
    Transform* sun = new Transform(parent);
    sun->AddComponent(new Sun(sun));
    sun->AddComponent(new DirectionalLight(sun));
    sun->GetComponent<DirectionalLight>()->ChangeIntensity(0.8f); // Initial: 0.4f
    // sun->GetComponent<DirectionalLight>()->ChangeColor(glm::vec3(0.2f, 0.9f, 0.2f));
    sun->GetComponent<DirectionalLight>()->ChangeDirection(glm::normalize(glm::vec3(-1.0f)));
    
    return sun;
}

Transform* PrefabManager::CreateObjectSpawner(Transform* parent)
{
    // Create the object spawner
    Transform* objectSpawner = new Transform(parent);
    objectSpawner->AddComponent(new ObjectSpawner(objectSpawner));

    return objectSpawner;
}

Transform* PrefabManager::CreateTree(Transform* parent)
{
    // Create a tree
    Transform* tree = new Transform(parent);
    tree->AddComponent(new Obstacle(tree, 4.5f));

    // Create the meshes
    Transform* treeTrunk = new Transform(tree);
    treeTrunk->AddComponent(new MeshRenderer(treeTrunk, MeshRenderer::CubeMesh, "treeTrunk", "Steep",
        MeshRenderer::Default, glm::vec3(5.6f, 30.0f, 5.6f), glm::vec4(128, 77, 33, 255) / 255.0f));
    treeTrunk->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::TreeBark), 3.0f);

    Transform* treeCorona = new Transform(tree);
    treeCorona->Translate(glm::vec3(0.0f, 20.0f, 0.0f));
    treeCorona->AddComponent(new MeshRenderer(treeCorona, MeshRenderer::CubeMesh, "treeCorona", "Steep",
        MeshRenderer::Default, glm::vec3(20.0f), glm::vec4(98, 204, 53, 255) / 255.0f));
    treeCorona->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::TreeCorona));

    // Create the light
    Transform* light = new Transform(treeCorona);
    light->Translate(glm::vec3(0.0f, 15.0f, 0.0f));
    light->AddComponent(new UpdateLightPosition(light));
    light->AddComponent(new PointLight(light));
    light->GetComponent<PointLight>()->ChangeIntensity(250.0f);
    light->GetComponent<PointLight>()->ChangeColor(glm::vec3(106, 209, 48) / 255.0f);

    return tree;
}

Transform* PrefabManager::CreateRock(Transform* parent)
{
    // Create the rock
    Transform* rock = new Transform(parent);
    rock->AddComponent(new Obstacle(rock, 7.5f));

    // Create the meshes
    Transform* rock1 = new Transform(rock);
    rock1->Translate(glm::vec3(-4.0f, 0.0f, -1.0f));
    rock1->AddComponent(new MeshRenderer(rock1, MeshRenderer::Sphere, "rock1", "Steep",
        MeshRenderer::Default, glm::vec3(12.0f), glm::vec4(61, 53, 65, 255) / 255.0f));
    rock1->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::Rock));

    Transform* rock2 = new Transform(rock);
    rock2->Translate(glm::vec3(4.5f, 0.0f, 1.0f));
    rock2->AddComponent(new MeshRenderer(rock2, MeshRenderer::Sphere, "rock2", "Steep",
        MeshRenderer::Default, glm::vec3(9.0f), glm::vec4(61, 53, 65, 255) / 255.0f));
    rock2->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::Rock));

    // Create the light
    Transform* light = new Transform(rock);
    light->Translate(glm::vec3(0.0f, 10.0f, 0.0f));
    light->AddComponent(new UpdateLightPosition(light));
    light->AddComponent(new PointLight(light));
    light->GetComponent<PointLight>()->ChangeIntensity(200.0f);
    light->GetComponent<PointLight>()->ChangeColor(glm::vec3(131, 109, 101) / 255.0f);

    return rock;
}

Transform* PrefabManager::CreateLightPole(Transform* parent)
{
    // Create the light pole
    Transform* lightPole = new Transform(parent);
    lightPole->AddComponent(new Obstacle(lightPole, 0.7f));

    // Create the meshes
    Transform* verticalPole = new Transform(lightPole);
    verticalPole->AddComponent(new MeshRenderer(verticalPole, MeshRenderer::CubeMesh, "verticalPole", "Steep",
        MeshRenderer::Default, glm::vec3(2.0f, 40.0f, 2.0f), glm::vec4(77, 54, 45, 255) / 255.0f));
    verticalPole->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::LightPole));

    Transform* horizontalPole = new Transform(lightPole);
    horizontalPole->Translate(glm::vec3(0.0f, 20.0f, 0.0f));
    horizontalPole->AddComponent(new MeshRenderer(horizontalPole, MeshRenderer::CubeMesh, "horizontalPole", "Steep",
        MeshRenderer::Default, glm::vec3(25.0f, 3.0f, 3.0f), glm::vec4(77, 54, 45, 255) / 255.0f));
    horizontalPole->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::LightPole));

    // Create the lights
    Transform* light1 = new Transform(horizontalPole);
    light1->Translate(glm::vec3(-12.0f, 0.0f, 0.0f));
    light1->AddComponent(new UpdateLightPosition(light1));
    light1->AddComponent(new SpotLight(light1));
    light1->GetComponent<SpotLight>()->ChangeIntensity(300.0f);
    light1->GetComponent<SpotLight>()->ChangeColor(glm::vec3(250, 191, 77) / 255.0f);

    Transform* light2 = new Transform(horizontalPole);
    light2->Translate(glm::vec3( 12.0f, 0.0f, 0.0f));
    light2->AddComponent(new UpdateLightPosition(light2));
    light2->AddComponent(new SpotLight(light2));
    light2->GetComponent<SpotLight>()->ChangeIntensity(300.0f);
    light2->GetComponent<SpotLight>()->ChangeColor(glm::vec3(250, 191, 77) / 255.0f);

    return lightPole;
}

Transform* PrefabManager::CreatePresent(Transform* parent)
{
    // Create the present
    Transform* present = new Transform(parent);
    present->AddComponent(new Obstacle(present, 10.0f, false));

    // Create the meshes
    Transform* presentBox = new Transform(present);
    presentBox->AddComponent(new MeshRenderer(presentBox, MeshRenderer::CubeMesh, "presentBox", "Steep",
        MeshRenderer::Default, glm::vec3(10.0f), glm::vec4(168, 7, 130, 255) / 255.0f));
    presentBox->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTexture(TextureLoader::Present));

    // Create the light
    Transform* light = new Transform(presentBox);
    light->Translate(glm::vec3(0.0f, 15.0f, 0.0f));
    light->AddComponent(new UpdateLightPosition(light));
    light->AddComponent(new PointLight(light));
    light->GetComponent<PointLight>()->ChangeIntensity(150.0f);
    light->GetComponent<PointLight>()->ChangeColor(glm::vec3(247, 48, 37) / 255.0f);   // Pink: 237, 0, 255

    return present;
}

Transform* PrefabManager::CreateUI(m1::Steep* scene, Transform* parent)
{
    // Create the top-level object
    Transform* rootUI = new Transform(parent);

    // Get the resolution
    glm::vec2 resolution = scene->GetResolution();

    // Create the Runs text
    Transform* runsText = new Transform(rootUI);
    runsText->AddComponent(new TextRenderer(runsText, "Run #1", glm::vec2(resolution.x / 2.0f - 80.0f, 25), 1.5f, glm::vec4(0.8f, 0.2f, 0.2f, 1.0f)));
    runsText->AddComponent(new RunsDisplay(runsText));

    // Create the distance text
    Transform* distanceText = new Transform(rootUI);
    distanceText->AddComponent(new TextRenderer(distanceText, "distance: 0m", glm::vec2(resolution.x / 2.0f - 50.0f, 70), 0.9f, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)));
    distanceText->AddComponent(new DistanceDisplay(distanceText));

    // Create the text
    Transform* scoreText = new Transform(rootUI);
    scoreText->AddComponent(new TextRenderer(scoreText, "Score", glm::vec2(105.0f, 30.0f), 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

    Transform* currentScoreAboveText = new Transform(rootUI);
    currentScoreAboveText->AddComponent(new TextRenderer(currentScoreAboveText, "Curr", glm::vec2(50.0f, 60.0f), 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

    Transform* highScoreAboveText = new Transform(rootUI);
    highScoreAboveText->AddComponent(new TextRenderer(highScoreAboveText, "High", glm::vec2(160.0f, 60.0f), 1.0f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)));

    // Create the score text
    Transform* currentScoreText = new Transform(rootUI);
    currentScoreText->AddComponent(new TextRenderer(currentScoreText, "", glm::vec2(60.0f, 90.0f), 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    currentScoreText->AddComponent(new ScoreDisplay(currentScoreText));

    // Create the high score text
    Transform* highScoreText = new Transform(rootUI);
    highScoreText->AddComponent(new TextRenderer(highScoreText, "", glm::vec2(170.0f, 90.0f), 1.0f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)));
    highScoreText->AddComponent(new HighScoreDisplay(highScoreText));

    // Create the lives holder
    Transform* livesHolder = new Transform(rootUI);
    livesHolder->Translate(glm::vec3(resolution.x - 180, resolution.y - 50, -200.0f));
    livesHolder->AddComponent(new LifeDisplay(livesHolder));
    livesHolder->AddComponent(new UiPanel(livesHolder));
    livesHolder->GetComponent<UiPanel>()->AnchorTop(50);
    livesHolder->GetComponent<UiPanel>()->AnchorRight(180);
    
    // Create the lives
    Transform* life1 = new Transform(livesHolder);
    life1->Translate(glm::vec3(-100.0f, 0.0f, 0.0f));
    life1->AddComponent(new MeshRenderer(life1, MeshRenderer::ZeldaHeart, "life1", "Simple", MeshRenderer::UI,
        glm::vec3(70.0f), glm::vec4(1.0f), false));

    Transform* life2 = new Transform(livesHolder);
    life2->AddComponent(new MeshRenderer(life2, MeshRenderer::ZeldaHeart, "life2", "Simple", MeshRenderer::UI,
        glm::vec3(70.0f), glm::vec4(1.0f), false));

    Transform* life3 = new Transform(livesHolder);
    life3->Translate(glm::vec3( 100.0f, 0.0f, 0.0f));
    life3->AddComponent(new MeshRenderer(life3, MeshRenderer::ZeldaHeart, "life3", "Simple", MeshRenderer::UI,
        glm::vec3(70.0f), glm::vec4(1.0f), false));

    // Create the start run text
    Transform* startRunText = new Transform(rootUI);
    startRunText->AddComponent(new TextRenderer(startRunText, "Press 'left click' to begin the run..", glm::vec2(260.0f, 200.0f), 1.2f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));
    startRunText->AddComponent(new StartRunDisplay(startRunText));

    // Create speed selection text
    Transform* selectSpeedText = new Transform(rootUI);
    selectSpeedText->AddComponent(new TextRenderer(selectSpeedText, "Select speed: ", glm::vec2(300.0f, 400.0f), 1.7f, glm::vec4(112, 171, 249, 255) / 255.0f));
    selectSpeedText->AddComponent(new SpeedSelectionDisplay(selectSpeedText));

    Transform* prevSpeedText = new Transform(selectSpeedText);
    prevSpeedText->AddComponent(new TextRenderer(prevSpeedText, "", glm::vec2(740.0f, 350.0f), 1.4f, glm::vec4(0.7f)));

    Transform* currSpeedText = new Transform(selectSpeedText);
    currSpeedText->AddComponent(new TextRenderer(currSpeedText, "", glm::vec2(720.0f, 400.0f), 1.7f, glm::vec4(1.0f)));

    Transform* nextSpeedText = new Transform(selectSpeedText);
    nextSpeedText->AddComponent(new TextRenderer(nextSpeedText, "", glm::vec2(740.0f, 458.0f), 1.4f, glm::vec4(0.7f)));


    // Create the game over screen
    Transform* gameOverText = new Transform(rootUI);
    gameOverText->AddComponent(new TextRenderer(gameOverText, "Game Over", glm::vec2(350.0f, 250.0f), 3.6f, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f)));
    gameOverText->AddComponent(new GameOverDisplay(gameOverText));

    Transform* gameOverSubText = new Transform(rootUI);
    gameOverSubText->AddComponent(new TextRenderer(gameOverSubText, "- press 'R' to restart -", glm::vec2(380.0f, 350.0f), 1.2f, glm::vec4(1.0f, 0.6f, 0.6f, 1.0f)));
    gameOverSubText->AddComponent(new GameOverDisplay(gameOverSubText));

    return rootUI;
}

Transform* PrefabManager::CreateShaderParams(transform::Transform* parent)
{
    // Create the shader params root object
    Transform* shaderParams = new Transform(parent, "SteepShaderParams");
    shaderParams->AddComponent(new SteepShaderParams(parent));

    return shaderParams;
}

