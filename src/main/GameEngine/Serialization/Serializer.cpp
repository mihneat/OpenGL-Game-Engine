﻿/**
 * !! File automatically generated through code.
 * 
 * Do NOT edit this file manually, it will be overriden after the next script reload!
 * 
 */

#include "Serializer.h"

#include "CppHeaderParser.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Camera.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Camera\CameraFollow.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Lights\DirectionalLight.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\DistanceDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Managers\GameManager.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\GameOverDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Player\GroundStick.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\HighScoreDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\LifeDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Lights\Light.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\MeshRenderer.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Objects\ObjectSpawner.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Objects\Obstacle.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Camera\OrthoCameraFollow.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Player\PlayerController.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Lights\PointLight.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\RunsDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\ScoreDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\SpeedSelectionDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Lights\SpotLight.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\UI\StartRunDisplay.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\Shading\SteepShaderParams.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Time\Sun.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\TextRenderer.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Rendering\UiPanel.h"
#include "..\src\main\GameEngine\ComponentBase\Components\Logic\Objects\UpdateLightPosition.h"

/**
 * Template(HEADER_PATH):
 *
 * #include "..\|HEADER_PATH|"
 *
 */

using namespace component;

typedef std::map<std::string, std::vector<SerializedField>> SerializedClassFields;

