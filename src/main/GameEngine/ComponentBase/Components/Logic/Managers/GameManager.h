#pragma once

#include <iostream>
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
    class GameManager : public Component
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

        GameManager(transform::Transform* transform) : Component(transform) { }

    protected:
        SERIALIZE_FIELD int score = 0;
        SERIALIZE_FIELD int highScore = 0;
        SERIALIZE_FIELD int runs = 1;

    public:
        void KeyPress(const int key, const int mods) override;
        void MouseBtnPress(const int mouseX, const int mouseY,
            const int button, const int mods) override;
        void MouseScroll(const int mouseX, const int mouseY,
            const int offsetX, const int offsetY) override;

        int GetScore() { return score; }
        int GetHighScore() { return highScore; }
        int GetRuns() { return runs; }

        GameState GetGameState() { return gameState; }
        GameSpeed GetGameSpeed() { return gameSpeed; }

        void UpdateScore(const int value) { score += value; highScore = std::max(highScore, score); }
        void AddResetable(IResetable* resetable) { resetables.insert(resetable); }
        void DeleteResetable(IResetable* resetable) { resetables.erase(resetable); }

        void ResetGame();
        void EndGame();

    private:
        GameState gameState = Start;
        GameSpeed gameSpeed = Medium;

        // Blue-y: 30, 5, 82, 255, Purple-y: 38, 6, 59, 255
        SERIALIZE_FIELD glm::vec4 currentSkyColor = glm::vec4(30, 5, 82, 255) / 255.0f;
        SERIALIZE_FIELD glm::vec4 defaultSkyColor = glm::vec4(30, 5, 82, 255) / 255.0f;
        SERIALIZE_FIELD glm::vec4 endSkyColor = glm::vec4(194, 21, 56, 76) / 255.0f;

        std::unordered_set<IResetable*> resetables;

    };
}   // namespace managers
