#pragma once

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include <iostream>
#include <unordered_map>

namespace m1 {
    class Steep;
}

namespace loaders
{
    class ShaderLoader
    {
    private:
        ShaderLoader() { }
        virtual ~ShaderLoader() { }

    public:
        static void InitShaders(m1::Steep* scene);
    };
}   // namespace loaders
