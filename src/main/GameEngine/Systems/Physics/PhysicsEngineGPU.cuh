#pragma once

__global__ void ProcessCollisionsForCollider(float *pixels, int screenWidth, int screenHeight);

extern "C"
cudaError_t ProcessCollisionsOnGPU(float* pixels, int screenWidth, int screenHeight, dim3 DIM_GRID, dim3 DIM_BLOCK);
