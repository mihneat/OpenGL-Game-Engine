#pragma once

#include <unordered_set>

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace m1 {
    class GameEngine;
}

namespace component
{
    SERIALIZE_CLASS
    class GameManager : public component::Component
    {
        MARK_SERIALIZABLE
        
    public:
        enum GameState {
            Start,
            Playing,
            Ended
        };

        enum GameSpeed {
            Snail,
            Slow,
            Medium,
            Fast,
            Cheetah,
            LightningMcQueen
        };

    protected:
        GameManager(transform::Transform* transform, m1::GameEngine* scene) : component::Component(transform),
            scene(scene), score(0), highScore(0), runs(1), gameState(Start), gameSpeed(Medium),
            defaultSkyColor(glm::vec4(30, 5, 82, 255) / 255.0f),    // Blue-y: 30, 5, 82, 255    // Purple-y: 38, 6, 59, 255
            endSkyColor(glm::vec4(194, 21, 56, 76) / 255.0f)
        {
            currentSkyColor = defaultSkyColor;
        }

        static GameManager* gameManager;

        m1::GameEngine* scene;

        SERIALIZE_FIELD int score;
        SERIALIZE_FIELD int highScore;
        SERIALIZE_FIELD int runs;

    public:
        GameManager(GameManager& other) = delete;

        void operator=(const GameManager&) = delete;

        static GameManager* GetInstance(transform::Transform* transform = NULL, m1::GameEngine* scene = NULL);

        void KeyPress(const int key, const int mods) override;
        void MouseBtnPress(const int mouseX, const int mouseY,
            const int button, const int mods) override;
        void MouseScroll(const int mouseX, const int mouseY,
            const int offsetX, const int offsetY) override;

        int GetScore() { return score; }
        int GetHighScore() { return highScore; }
        int GetRuns() { return runs; }
        glm::vec4 GetSkyColor() { return currentSkyColor; }
        void SetSkyColor(glm::vec4 newSkyColor) { currentSkyColor = newSkyColor; }

        GameState GetGameState() { return gameState; }
        GameSpeed GetGameSpeed() { return gameSpeed; }

        m1::GameEngine* GetSceneReference();

        void UpdateScore(const int value) { score += value; highScore = std::max(highScore, score); }
        void AddResetable(component::IResetable* resetable) { resetables.insert(resetable); }
        void DeleteResetable(component::IResetable* resetable) { resetables.erase(resetable); }

        void ResetGame();
        void EndGame();

    private:
        GameState gameState;
        GameSpeed gameSpeed;

        SERIALIZE_FIELD glm::vec4 currentSkyColor;
        SERIALIZE_FIELD glm::vec4 defaultSkyColor;
        SERIALIZE_FIELD glm::vec4 endSkyColor;

        std::unordered_set<component::IResetable*> resetables;

    };
}   // namespace managers
