#pragma once

#include <string>

#include "utils/text_utils.h"


namespace RESOURCE_PATH
{
    const std::string ROOT      = PATH_JOIN("assets");
    const std::string MODELS    = PATH_JOIN(ROOT, "models");
    const std::string TEXTURES  = PATH_JOIN(ROOT, "textures");
    const std::string SHADERS   = PATH_JOIN(ROOT, "shaders");
    const std::string FONTS     = PATH_JOIN(ROOT, "fonts");
}

namespace SOURCE_PATH
{
    const std::string MAIN          = PATH_JOIN("src", "main");
    const std::string M1            = PATH_JOIN("src", "lab_m1");
    const std::string M2            = PATH_JOIN("src", "lab_m2");
    const std::string EXTRA         = PATH_JOIN("src", "lab_extra");
}

namespace ENGINE_PATH
{
    const std::string GAME_ENGINE   = PATH_JOIN(SOURCE_PATH::M1, "GameEngine");
    const std::string ASSETS        = PATH_JOIN(GAME_ENGINE, "Assets");
}
