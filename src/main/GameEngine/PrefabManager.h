#pragma once

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Transform.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/UiPanel.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Shading/SteepShaderParams.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Camera/CameraFollow.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Camera/OrthoCameraFollow.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Objects/Obstacle.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Objects/ObjectSpawner.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Objects/UpdateLightPosition.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Player/GroundStick.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Player/PlayerController.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Time/Sun.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/ScoreDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/RunsDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/DistanceDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/HighScoreDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/LifeDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/StartRunDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/SpeedSelectionDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Logic/UI/GameOverDisplay.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/DirectionalLight.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/SpotLight.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/PointLight.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class SpriteRenderer;
}

namespace m1 {
    class GameEngine;
}

namespace prefabManager
{
    class PrefabManager
    {
    public:
        static transform::Transform* CreateCamera(transform::Transform* parent, transform::Transform* player,
            const float aspectRatio, const glm::vec2 viewportBottomLeft, const glm::vec2 viewportTopRight);
        static transform::Transform* CreatePlayer(transform::Transform* parent);
        static transform::Transform* CreateGround(transform::Transform* parent);
        static transform::Transform* CreateSun(transform::Transform* parent);
        static transform::Transform* CreateObjectSpawner(transform::Transform* parent);
        static transform::Transform* CreateTree(transform::Transform* parent);
        static transform::Transform* CreateRock(transform::Transform* parent);
        static transform::Transform* CreateLightPole(transform::Transform* parent);
        static transform::Transform* CreatePresent(transform::Transform* parent);
        static transform::Transform* CreateUI(m1::GameEngine* scene, transform::Transform* parent);
        static transform::Transform* CreateShaderParams(transform::Transform* parent);

    private:
        PrefabManager() {}
        ~PrefabManager() {}

    };
}   // namespace prefabManager
