#pragma once

#include <unordered_set>

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace m1 {
    class Steep;
}

namespace managers
{
    class GameManager : public component::Component
    {
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
        GameManager(transform::Transform* transform, m1::Steep* scene) : component::Component(transform),
            scene(scene), score(0), highScore(0), runs(1), gameState(Start), gameSpeed(Medium),
            defaultSkyColor(glm::vec4(30, 5, 82, 255) / 255.0f),    // Blue-y: 30, 5, 82, 255    // Purple-y: 38, 6, 59, 255
            endSkyColor(glm::vec4(194, 21, 56, 76) / 255.0f)
        {
            currentSkyColor = defaultSkyColor;
        }

        static GameManager* gameManager;

        m1::Steep* scene;

        int score;
        int highScore;
        int runs;

    public:
        GameManager(GameManager& other) = delete;

        void operator=(const GameManager&) = delete;

        static GameManager* GetInstance(transform::Transform* transform = NULL, m1::Steep* scene = NULL);

        void KeyPress(const int key, const int mods);
        void MouseBtnPress(const int mouseX, const int mouseY,
            const int button, const int mods);

        int GetScore() { return score; }
        int GetHighScore() { return highScore; }
        int GetRuns() { return runs; }
        glm::vec4 GetSkyColor() { return currentSkyColor; };
        void SetSkyColor(glm::vec4 newSkyColor) { currentSkyColor = newSkyColor; };

        GameState GetGameState() { return gameState; }
        GameSpeed GetGameSpeed() { return gameSpeed; }

        m1::Steep* GetSceneReference();

        void UpdateScore(const int value) { score += value; highScore = std::max(highScore, score); }
        void AddResetable(component::IResetable* resetable) { resetables.insert(resetable); }
        void DeleteResetable(component::IResetable* resetable) { resetables.erase(resetable); };

        void ResetGame();
        void EndGame();

    private:
        GameState gameState;
        GameSpeed gameSpeed;

        glm::vec4 currentSkyColor;
        glm::vec4 defaultSkyColor;
        glm::vec4 endSkyColor;

        std::unordered_set<component::IResetable*> resetables;

    };
}   // namespace managers
