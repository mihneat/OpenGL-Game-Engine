#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "utils/memory_utils.h"

using namespace component;

void GameManager::KeyPress(const int key, const int mods)
{
    if (key == GLFW_KEY_R) {
        ResetGame();
    }

    if (gameState == Start) {
        if (key == GLFW_KEY_DOWN && gameSpeed != LightningMcQueen) {
            gameSpeed = static_cast<GameSpeed>(static_cast<int>(gameSpeed) + 1);
        }

        if (key == GLFW_KEY_UP && gameSpeed != Snail) {
            gameSpeed = static_cast<GameSpeed>(static_cast<int>(gameSpeed) - 1);
        }
    }
}

void GameManager::MouseScroll(const int mouseX, const int mouseY, const int offsetX, const int offsetY)
{
    if (gameState == Start) {
        if (offsetY < 0 && gameSpeed != LightningMcQueen) {
            gameSpeed = static_cast<GameSpeed>(static_cast<int>(gameSpeed) + 1);
        }

        if (offsetY > 0 && gameSpeed != Snail) {
            gameSpeed = static_cast<GameSpeed>(static_cast<int>(gameSpeed) - 1);
        }
    }
}

void GameManager::MouseBtnPress(const int mouseX, const int mouseY,
    const int button, const int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (gameState == Start) {
            gameState = Playing;
        }
    }
}

void GameManager::ResetGame()
{
    // If the game is playing / ended, increment run count on reset
    if (gameState != Start) {
        ++runs;
    }

    // Reset manager variables
    gameState = Start;
    score = 0;

    // Reset all subscribed objects
    for (IResetable* resettable : resetables) {
        resettable->Reset();
    }
}

void GameManager::EndGame()
{
    // currentSkyColor = endSkyColor;
    gameState = Ended;

    // Write to stdout the results of the run
    std::cout << "  <=== Run #" << runs << " ===>\n";
    std::cout << " Presents collected: " << score << "\n";
    std::cout << " Highest achieved score: " << highScore << "\n\n\n";
}
