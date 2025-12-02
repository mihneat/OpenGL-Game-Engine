#pragma once

class CUDAUtils
{
public:
    static void CheckCUDAError(const char *msg);
    static bool InitCUDA();
};
