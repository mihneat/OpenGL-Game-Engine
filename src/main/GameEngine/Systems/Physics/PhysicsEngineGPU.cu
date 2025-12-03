#include "PhysicsEngineGPU.cuh"

// Kernel executed on CUDA device
__global__ void ProcessCollisionsForCollider(float* pixels_d, int screenWidth, int screenHeight)
{
    // Identificarea pozitiei exacte a thread-ului
    int x, y;
    x = (blockIdx.x * blockDim.x + threadIdx.x);
    y = (blockIdx.y * blockDim.y + threadIdx.y);

    int indice = 3 * (y*screenWidth + x);

    // TODO
    // pixels_d[?] = ?

}

extern "C"
cudaError_t ProcessCollisionsOnGPU(float* pixels, int screenWidth, int screenHeight, dim3 DIM_GRID, dim3 DIM_BLOCK)
{
    ProcessCollisionsForCollider <<< DIM_GRID, DIM_BLOCK >>> (pixels, screenWidth, screenHeight);

    return cudaGetLastError();
}