const std::vector<SerializedField>& Serializer::GetSerializedFieldsForClass(const std::string& className)
{
    static const SerializedClassFields serializedClassFields = {
        // The empty string is necessary
        {"", std::vector<SerializedField>{}},
        {"Camera", std::vector<SerializedField>{{"distanceToTarget", FieldTypeFloat},{"autoResize", FieldTypeBool},}},
        {"CameraFollow", std::vector<SerializedField>{{"followTarget", FieldTypeTransform},{"distanceToTarget", FieldTypeFloat},{"forwardFollowDistance", FieldTypeFloat},{"angleScale", FieldTypeFloat},{"retroAngleScale", FieldTypeFloat},{"isRetroCam", FieldTypeBool},}},
        {"DirectionalLight", std::vector<SerializedField>{{"type", FieldTypeInt},{"intensity", FieldTypeFloat},{"position", FieldTypeVec3},{"color", FieldTypeColour},{"direction", FieldTypeVec3},}},
        {"DistanceDisplay", std::vector<SerializedField>{{"player", FieldTypeTransform},}},
        {"GameManager", std::vector<SerializedField>{{"score", FieldTypeInt},{"highScore", FieldTypeInt},{"runs", FieldTypeInt},{"currentSkyColor", FieldTypeColour},{"defaultSkyColor", FieldTypeColour},{"endSkyColor", FieldTypeColour},}},
        {"GameOverDisplay", std::vector<SerializedField>{}},
        {"GroundStick", std::vector<SerializedField>{{"player", FieldTypeTransform},{"offset", FieldTypeVec3},}},
        {"HighScoreDisplay", std::vector<SerializedField>{}},
        {"LifeDisplay", std::vector<SerializedField>{{"player", FieldTypeTransform},}},
        {"Light", std::vector<SerializedField>{{"type", FieldTypeInt},{"intensity", FieldTypeFloat},{"position", FieldTypeVec3},{"color", FieldTypeColour},{"direction", FieldTypeVec3},}},
        {"MeshRenderer", std::vector<SerializedField>{{"type", FieldTypeEnum, "MeshEnum"},{"color", FieldTypeColour},{"meshScale", FieldTypeVec3},{"debugOnly", FieldTypeBool},{"renderInWorldSpace", FieldTypeBool},{"layer", FieldTypeEnum, "LayerEnum"},{"texture", FieldTypeGUID},{"texScale", FieldTypeVec2},{"material", FieldTypeGUID},}},
        {"ObjectSpawner", std::vector<SerializedField>{{"player", FieldTypeTransform},{"spawnTimeInterval", FieldTypeVec2},{"spawnDistance", FieldTypeFloat},{"spawnSpread", FieldTypeFloat},}},
        {"Obstacle", std::vector<SerializedField>{{"collisionRadius", FieldTypeFloat},{"isHazard", FieldTypeBool},}},
        {"OrthoCameraFollow", std::vector<SerializedField>{{"followTarget", FieldTypeTransform},{"isFixed", FieldTypeBool},{"zoom", FieldTypeFloat},{"minDimensions", FieldTypeVec2},{"maxDimensions", FieldTypeVec2},{"fixedDimensions", FieldTypeVec2},{"zoomSpeed", FieldTypeFloat},}},
        {"PlayerController", std::vector<SerializedField>{{"acceleration", FieldTypeFloat},{"speed", FieldTypeFloat},{"maxSpeed", FieldTypeFloat},{"playerBody", FieldTypeTransform},{"tilt", FieldTypeFloat},{"tiltFactor", FieldTypeFloat},{"lives", FieldTypeInt},{"maxLives", FieldTypeInt},{"initialPosition", FieldTypeVec3},}},
        {"PointLight", std::vector<SerializedField>{{"type", FieldTypeInt},{"intensity", FieldTypeFloat},{"position", FieldTypeVec3},{"color", FieldTypeColour},{"direction", FieldTypeVec3},}},
        {"RunsDisplay", std::vector<SerializedField>{}},
        {"ScoreDisplay", std::vector<SerializedField>{}},
        {"SpeedSelectionDisplay", std::vector<SerializedField>{}},
        {"SpotLight", std::vector<SerializedField>{{"type", FieldTypeInt},{"intensity", FieldTypeFloat},{"position", FieldTypeVec3},{"color", FieldTypeColour},{"direction", FieldTypeVec3},}},
        {"StartRunDisplay", std::vector<SerializedField>{}},
        {"SteepShaderParams", std::vector<SerializedField>{{"groundMat", FieldTypeGUID},}},
        {"Sun", std::vector<SerializedField>{{"speed", FieldTypeFloat},{"nightSpeed", FieldTypeFloat},}},
        {"TextRenderer", std::vector<SerializedField>{{"scale", FieldTypeFloat},{"color", FieldTypeColour},{"text", FieldTypeString},{"position", FieldTypeVec2},}},
        {"UiPanel", std::vector<SerializedField>{{"anchorTop", FieldTypeBool},{"anchorRight", FieldTypeBool},{"offsetTop", FieldTypeFloat},{"offsetRight", FieldTypeFloat},}},
        {"UpdateLightPosition", std::vector<SerializedField>{}},

        /**
         * Template(CLASS_NAME, SERIALIZED_FIELD[]):
         *
         * {"|CLASS_NAME|", std::vector<SerializedField>{||FOR_EACH SERIALIZED_FIELD|| {"|SERIALIZED_FIELD.ATTRIBUTE_NAME|", FieldType|SERIALIZED_FIELD.TYPE|} ||END_FOR_EACH||}},
         *
         */
    };

    // Try to find the class
    const auto it = serializedClassFields.find(className);
    if (it != serializedClassFields.end())
        return it->second;

    return serializedClassFields.find("")->second;
}

