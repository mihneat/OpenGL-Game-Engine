#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace managers;
using namespace component;

GameManager* GameManager::gameManager = nullptr;

GameManager* GameManager::GetInstance(transform::Transform* transform, m1::Steep* scene)
{
    if (gameManager == nullptr) {
        gameManager = new GameManager(transform, scene);
    }
    return gameManager;
}

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

void GameManager::MouseBtnPress(const int mouseX, const int mouseY,
    const int button, const int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (gameState == Start) {
            gameState = Playing;
        }
    }
}

m1::Steep* GameManager::GetSceneReference()
{
    return scene;
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
    for (IResetable* resetable : resetables) {
        resetable->Reset();
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
