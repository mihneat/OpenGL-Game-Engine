#include "lab_m1/Homework/3 - Steep/Managers/ShaderLoader.h"

using namespace std;
using namespace loaders;

void ShaderLoader::InitShaders(m1::Steep* scene)
{
    scene->LoadShader("Steep", "my_shaders/steep_shader/Steep.VS.glsl", "my_shaders/steep_shader/Steep.FS.glsl");
}