void* Serializer::GetAttributeReference(Component* instance, const std::string& attributeName)
{
    if (dynamic_cast<Camera*>(instance) != nullptr)
    {
        Camera* obj = dynamic_cast<Camera*>(instance);

        if (attributeName == "distanceToTarget")
            return &obj->distanceToTarget;

        if (attributeName == "autoResize")
            return &obj->autoResize;

        return nullptr;
    }

    if (dynamic_cast<CameraFollow*>(instance) != nullptr)
    {
        CameraFollow* obj = dynamic_cast<CameraFollow*>(instance);

        if (attributeName == "followTarget")
            return &obj->followTarget;

        if (attributeName == "distanceToTarget")
            return &obj->distanceToTarget;

        if (attributeName == "forwardFollowDistance")
            return &obj->forwardFollowDistance;

        if (attributeName == "angleScale")
            return &obj->angleScale;

        if (attributeName == "retroAngleScale")
            return &obj->retroAngleScale;

        if (attributeName == "isRetroCam")
            return &obj->isRetroCam;

        return nullptr;
    }

    if (dynamic_cast<DirectionalLight*>(instance) != nullptr)
    {
        DirectionalLight* obj = dynamic_cast<DirectionalLight*>(instance);

        if (attributeName == "type")
            return &obj->type;

        if (attributeName == "intensity")
            return &obj->intensity;

        if (attributeName == "position")
            return &obj->position;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "direction")
            return &obj->direction;

        return nullptr;
    }

    if (dynamic_cast<DistanceDisplay*>(instance) != nullptr)
    {
        DistanceDisplay* obj = dynamic_cast<DistanceDisplay*>(instance);

        if (attributeName == "player")
            return &obj->player;

        return nullptr;
    }

    if (dynamic_cast<GameManager*>(instance) != nullptr)
    {
        GameManager* obj = dynamic_cast<GameManager*>(instance);

        if (attributeName == "score")
            return &obj->score;

        if (attributeName == "highScore")
            return &obj->highScore;

        if (attributeName == "runs")
            return &obj->runs;

        if (attributeName == "currentSkyColor")
            return &obj->currentSkyColor;

        if (attributeName == "defaultSkyColor")
            return &obj->defaultSkyColor;

        if (attributeName == "endSkyColor")
            return &obj->endSkyColor;

        return nullptr;
    }

    if (dynamic_cast<GameOverDisplay*>(instance) != nullptr)
    {
        GameOverDisplay* obj = dynamic_cast<GameOverDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<GroundStick*>(instance) != nullptr)
    {
        GroundStick* obj = dynamic_cast<GroundStick*>(instance);

        if (attributeName == "player")
            return &obj->player;

        if (attributeName == "offset")
            return &obj->offset;

        return nullptr;
    }

    if (dynamic_cast<HighScoreDisplay*>(instance) != nullptr)
    {
        HighScoreDisplay* obj = dynamic_cast<HighScoreDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<LifeDisplay*>(instance) != nullptr)
    {
        LifeDisplay* obj = dynamic_cast<LifeDisplay*>(instance);

        if (attributeName == "player")
            return &obj->player;

        return nullptr;
    }

    if (dynamic_cast<Light*>(instance) != nullptr)
    {
        Light* obj = dynamic_cast<Light*>(instance);

        if (attributeName == "type")
            return &obj->type;

        if (attributeName == "intensity")
            return &obj->intensity;

        if (attributeName == "position")
            return &obj->position;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "direction")
            return &obj->direction;

        return nullptr;
    }

    if (dynamic_cast<MeshRenderer*>(instance) != nullptr)
    {
        MeshRenderer* obj = dynamic_cast<MeshRenderer*>(instance);

        if (attributeName == "type")
            return &obj->type;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "meshScale")
            return &obj->meshScale;

        if (attributeName == "debugOnly")
            return &obj->debugOnly;

        if (attributeName == "renderInWorldSpace")
            return &obj->renderInWorldSpace;

        if (attributeName == "layer")
            return &obj->layer;

        if (attributeName == "texture")
            return &obj->texture;

        if (attributeName == "texScale")
            return &obj->texScale;

        if (attributeName == "material")
            return &obj->material;

        return nullptr;
    }

    if (dynamic_cast<ObjectSpawner*>(instance) != nullptr)
    {
        ObjectSpawner* obj = dynamic_cast<ObjectSpawner*>(instance);

        if (attributeName == "player")
            return &obj->player;

        if (attributeName == "spawnTimeInterval")
            return &obj->spawnTimeInterval;

        if (attributeName == "spawnDistance")
            return &obj->spawnDistance;

        if (attributeName == "spawnSpread")
            return &obj->spawnSpread;

        return nullptr;
    }

    if (dynamic_cast<Obstacle*>(instance) != nullptr)
    {
        Obstacle* obj = dynamic_cast<Obstacle*>(instance);

        if (attributeName == "collisionRadius")
            return &obj->collisionRadius;

        if (attributeName == "isHazard")
            return &obj->isHazard;

        return nullptr;
    }

    if (dynamic_cast<OrthoCameraFollow*>(instance) != nullptr)
    {
        OrthoCameraFollow* obj = dynamic_cast<OrthoCameraFollow*>(instance);

        if (attributeName == "followTarget")
            return &obj->followTarget;

        if (attributeName == "isFixed")
            return &obj->isFixed;

        if (attributeName == "zoom")
            return &obj->zoom;

        if (attributeName == "minDimensions")
            return &obj->minDimensions;

        if (attributeName == "maxDimensions")
            return &obj->maxDimensions;

        if (attributeName == "fixedDimensions")
            return &obj->fixedDimensions;

        if (attributeName == "zoomSpeed")
            return &obj->zoomSpeed;

        return nullptr;
    }

    if (dynamic_cast<PlayerController*>(instance) != nullptr)
    {
        PlayerController* obj = dynamic_cast<PlayerController*>(instance);

        if (attributeName == "acceleration")
            return &obj->acceleration;

        if (attributeName == "speed")
            return &obj->speed;

        if (attributeName == "maxSpeed")
            return &obj->maxSpeed;

        if (attributeName == "playerBody")
            return &obj->playerBody;

        if (attributeName == "tilt")
            return &obj->tilt;

        if (attributeName == "tiltFactor")
            return &obj->tiltFactor;

        if (attributeName == "lives")
            return &obj->lives;

        if (attributeName == "maxLives")
            return &obj->maxLives;

        if (attributeName == "initialPosition")
            return &obj->initialPosition;

        return nullptr;
    }

    if (dynamic_cast<PointLight*>(instance) != nullptr)
    {
        PointLight* obj = dynamic_cast<PointLight*>(instance);

        if (attributeName == "type")
            return &obj->type;

        if (attributeName == "intensity")
            return &obj->intensity;

        if (attributeName == "position")
            return &obj->position;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "direction")
            return &obj->direction;

        return nullptr;
    }

    if (dynamic_cast<RunsDisplay*>(instance) != nullptr)
    {
        RunsDisplay* obj = dynamic_cast<RunsDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<ScoreDisplay*>(instance) != nullptr)
    {
        ScoreDisplay* obj = dynamic_cast<ScoreDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<SpeedSelectionDisplay*>(instance) != nullptr)
    {
        SpeedSelectionDisplay* obj = dynamic_cast<SpeedSelectionDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<SpotLight*>(instance) != nullptr)
    {
        SpotLight* obj = dynamic_cast<SpotLight*>(instance);

        if (attributeName == "type")
            return &obj->type;

        if (attributeName == "intensity")
            return &obj->intensity;

        if (attributeName == "position")
            return &obj->position;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "direction")
            return &obj->direction;

        return nullptr;
    }

    if (dynamic_cast<StartRunDisplay*>(instance) != nullptr)
    {
        StartRunDisplay* obj = dynamic_cast<StartRunDisplay*>(instance);

        return nullptr;
    }

    if (dynamic_cast<SteepShaderParams*>(instance) != nullptr)
    {
        SteepShaderParams* obj = dynamic_cast<SteepShaderParams*>(instance);

        if (attributeName == "groundMat")
            return &obj->groundMat;

        return nullptr;
    }

    if (dynamic_cast<Sun*>(instance) != nullptr)
    {
        Sun* obj = dynamic_cast<Sun*>(instance);

        if (attributeName == "speed")
            return &obj->speed;

        if (attributeName == "nightSpeed")
            return &obj->nightSpeed;

        return nullptr;
    }

    if (dynamic_cast<TextRenderer*>(instance) != nullptr)
    {
        TextRenderer* obj = dynamic_cast<TextRenderer*>(instance);

        if (attributeName == "scale")
            return &obj->scale;

        if (attributeName == "color")
            return &obj->color;

        if (attributeName == "text")
            return &obj->text;

        if (attributeName == "position")
            return &obj->position;

        return nullptr;
    }

    if (dynamic_cast<UiPanel*>(instance) != nullptr)
    {
        UiPanel* obj = dynamic_cast<UiPanel*>(instance);

        if (attributeName == "anchorTop")
            return &obj->anchorTop;

        if (attributeName == "anchorRight")
            return &obj->anchorRight;

        if (attributeName == "offsetTop")
            return &obj->offsetTop;

        if (attributeName == "offsetRight")
            return &obj->offsetRight;

        return nullptr;
    }

    if (dynamic_cast<UpdateLightPosition*>(instance) != nullptr)
    {
        UpdateLightPosition* obj = dynamic_cast<UpdateLightPosition*>(instance);

        return nullptr;
    }


    /**
     * Template(CLASS_NAME, ATTRIBUTE_NAME[]):
     *
     * if (dynamic_cast<|CLASS_NAME|*>(instance) != nullptr)
     * {
     *      |CLASS_NAME|* obj = dynamic_cast<|CLASS_NAME|*>(instance);
     *
     *      ||FOR_EACH ATTRIBUTE_NAME||
     *      if (attributeName == "|ATTRIBUTE_NAME|")
     *          return &obj->|ATTRIBUTE_NAME|;
     *
     *      ||END_FOR_EACH||
     *      return nullptr;
     * }
     *
     */

    return nullptr;
}

