#include "CUDAUtils.h"

// Include CUDA
#include <cuda_runtime.h>

// Include helper functions
#include <helper_cuda.h>
#include <helper_functions.h>

// Code taken from a University course on GPGPU development
void CUDAUtils::CheckCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();

    if (cudaSuccess != err)
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString(err));
        getchar();
        exit(EXIT_FAILURE);
    }
}

bool CUDAUtils::InitCUDA()
{
#if __DEVICE_EMULATION__
    return true;
#else
    int count = 0;
    int i = 0;

    cudaGetDeviceCount(&count);
    if (count == 0) {
        fprintf(stderr, "No devices are available.\n");
        return false;
    }

    printf("There are %d devices.\n", count);

    for (i = 0; i < count; i++) {
        cudaDeviceProp prop;
        if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
            if (prop.major >= 1) {
                break;
            }
        }
    }
    if (i == count) {
        fprintf(stderr, "There are no devices which support CUDA.\n");
        return false;
    }
	
    cudaDeviceProp deviceProp;
    int devID = gpuGetMaxGflopsDeviceId();
    checkCudaErrors(cudaSetDevice(devID));

    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));
    printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID,
        deviceProp.name, deviceProp.major, deviceProp.minor);

    printf("CUDA initialized successfully!\n");
    return true;
#endif
}
