#pragma once
#include "utils/glm_utils.h"

namespace utils
{
    class FBOContainer
    {
    public:
        FBOContainer();
        ~FBOContainer();

        void Init(glm::ivec2 resolution);
        void Bind() const;

        glm::ivec2 GetResolution() const;
        void SetResolution(glm::ivec2 resolution);

        unsigned int GetColorTextureID() const;

        void UploadDataToTexture() const;

    private:
        void CreateFrameBuffer(glm::ivec2 resolution);
        void DeleteFrameBuffer();

        bool isInitialized = false;
        
        unsigned int fbo = 0;
        unsigned int colorTexture = 0;
        unsigned int depthTexture = 0;

        glm::ivec2 currentResolution = {0, 0};
    };
}