Component* Serializer::ComponentFactory(const std::string& className, transform::Transform* parent)
{
    if (className == "Camera") return new Camera(parent);
    if (className == "CameraFollow") return new CameraFollow(parent);
    if (className == "DirectionalLight") return new DirectionalLight(parent);
    if (className == "DistanceDisplay") return new DistanceDisplay(parent);
    if (className == "GameManager") return new GameManager(parent);
    if (className == "GameOverDisplay") return new GameOverDisplay(parent);
    if (className == "GroundStick") return new GroundStick(parent);
    if (className == "HighScoreDisplay") return new HighScoreDisplay(parent);
    if (className == "LifeDisplay") return new LifeDisplay(parent);
    if (className == "Light") return new Light(parent);
    if (className == "MeshRenderer") return new MeshRenderer(parent);
    if (className == "ObjectSpawner") return new ObjectSpawner(parent);
    if (className == "Obstacle") return new Obstacle(parent);
    if (className == "OrthoCameraFollow") return new OrthoCameraFollow(parent);
    if (className == "PlayerController") return new PlayerController(parent);
    if (className == "PointLight") return new PointLight(parent);
    if (className == "RunsDisplay") return new RunsDisplay(parent);
    if (className == "ScoreDisplay") return new ScoreDisplay(parent);
    if (className == "SpeedSelectionDisplay") return new SpeedSelectionDisplay(parent);
    if (className == "SpotLight") return new SpotLight(parent);
    if (className == "StartRunDisplay") return new StartRunDisplay(parent);
    if (className == "SteepShaderParams") return new SteepShaderParams(parent);
    if (className == "Sun") return new Sun(parent);
    if (className == "TextRenderer") return new TextRenderer(parent);
    if (className == "UiPanel") return new UiPanel(parent);
    if (className == "UpdateLightPosition") return new UpdateLightPosition(parent);

    /**
     * Template(CLASS_NAME):
     *
     * if (className == "|CLASS_NAME|") return new |CLASS_NAME|(parent);
     * 
     */
    
    return nullptr;
}

const std::vector<std::pair<std::string, int>>& Serializer::GetValuePairsForEnum(const std::string& enumName)
{
    static const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> enumPairs = {
        {"", {}},
        {"MeshEnum", {{"Square", 0},{"FragmentedSquare", 1},{"Circle", 2},{"Cube", 3},{"CubeMesh", 4},{"Sphere", 5},{"ZeldaHeart", 6},{"Cone", 7},{"None", 8},}},
        {"LayerEnum", {{"Default", 0},{"UI", 1},}},

        /**
         * Template(ENUM_NAME, ENUM_VALUES[])
         *
         * {"|ENUM_NAME|", {||FOR_EACH ENUM_VALUES||{"|.NAME|", |.INDEX|}, ||END FOR_EACH||}}
         *
         */
    };

    // Try to find the enum
    const auto it = enumPairs.find(enumName);
    if (it != enumPairs.end())
        return it->second;

    return enumPairs.find("")->second;
}
