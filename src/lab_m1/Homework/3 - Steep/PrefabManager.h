#pragma once

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Transform.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/MeshRenderer.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/TextRenderer.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/UiPanel.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Shading/SteepShaderParams.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Camera/CameraFollow.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Camera/OrthoCameraFollow.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Objects/Obstacle.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Objects/ObjectSpawner.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Objects/UpdateLightPosition.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Player/GroundStick.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Player/PlayerController.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Time/Sun.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/ScoreDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/RunsDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/DistanceDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/HighScoreDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/LifeDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/StartRunDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/SpeedSelectionDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/GameOverDisplay.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/DirectionalLight.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/SpotLight.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/PointLight.h"

namespace component
{
    class SpriteRenderer;
}

namespace m1 {
    class Steep;
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
        static transform::Transform* CreateUI(m1::Steep* scene, transform::Transform* parent);
        static transform::Transform* CreateShaderParams(transform::Transform* parent);

    private:
        PrefabManager() {};
        ~PrefabManager() {};

    };
}   // namespace prefabManager
